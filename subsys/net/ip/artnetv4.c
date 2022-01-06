// TODO: check for broadcast permissions
// TODO: dhcp config
// TODO: default ip settings

#include <logging/log.h>
LOG_MODULE_REGISTER(net_artnet4, CONFIG_NET_ARTNETV4_LOG_LEVEL);

#include <stdio.h>
#include <sys/byteorder.h>

#include <net/net_core.h>
#include <net/net_pkt.h>
#include <net/net_if.h>
#include <net/net_mgmt.h>
#include "net_private.h"

#include <net/udp.h>
#include "udp_internal.h"

#include "ipv4.h"
#include "artnetv4.h"

/* supported packets
 * - art_poll{,_reply}
 * - art_ip_prog{,_reply}
 * - art_address ???
 * - art_command
 * - !! art_trigger
 * - art_dmx
 * - art_sync
 * - art_nzs
 * - !! art_vlc
 * - !! art_input
 * - art_firmware_{master,reply}
 * - !! art_tod_request
 * - !! art_tod_data
 * - !! art_tod_control
 * - !! art_rdm
 * - !! art_rdm_sub
 */

/* Every ArtPollReply packet has some static fields which are not mutable
 * in time hence simple initialize a template once a memcpy() it later.
 */
static struct artnet_art_poll_reply g_poll_reply;

/* Art-Net magic cookie. Used to verify data integrity. */
static const uint8_t magic_cookie[8] = {
	'A', 'r', 't', '-', 'N', 'e', 't', 0x00
};

/* Art-Net v4 protocol version low byte. */
static const uint8_t protocol_version = 14;

/* Art-Net controller sent settings */
static uint8_t g_talk_to_me = 0x00;
static uint8_t g_priority = 0x00;

static inline bool artnet_is_protocol_version_correct(uint8_t prot_ver[2])
{
	if (prot_ver[0] == 0 && prot_ver[1] == protocol_version) {
		return true;
	} else {
		return false;
	}
}

static void artnet_initialize_static_members(void)
{
	/* Initialize static members of ArtPollReply message struct */
	memset(&g_poll_reply, 0, sizeof(g_poll_reply));
	g_poll_reply.port = htons(ARTNET_PORT);
	sys_put_le16(CONFIG_NET_ARTNETV4_FIRMWARE_REVISION,
	  	     g_poll_reply.vers_info);
	sys_put_le16(CONFIG_NET_ARTNETV4_OEM_CODE, g_poll_reply.oem);
	sys_put_le16(CONFIG_NET_ARTNETV4_ESTA_CODE, g_poll_reply.esta_man);
}

/* Any Art-Net packet issued by Node has a default header. */
static struct net_pkt* artnet_pkt_with_hdr(struct net_if *iface,
					   struct in_addr *src_addr,
					   struct in_addr *dst_addr,
					   uint16_t op_code,
					   size_t pkt_body_len)
{
	struct net_pkt *pkt;
	size_t pkt_size;

       	pkt_size = sizeof(struct artnet_hdr) + pkt_body_len;
	pkt = net_pkt_alloc_with_buffer(iface, pkt_size,
					AF_INET, IPPROTO_UDP, K_FOREVER);

	net_pkt_set_ipv4_ttl(pkt, 0xFF);

	if (!src_addr) {
		src_addr = net_ipv4_unspecified_address();
	}

	if (!dst_addr) {
		dst_addr = net_ipv4_broadcast_address();
	}

	if (net_ipv4_create(pkt, src_addr, dst_addr) ||
	    net_udp_create(pkt, htons(ARTNET_PORT), htons(ARTNET_PORT))) {
		goto fail;
	}

	if (net_pkt_write(pkt, (void *)magic_cookie,
			  ARRAY_SIZE(magic_cookie)) ||
	    net_pkt_write_le16(pkt, op_code)) {
		goto fail;
	}
	return pkt;
fail:
	net_pkt_unref(pkt);
	return NULL;
}

/* Art-Net ArtPoll reply packet. Mostly it has static data which can be known
 * at compile time. Although it has some dynamic data which requires some
 * data/events exchange between application and protocol code, e.g. a node
 * should be capable of changing indicators state upon Controller request,
 * sending some diagnostics data, etc.
 */
static void artnet_send_art_poll_reply(struct net_if *iface)
{
	NET_PKT_DATA_ACCESS_DEFINE(artnet_access, struct artnet_art_poll_reply);
	static uint32_t sent_count = 0;

	struct artnet_art_poll_reply *pr;
	struct net_pkt *pkt;
	struct in_addr *src_addr;

	src_addr = net_if_ipv4_get_global_addr(iface, NET_ADDR_PREFERRED);
	if (!src_addr) {
		NET_DBG("poll reply: no ip4 addr");
		return;
	}

	pkt = artnet_pkt_with_hdr(iface, src_addr, NULL, ARTNET_OP_POLL_REPLY,
				  sizeof(struct artnet_art_poll_reply));
	if (!pkt) {
		NET_DBG("poll reply: alloc failed");
		return;
	}

	pr = (struct artnet_art_poll_reply *)net_pkt_get_data(pkt,
							      &artnet_access);
	if (!pr) {
		NET_DBG("poll reply: pkt get failed");
		goto fail;
	}

	memcpy(pr, &g_poll_reply, sizeof(struct artnet_art_poll_reply));

	sys_put_le32(src_addr->s_addr, pr->ip_addr);
	sys_put_le32(src_addr->s_addr, pr->bind_ip);
	pr->bind_idx = 1;
	pr->net_switch = 0;
	pr->sub_switch = 0;
	pr->status1 = 0;
#if defined(CONFIG_NET_DHCPV4) && defined(CONFIG_NET_NATIVE_IPV4)
	pr->status2 = ARTNET_STATUS2_DHCP_CAP;
#endif /* CONFIG_NET_DHCPV4 */
	snprintf(pr->node_report, ARTNET_NODE_REPORT_LEN, "#%04x [%d] %s",
		 ARTNET_RC_DEBUG, sent_count, "debugging");
       	pr->num_ports[0] = 0;
	pr->num_ports[1] = 1;
	pr->port_types[0] = BIT(7) | 0x05;
	pr->sw_out[0] = 0x03;

	if (net_pkt_set_data(pkt, &artnet_access)) {
		NET_DBG("poll reply: couldn't send");
		goto fail;
	}

	net_pkt_cursor_init(pkt);

	net_ipv4_finalize(pkt, IPPROTO_UDP);

	if (net_send_data(pkt) < 0) {
		goto fail;
	}

	sent_count++;

	NET_DBG("poll reply: data sent");
	return;

fail:
	net_pkt_unref(pkt);
	NET_DBG("poll reply: failed");
}

/* Art-Net controller constantly polls network for any Art-Net node present to
 * report its status and stuff each 1-5 seconds. */
static void artnet_handle_art_poll(struct net_if *iface, struct net_pkt *pkt)
{
	NET_PKT_DATA_ACCESS_DEFINE(artnet_access, struct artnet_art_poll);
	struct artnet_art_poll *art_poll;

	art_poll = (struct artnet_art_poll *)net_pkt_get_data(pkt, &artnet_access);
	if (!art_poll) {
		NET_DBG("not an art_poll pkt");
		return;
	}

	g_talk_to_me = art_poll->talk_to_me;
	g_priority = art_poll->priority;
	NET_DBG("art poll: [talk=0x%x, pri=0x%x]", g_talk_to_me, g_priority);

	artnet_send_art_poll_reply(iface);
}

/* Reply to ArtIpProg packet */
static void artnet_send_art_ip_prog_reply(struct net_if *iface,
					  union net_ip_header *ip_hdr)
{
	NET_PKT_DATA_ACCESS_DEFINE(artnet_access, struct artnet_art_ip_prog_reply);
	struct artnet_art_ip_prog_reply *ipr;
	struct net_pkt *pkt;
	struct in_addr *src_addr;
	struct in_addr *dst_addr;

	src_addr = net_if_ipv4_get_global_addr(iface, NET_ADDR_PREFERRED);
	if (!src_addr) {
		NET_DBG("src addr: fail");
		return;
	}

	/* memcpy(dst_addr->s4_addr, ip_hdr->ipv4->src, NET_IPV4_ADDR_LEN); */
	dst_addr = net_ipv4_broadcast_address();

	pkt = artnet_pkt_with_hdr(iface, src_addr, dst_addr, ARTNET_OP_IP_PROG_REPLY,
				  sizeof(struct artnet_art_ip_prog_reply));
	if (!pkt) {
		NET_DBG("pkt: failed");
		return;
	}

	ipr = (struct artnet_art_ip_prog_reply *)net_pkt_get_data(pkt, &artnet_access);
	if (!ipr) {
		NET_DBG("get data: failed");
		goto fail;
	}

	memset(ipr, 0, sizeof(struct artnet_art_ip_prog_reply));
	ipr->prot_ver[0] = 0;
	ipr->prot_ver[1] = protocol_version;

	sys_put_le32(src_addr->s_addr, ipr->prog_ip);
	sys_put_le32(iface->config.ip.ipv4->netmask.s_addr, ipr->prog_sm);

#if defined(CONFIG_NET_DHCPV4) && defined(CONFIG_NET_NATIVE_IPV4)
	if (iface->cpnfig.dhcpv4.state != NET_DHCPV4_DISABLED) {
		ipr->status = 1 << 6;
	}
#endif /* CONFIG_NET_DHCPV4 */

	if (net_pkt_set_data(pkt, &artnet_access)) {
		NET_DBG("set data: failed");
		goto fail;
	}

	net_pkt_cursor_init(pkt);

	net_ipv4_finalize(pkt, IPPROTO_UDP);

	if (net_send_data(pkt) < 0) {
		NET_DBG("send: failed");
		goto fail;
	}

	return;
fail:
	net_pkt_unref(pkt);
	return;
}

/* Art-Net Controller should be capable of reprogramming Node IP settings
 * via Art-Net protocol. The function is used to handle it.
 */
static void artnet_handle_art_ip_prog(struct net_if *iface,
				      struct net_pkt *pkt,
				      union net_ip_header *ip_hdr)
{
	NET_PKT_DATA_ACCESS_DEFINE(artnet_access, struct artnet_art_ip_prog);
	struct artnet_art_ip_prog *ipp;
	struct in_addr addr;

	ipp = (struct artnet_art_ip_prog *)net_pkt_get_data(pkt, &artnet_access);
	if (!ipp) {
		NET_DBG("get data: failed");
		return;
	}

	if (!artnet_is_protocol_version_correct(ipp->prot_ver)) {
		NET_DBG("prot vers: incorrect");
		return;
	}

	NET_DBG("command: 0x%02x", ipp->command);
	if (!(ipp->command & ARTNET_IP_COMMAND_ENABLE)) {
		NET_DBG("programming disabled");
		return;
	}

	if (ipp->command & ARTNET_IP_COMMAND_DHCP_ENABLE) {
		NET_DBG("enable dhcp");
		return;
	} else if (ipp->command & ARTNET_IP_COMMAND_SET_DEFAULT) {
		NET_DBG("set default");
		return;
	} else {
		if (ipp->command & ARTNET_IP_COMMAND_SET_IP_ADDR) {
			NET_DBG("set ip addr");
			memcpy(addr.s4_addr, ipp->prog_ip,
					NET_IPV4_ADDR_SIZE);
			net_if_ipv4_addr_rm(iface, net_if_ipv4_get_global_addr(iface, NET_ADDR_PREFERRED));
			if (!net_if_ipv4_addr_add(iface, &addr, NET_ADDR_MANUAL, 0)) {
				NET_DBG("failed to add addr");
				return;
			}
		}

		if (ipp->command & ARTNET_IP_COMMAND_SET_SUBNET) {
			NET_DBG("set subnet mask");
			memcpy(addr.s4_addr, ipp->prog_sm,
					NET_IPV4_ADDR_SIZE);
			net_if_ipv4_set_netmask(iface, &addr);
		}
	}

	artnet_send_art_ip_prog_reply(iface, ip_hdr);
}

/* Actual Art-Net devices payload data. I need to pass it to application
 * with as small overhead as possible due to high load:
 *
 * it's kinda natural for Art-Net node device to receive data at up to 44Hz
 * rate per port; and one node can utilize up to tens of ports (i.e. some
 * widely known Art-Net controllers use 16/32 ports per device)
 */
static void artnet_handle_art_dmx(struct net_if *iface,
				  struct net_pkt *pkt)
{
	NET_PKT_DATA_ACCESS_DEFINE(artnet_access, struct artnet_art_dmx);
	struct artnet_art_dmx *ad;

	uint16_t len;
	uint16_t uni;
	uint8_t sequence;

	ad = (struct artnet_payload *)net_pkt_get_data(pkt, &artnet_access);
	if (!ad) {
		NET_DBG("no data");
		return;
	}

	if (!artnet_is_protocol_version_correct(ad->prot_ver)) {
		NET_DBG("version incorrect");
		return;
	}

	sequence = ad->sequence;
	uni = sys_get_le16(&ad->sub_uni);
	len = sys_get_le16(ad->length);
	NET_DBG("artnet: [seq=%d,phys=%d,uni=%d,len=%d",
		sequence, ad->physical, uni, len);
	net_mgmt_event_notify_with_info(NET_EVENT_IPV4_ARTNET_OUT, iface,
					ad->data, len);
}

/* Here I do check Art-Net header and decide on how the node would response
 * to the received packet. Basically it's a simple switch over op_code.
 */
static enum net_verdict net_artnetv4_input(struct net_conn *conn,
					   struct net_pkt *pkt,
					   union net_ip_header *ip_hdr,
					   union net_proto_header *proto_hdr,
					   void *user_data)
{
	NET_PKT_DATA_ACCESS_DEFINE(artnet_access, struct artnet_hdr);
	enum net_verdict verdict = NET_DROP;
	struct artnet_hdr *hdr;
	struct net_if *iface;
	uint16_t op_code;

	net_pkt_cursor_init(pkt);

	iface = net_pkt_iface(pkt);
	if (!iface) {
		NET_DBG("no iface");
		return NET_DROP;
	}

	if (net_pkt_skip(pkt, NET_IPV4UDPH_LEN)) {
		return NET_DROP;
	}

	hdr = (struct artnet_hdr *)net_pkt_get_data(pkt, &artnet_access);
	if (!hdr) {
		return NET_DROP;
	}

	/* Data integrity is achieved by comparing ID field of Art-Net packet
	 * as per Art-Net 4 specification. */
	if (memcmp(hdr->id, magic_cookie, sizeof(magic_cookie))) {
		NET_DBG("Skipping non artnet pkt");
		return NET_CONTINUE;
	}
	op_code = sys_get_le16((uint8_t *)&hdr->op_code);
	net_pkt_acknowledge_data(pkt, &artnet_access);

	switch (op_code) {
	case ARTNET_OP_POLL:
		artnet_handle_art_poll(iface, pkt);
		break;
	case ARTNET_OP_OUTPUT:
		NET_DBG("Received ArtDmx pkt");
		artnet_handle_art_dmx(iface, pkt);
		break;
	case ARTNET_OP_IP_PROG:
		NET_DBG("ip prog");
		artnet_handle_art_ip_prog(iface, pkt, ip_hdr);
		break;
	case ARTNET_OP_SYNC:
		NET_DBG("art sync");
		// TBD: simply send net_event or call callback to make node output
		// all the received data synchronously
		break;
	default:
		NET_DBG("Received unsupported pkt: [op=0x%x]", op_code);
		break;
	}

	net_pkt_unref(pkt);

	NET_DBG("Received artnet packet [op=0x%x]", hdr->op_code);
	return NET_OK;
}

/* Init function. It should be used (later) for node runtime configuration
 * by applicaion code, e.g. number of ports and its description, node
 * capabilities such as indicator diodes or squawking, any static info
 * to describe a speficic node
 */
int net_artnetv4_init(void)
{
	int ret;

	NET_DBG("");

	artnet_initialize_static_members();

	ret = net_udp_register(AF_INET, NULL, NULL,
			 ARTNET_PORT,
			 ARTNET_PORT,
			 NULL, net_artnetv4_input, NULL, NULL);
	if (ret < 0) {
		NET_DBG("UDP callback registration failed");
		return ret;
	}

	return 0;
}
