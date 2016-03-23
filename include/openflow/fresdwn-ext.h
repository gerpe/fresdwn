#include "openflow/openflow.h"

#define FRESDWN_OUI_STR "0x00000005"
#define FRESDWN_VENDOR_ID 0x00000005
#define FRESDWN_ARRAY_SIZE 1392
/*
 *
 *      FRESDWN
 *
 */

struct fresdwn_header {
    struct ofp_experimenter_header fdwnh;  
};
OFP_ASSERT(sizeof(struct fresdwn_header) == 16);

/* Freswdn messages types*/
enum fresdwn_type {
    /* Switch status request.  The request body is an ASCII string that
     * specifies a prefix of the key names to include in the output; if it is
     * the null string, then all key-value pairs are included. */
    // FRESDWNT_STATUS_REQUEST,
    FRESDWNT_FEATURE_REQUEST, // Solicita as caracteristicas do dispositivo

    /* Switch status reply.  The reply body is an ASCII string of key-value
     * pairs in the form "key=value\n". */
    FRESDWNT_FEATURE_REPLY, // Informa as caracteristicas para a controladora
    FRESDWNT_STATUS_REQUEST,

    /* Switch status reply.  The reply body is an ASCII string of key-value
     * pairs in the form "key=value\n". */
    FRESDWNT_STATUS_REPLY,

    /* Generic GET/SET Wireless configuration */
    FRESDWNT_ACT_SET_CONFIG,
    FRESDWNT_ACT_GET_CONFIG,
    FRESDWNT_COMMAND_REQUEST,
    FRESDWNT_COMMAND_REPLY,
    FRESDWNT_FLOW_END_CONFIG,
    FRESDWNT_FLOW_END,
    FRESDWNT_MGMT,

    /* GET/SET Channel configuration */
    FRESDWNT_ACT_SET_CHANNEL_CONFIG,
    FRESDWNT_ACT_GET_CHANNEL_CONFIG,

    /* GET PHYSICAL information */
    FRESDWNT_ACT_GET_AP_CONFIG,
    FRESDWNT_ACT_GET_AP_IN_RANGE_INFO,
    FRESDWNT_ACT_GET_BEACON_INFO,
    FRESDWNT_ACT_GET_NOISE_INFO,
    FRESDWNT_ACT_GET_POSITION_INFO,
    
    /* GET/SET SSID configuration */
    FRESDWNT_ACT_SET_SSID_CONFIG,
    FRESDWNT_ACT_GET_SSID_CONFIG,

    /* DUMMY Message for tests*/
    FRESDWNT_DUMMY
};

struct fresdwn_dummy {
    struct fresdwn_header header;
    uint8_t dummy_content[FRESDWN_ARRAY_SIZE];
};
OFP_ASSERT(sizeof(struct fresdwn_dummy) == 1408);

/* Wireless technology types*/
enum fresdwn_type_wireless_tecnology {
    FRESDWNT_802_11_a,
    FRESDWNT_802_11_b,
    FRESDWNT_802_11_g,
    FRESDWNT_802_11_n,
    FRESDWNT_802_11_ac,
    FRESDWNT_3G,
    FRESDWNT_4G,
    FRESDWNT_5G
};

enum fresdwn_type_of_wireless_atribute {
// 802.11
    FRESDWNT_FREQUENCY, // 2,4gH
    FRESDWNT_CHANNEL_SIZE, // 20mhz
    FRESDWNT_CHANNEL_NUMBER, // 6
    FRESDWNT_TRANSMITION_TYPE, // FHSS
    FRESDWNT_TRANSMITION_MIN_POWER, // 3mW
    FRESDWNT_TRANSMITION_POWER, // 30mW
    FRESDWNT_TRANSMITION_MAX_POWER, // 30mW
    FRESDWNT_SSID, // CONVIDADO - 32chars
    FRESDWNT_BSSID, // 8f:....
    FRESDWNT_BSSID_TYPE, // 1-infrastructure, 2-independent 3-any
    FRESDWNT_CRYPTO_TECNOLOGY, // WPA
    FRESDWNT_CRYPTO_TYPE_OF_KEY, // TPIK
    FRESDWNT_CRYPTO_SIZE_OF_KEY, // 256bits
    FRESDWNT_ANTENNA_POWER, // 2dbi
    FRESDWNT_ANTENNA_TYPE, // omnidirecional
    FRESDWNT_ANTENNA_QUANTITIES, // omnidirecional
    FRESDWNT_DATA_RATE_RX, // 11M
    FRESDWNT_DATA_RATE_SUPPORTED_RX, // 1M,11M,54M   126chars
    FRESDWNT_DATA_RATE_TX, // 11M
    FRESDWNT_DATA_RATE_SUPPORTED_TX, // 1M,11M,54M   126chars 
    FRESDWNT_MAC_ADDRESS, 
    FRESDWNT_MEDIUM_OCCUPANCY_LIMIT, // 0..1000
    FRESDWNT_CONTENTION_FREE_POLLABLE, // TRUE 
    FRESDWNT_CONTENTION_FREE_PERIOD, // 0..255
    FRESDWNT_CONTENTION_FREE_MAX_DURATION, //0..65536
    FRESDWNT_AUTHENTICATION_RESPONSE_TIMEOUT, // 1..4294967295
    FRESDWNT_PRIVACY_OPTION_IMPLEMENTED, // FALSE (WEP DESABILITADO)
    FRESDWNT_POWER_MANAGEMENT_MODE, // 1-active  2-powersave
    FRESDWNT_BEACON_PERIOD, // 1..65535
    FRESDWNT_DTIM_PERIOD, // 1..255
    FRESDWNT_ASSOCIATION_RESPONSE_TIMEOUT, //1..4294967295
    FRESDWNT_DISASSOCIATE_REASON, // 1..65536
    FRESDWNT_DISASSOCIATE_STATION, // MAC 
    FRESDWNT_DEAUTHENTICATE_REASON, // 1..65536
    FRESDWNT_DEAUTHENTICATE_STATION, // MAC 
    FRESDWNT_AUTHENTICATE_FAIL_STATUS, // 1..65535
    FRESDWNT_AUTHENTICATE_FAIL_STATION, // MAC 
    FRESDWNT_AUTHENTICATION_ALGORITH, // 1-OPEN SYSTEM    2-SHARED KEY
// CELULAR
};
enum fresdwn_type_of_frequency_atribute {
// 802.11
    FRESDWNT_2_4Ghz, 
    FRESDWNT_5Ghz, 
// CELULAR
    FRESDWNT_850Mhz, 
    FRESDWNT_900Mhz,
    FRESDWNT_1700Mhz,
    FRESDWNT_1800Mhz,
    FRESDWNT_1900Mhz, 
    FRESDWNT_2100Mhz,
};

enum fresdwn_action_subtype {
    FRESDWN_ACTION_SEND,           
    FRESDWN_ACTION_RECEIVE,       
    FRESDWN_ACTION_PROCESSING

};

/* Action structure for FRESDWN_ACTION_SEND. */
struct fresdwn_action_send {
    uint16_t type;                  /* OFPAT_VENDOR. */
    uint16_t len;                   /* Length is 16. */
    uint32_t vendor;                /* FRESDWN_VENDOR_ID. */
    uint16_t subtype;               /* FRESDWN_ACTION_SEND. */
    uint16_t in_port;               /* New in_port for checking flow table. */
    uint8_t pad[4];
};
OFP_ASSERT(sizeof(struct fresdwn_action_send) == 16);

/* Action structure for FRESDWN_ACTION_RECEIVE. */
struct fresdwn_action_receive {
    uint16_t type;                  /* OFPAT_VENDOR. */
    uint16_t len;                   /* Length is 16. */
    uint32_t vendor;                /* FRESDWN_VENDOR_ID. */
    uint16_t subtype;               /* FRESDWN_ACTION_RECEIVE. */
    uint16_t in_port;               /* New in_port for checking flow table. */
    uint8_t pad[4];
};
OFP_ASSERT(sizeof(struct fresdwn_action_receive) == 16);

/* Header for FRESDWN-defined actions. */
struct fresdwn_action_header {
    uint16_t type;                  /* OFPAT_VENDOR. */
    uint16_t len;                   /* Length is 16. */
    uint32_t vendor;                /* FRESDWN_VENDOR_ID. */
    uint16_t subtype;               /* FRESDWN_ACTION_*. */
    uint8_t pad[6];
};
OFP_ASSERT(sizeof(struct fresdwn_action_header) == 16);
