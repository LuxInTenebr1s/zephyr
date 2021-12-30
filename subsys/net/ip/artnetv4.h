#ifndef __INTERNAL_ARTNETV4_H
#define __INTERNAL_ARTNETV4_H

#define ARTNET_PORT 	0x1936

int net_artnetv4_init();

struct artnet_payload {
	uint8_t data[512];
} __packed;

struct artnet_hdr {
	uint8_t id[8];
	uint16_t op_code;
} __packed;

struct artnet_art_poll {
	uint8_t prot_ver[2];
	uint8_t talk_to_me;
	uint8_t priority;
} __packed;

enum artnet_op_code {
	ARTNET_OP_POLL 			= 0x2000,
	ARTNET_OP_POLL_REPLY 		= 0x2100,
	ARTNET_OP_DIAG_DATA 		= 0x2300,
	ARTNET_OP_COMMAND 		= 0x2400,
	ARTNET_OP_OUTPUT 		= 0x5000,
	ARTNET_OP_NZS 			= 0x5100,
	ARTNET_OP_SYNC 			= 0x5200,
	ARTNET_OP_ADDRESS 		= 0x6000,
	ARTNET_OP_INPUT 		= 0x7000,
	ARTNET_OP_TOD_REQUEST 		= 0x8000,
	ARTNET_OP_TOD_DATA 		= 0x8100,
	ARTNET_OP_TOD_CONTOL 		= 0x8200,
	ARTNET_OP_RDM 			= 0x8300,
	ARTNET_OP_RDM_SUB 		= 0x8400,
	ARTNET_OP_VIDEO_SETUP 		= 0xA010,
	ARTNET_OP_VIDEO_PALETTE 	= 0xA020,
	ARTNET_OP_VIDEO_DATA 		= 0xA040,
	ARTNET_OP_MAC_MASTER 		= 0xF000,
	ARTNET_OP_MAC_SLAVE 		= 0xF100,
	ARTNET_OP_FIRMWARE_MASTER 	= 0xF200,
	ARTNET_OP_FIRMWARE_REPLY 	= 0xF300,
	ARTNET_OP_IP_PROG 		= 0xF800,
	ARTNET_OP_IP_PROG_REPLY 	= 0xF900,
	ARTNET_OP_MEDIA 		= 0x9000,
	ARTNET_OP_MEDIA_PATCH 		= 0x9100,
	ARTNET_OP_MEDIA_CONTROL 	= 0x9200,
	ARTNET_OP_MEDIA_CONTROL_REPLY 	= 0x9300,
	ARTNET_OP_TIME_CODE 		= 0x9700,
	ARTNET_OP_TIME_SYNC 		= 0x9800,
	ARTNET_OP_TRIGGER 		= 0x9900,
	ARTNET_OP_DIRECTORY 		= 0x9A00,
	ARTNET_OP_DIRECTORY_REPLY 	= 0x9B00,
};

enum artnet_node_rc {
	ARTNET_RC_DEBUG 		= 0x0000,
	ARTNET_RC_POWER_OK 		= 0x0001,
	ARTNET_RC_POWER_FAIL 		= 0x0002,
	ARTNET_RC_SOCKET_WR1 		= 0x0003,
	ARTNET_RC_PARSE_FAIL 		= 0x0004,
	ARTNET_RC_UDP_FAIL 		= 0x0005,
	ARTNET_RC_SH_NAME_OK 		= 0x0006,
	ARTNET_RC_LO_NAME_OK 		= 0x0007,
	ARTNET_RC_DMX_ERROR 		= 0x0008,
	ARTNET_RC_DMX_UDP_FULL 		= 0x0009,
	ARTNET_RC_DMX_RX_FULL 		= 0x000A,
	ARTNET_RC_SWITCH_ERR 		= 0x000B,
	ARTNET_RC_CONFIG_ERR 		= 0x000C,
	ARTNET_RC_DMX_SHORT 		= 0x000D,
	ARTNET_RC_FIRMWARE_FAIL 	= 0x000E,
	ARTNET_RC_USER_FAIL 		= 0x000F,
	ARTNET_RC_FACTORY_RES 		= 0x0010,
};

enum artnet_style_code {
	ARTNET_ST_NODE 			= 0x00,
	ARTNET_ST_CONTROLLER 		= 0x01,
	ARTNET_ST_MEDIA 		= 0x02,
	ARTNET_ST_ROUTE 		= 0x03,
	ARTNET_ST_BACKUP 		= 0x04,
	ARTNET_ST_CONFIG 		= 0x05,
	ARTNET_ST_VISUAL 		= 0x06,
};

#define ARTNET_SHORT_NAME_LEN 		18
#define ARTNET_LONG_NAME_LEN 		64
#define ARTNET_NODE_REPORT_LEN 		64
#define ARTNET_PORT_NUM 		4

enum artnet_port_type {
	ARTNET_PORT_TYPE_MIDI 		= 0x01,
	ARTNET_PORT_TYPE_AVAB 		= 0x02,
	ARTNET_PORT_TYPE_CMX 		= 0x03,
	ARTNET_PORT_TYPE_ADB625 	= 0x04,
	ARTNET_PORT_TYPE_ART_NET 	= 0x05,
};

enum artnet_poll_good_input {
	ARTNET_GI_DATA_RCVD 		= BIT(7),
	ARTNET_GI_DMX_TEST_PKT 	= BIT(6),
	ARTNET_GI_DMX_SIP 		= BIT(5),
	ARTNET_GI_DMX_TEXT_PKT 	= BIT(4),
	ARTNET_GI_INPUT_DIS 		= BIT(3),
	ARTNET_GI_RCV_ERR 		= BIT(2),
};

enum artnet_poll_good_output_a {
	ARTNET_GO_A_DATA_TRSMT 		= BIT(7),
	ARTNET_GO_A_DMX_TEST_PKT 	= BIT(6),
	ARTNET_GO_A_DMX_SIP 		= BIT(5),
	ARTNET_GO_A_DMX_TEXT_PKT 	= BIT(4),
	ARTNET_GO_A_MERGE 		= BIT(3),
	ARTNET_GO_A_DMX_SHORT 		= BIT(2),
	ARTNET_GO_A_MERGE_LTP 		= BIT(1),
	ARTNET_GO_A_SACN 		= BIT(0),
};

enum artnet_poll_good_output_b {
	ARTNET_GO_B_RDM_ENABLE 		= BIT(7),
	ARTNET_GO_B_STYLE_CONT 		= BIT(6),
};

enum artnet_poll_status1 {
	ARTNET_STATUS1_UBEA 		= BIT(0),
	ARTNET_STATUS1_RDM 		= BIT(1),
	ARTNET_STATUS1_BOOT_ROM		= BIT(2),
	ARTNET_STATUS1_PORT_ADDR_PANEL 	= BIT(4),
	ARTNET_STATUS1_PORT_ADDR_WEB 	= BIT(5),
	ARTNET_STATUS1_IND_LOC_MODE 	= BIT(6),
	ARTNET_STATUS1_IND_MUTE_MODE 	= BIT(7),
	ARTNET_STATUS1_IND_NORMAN_MODE  = BIT(6) | BIT(7),
};

enum artnet_poll_status2 {
	ARTNET_STATUS2_WEB_BROWSER 	= BIT(0),
	ARTNET_STATUS2_DHCP 		= BIT(1),
	ARTNET_STATUS2_DHCP_CAP 	= BIT(2),
	ARTNET_STATUS2_8BIT_PORT 	= BIT(3),
	ARTNET_STATUS2_SACN_SWITCH 	= BIT(4),
	ARTNET_STATUS2_SQUAWKING 	= BIT(5),
	ARTNET_STATUS2_STYLE_SWITCH 	= BIT(6),
	ARTNET_STATUS2_RDM_CTRL 	= BIT(7),
};

enum artnet_poll_status3 {
	ARTNET_STATUS3_FAIL_OVER_STATE 	= BIT(7) | BIT(6),
	ARTNET_STATUS3_FAIL_OVER_SUPP   = BIT(5),
};

struct artnet_art_poll_reply {
	uint8_t ip_addr[4]; 				/* ok */
	uint16_t port; 					/* ok */
	uint8_t vers_info[2]; 				/* ok */
	uint8_t net_switch; 				/* app stat */
	uint8_t sub_switch; 				/* app stat */
	uint8_t oem[2]; 				/* ok ? */
	uint8_t ubea_vers; 				/* ok */
	uint8_t status1; 				/* app dyn */
	uint8_t esta_man[2]; 				/* app stat */
	uint8_t short_name[ARTNET_SHORT_NAME_LEN]; 	/* ok */
	uint8_t long_name[ARTNET_LONG_NAME_LEN]; 	/* ok */
	uint8_t node_report[ARTNET_NODE_REPORT_LEN]; 	/* app dyn */
	uint8_t num_ports[2]; 				/* app stat */
	uint8_t port_types[ARTNET_PORT_NUM]; 		/* app stat */
	uint8_t good_input[ARTNET_PORT_NUM]; 		/* app dyn */
	uint8_t good_output_a[ARTNET_PORT_NUM]; 	/* app dyn */
	uint8_t sw_in[ARTNET_PORT_NUM]; 		/* app stat */
	uint8_t sw_out[ARTNET_PORT_NUM]; 		/* app stat */
	uint8_t sw_video; 				/* ok */
	uint8_t sw_macro; 				/* ?? */
	uint8_t sw_remote; 				/* ?? */
	uint8_t spare[3];
	uint8_t style; 					/* ok */
	uint8_t mac_addr[6]; 				/* ok */
	uint8_t bind_ip[4]; 				/* ok */
	uint8_t bind_idx; 				/* ok */
	uint8_t status2; 				/* app stat */
	uint8_t good_output_b[ARTNET_PORT_NUM];
	uint8_t status3;
	uint8_t filler[21];
} __packed;

enum artnet_ip_prog_command {
	ARTNET_IP_COMMAND_ENABLE 	= BIT(7),
	ARTNET_IP_COMMAND_DHCP_ENABLE 	= BIT(6),
	ARTNET_IP_COMMAND_SET_DEFAULT 	= BIT(3),
	ARTNET_IP_COMMAND_SET_IP_ADDR 	= BIT(2),
	ARTNET_IP_COMMAND_SET_SUBNET 	= BIT(1),
	ARTNET_IP_COMMAND_SET_PORT 	= BIT(0),
};

struct artnet_art_ip_prog {
	uint8_t prot_ver[2];
	uint8_t filler1;
	uint8_t filler2;
	uint8_t command;
	uint8_t filler4;
	uint8_t prog_ip[4];
	uint8_t prog_sm[4];
	uint8_t prog_port[2];
	uint8_t spare[8];
} __packed;

struct artnet_art_ip_prog_reply {
	uint8_t prot_ver[2];
	uint8_t filler[4];
	uint8_t prog_ip[4];
	uint8_t prog_sm[4];
	uint8_t prog_port[2];
	uint8_t status;
	uint8_t spare[7];
} __packed;

enum artnet_address_command {
	ARTNET_AC_NONE 		= 0x00,
	ARTNET_AC_CANCEL_MERGE 	= 0x01,
	ARTNET_AC_LED_NORMAL 	= 0x02,
	ARTNET_AC_LED_MUTE 	= 0x03,
	ARTNET_AC_LED_LOCATE 	= 0x04,
	ARTNET_AC_RESET_RX 	= 0x05,
	ARTNET_AC_FAIL_HOLD 	= 0x08,
	ARTNET_AC_FAIL_ZERO 	= 0x09,
	ARTNET_AC_FAIL_FULL 	= 0x0A,
	ARTNET_AC_FAIL_SCENE 	= 0x0B,
	ARTNET_AC_FAIL_RECORD 	= 0x0C,
	ARTNET_AC_MERGE_LTP0 	= 0x10,
	ARTNET_AC_MERGE_LTP1 	= 0x11,
	ARTNET_AC_MERGE_LTP2 	= 0x12,
	ARTNET_AC_MERGE_LTP3 	= 0x13,
	ARTNET_AC_MERGE_HTP0 	= 0x50,
	ARTNET_AC_MERGE_HTP1 	= 0x51,
	ARTNET_AC_MERGE_HTP2 	= 0x52,
	ARTNET_AC_MERGE_HTP3 	= 0x53,
	ARTNET_AC_ART_NET_SEL0 	= 0x60,
	ARTNET_AC_ART_NET_SEL1 	= 0x61,
	ARTNET_AC_ART_NET_SEL2 	= 0x62,
	ARTNET_AC_ART_NET_SEL3 	= 0x63,
	ARTNET_AC_ACN_SEL0 	= 0x70,
	ARTNET_AC_ACN_SEL1 	= 0x71,
	ARTNET_AC_ACN_SEL2 	= 0x72,
	ARTNET_AC_ACN_SEL3 	= 0x73,
	ARTNET_AC_CLEAR_OP0 	= 0x90,
	ARTNET_AC_CLEAR_OP1 	= 0x91,
	ARTNET_AC_CLEAR_OP2 	= 0x92,
	ARTNET_AC_CLEAR_OP3 	= 0x93,
	ARTNET_AC_STYLE_DELTA0  = 0xA0,
	ARTNET_AC_STYLE_DELTA1  = 0xA1,
	ARTNET_AC_STYLE_DELTA2  = 0xA2,
	ARTNET_AC_STYLE_DELTA3  = 0xA3,
	ARTNET_AC_STYLE_CONST0 	= 0xB0,
	ARTNET_AC_STYLE_CONST1 	= 0xB1,
	ARTNET_AC_STYLE_CONST2 	= 0xB2,
	ARTNET_AC_STYLE_CONST3 	= 0xB3,
	ARTNET_AC_RDM_ENABLE0 	= 0xC0,
	ARTNET_AC_RDM_ENABLE1 	= 0xC1,
	ARTNET_AC_RDM_ENABLE2 	= 0xC2,
	ARTNET_AC_RDM_ENABLE3 	= 0xC3,
	ARTNET_AC_RDM_DISABLE0 	= 0xD0,
	ARTNET_AC_RDM_DISABLE1 	= 0xD1,
	ARTNET_AC_RDM_DISABLE2 	= 0xD2,
	ARTNET_AC_RDM_DISABLE3 	= 0xD3,
};

struct artnet_art_address {
	uint8_t id[8];
	uint16_t op_code;
	uint8_t prot_ver[2];
	uint8_t new_switch;
	uint8_t bind_idx;
	uint8_t short_name[ARTNET_SHORT_NAME_LEN];
	uint8_t long_name[ARTNET_LONG_NAME_LEN];
	uint8_t sw_in[ARTNET_PORT_NUM];
	uint8_t sw_out[ARTNET_PORT_NUM];
	uint8_t sw_switch;
	uint8_t sw_video;
	uint8_t command;
} __packed;

enum artnet_diag_priority {
	ARTNET_DP_LOW 		= 0x10,
	ARTNET_DP_MED 		= 0x40,
	ARTNET_DP_HIGH 		= 0x80,
	ARTNET_DP_CRITICAL 	= 0xE0,
	ARTNET_DP_VOLATILE 	= 0xF0,
};

struct artnet_art_diag_data {
	uint8_t id[8];
	uint16_t op_code;
	uint8_t prot_ver[2];
	uint8_t filler1;
	uint8_t priority;
	uint8_t filler2;
	uint8_t filler3;
	uint8_t length[2];
	uint8_t data[512];
} __packed;

enum artnet_time_code_type {
	ARTNET_TCT_FILM_24 	= 0x00,
	ARTNET_TCT_EBU_25 	= 0x01,
	ARTNET_TCT_DF_29_97 	= 0x02,
	ARTNET_TCT_SMPTE_30 	= 0x03,
};

struct artnet_art_time_code {
	uint8_t filler1;
	uint8_t filler2;
	uint8_t frames;
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t type;
} __packed;

struct artnet_art_command {
	uint8_t esta_man[2];
	uint8_t length[2];
	uint8_t data[512];
} __packed;

struct artnet_art_trigger {
	uint8_t filler1;
	uint8_t filler2;
	uint8_t oem_code[2];
	uint8_t key;
	uint8_t subkey;
	uint8_t data[512];
} __packed;

struct artnet_art_dmx {
	uint8_t prot_ver[2];
	uint8_t sequence;
	uint8_t physical;
	uint8_t sub_uni;
	uint8_t net;
	uint8_t length[2];
	uint8_t data[512];
} __packed;

struct artnet_art_sync {
	uint8_t aux1;
	uint8_t aux2;
} __packed;

struct artnet_art_nzs {
	uint8_t sequence;
	uint8_t start_code;
	uint8_t sub_uni;
	uint8_t net;
	uint8_t length[2];
	uint8_t data[512];
} __packed;

struct artnet_art_vlc {
	uint8_t sequence;
	uint8_t start_code;
	uint8_t sub_uni;
	uint8_t net;
	uint8_t length[2];
	uint8_t vlc[512];
} __packed;

struct artnet_art_input {
	uint8_t filler1;
	uint8_t bind_idx;
	uint8_t num_ports[2];
	uint8_t input[ARTNET_PORT_NUM];
} __packed;

struct artnet_art_firmware_master {
	uint8_t filler1;
	uint8_t filler2;
	uint8_t type;
	uint8_t block_id;
	uint8_t firmware_len[4];
	uint8_t spare[20];
	uint16_t data[512];
} __packed;

enum artnet_firmware_reply_type {
	ARTNET_FR_BLOCK_GOOD 	= 0x00,
	ARTNET_FR_ALL_GOOD 	= 0x01,
	ARTNET_FR_FAIL 		= 0xff,
};

struct artnet_art_firmware_reply {
	uint8_t filler1;
	uint8_t filler2;
	uint8_t type;
	uint8_t spare[21];
} __packed;

struct artnet_art_tod_request {
	uint8_t filler1;
	uint8_t filler2;
	uint8_t spare[7];
	uint8_t net;
	uint8_t command;
	uint8_t add_count;
	uint8_t address[32];
} __packed;

struct artnet_art_tod_data {
	uint8_t rdm_ver;
	uint8_t port;
	uint8_t spare[6];
	uint8_t bind_idx;
	uint8_t net;
	uint8_t command_resp;
	uint8_t address;
	uint8_t uid_total[2];
	uint8_t block_count;
	uint8_t uid_count;
} __packed;

enum artnet_tod_control_command {
	ARTNET_ATC_NONE 	= 0x00,
	ARTNET_ATC_FLUSH 	= 0x01,
};

struct artnet_art_tod_control {
	uint8_t filler1;
	uint8_t filler2;
	uint8_t spare[7];
	uint8_t net;
	uint8_t command;
	uint8_t address;
} __packed;

struct artnet_art_rdm {
	uint8_t rdm_ver;
	uint8_t filler2;
	uint8_t spare[7];
	uint8_t net;
	uint8_t command;
	uint8_t address;
} __packed;

struct artnet_art_rdm_sub {
	uint8_t rdm_ver;
	uint8_t filler2;
	uint8_t uid[6];
	uint8_t spare1;
	uint8_t command_class;
	uint16_t parameter_id;
	uint16_t sub_device;
	uint16_t sub_count;
	uint8_t spare2[4];
} __packed;

#endif /* __INTERNAL_ARTNETV4_H */
