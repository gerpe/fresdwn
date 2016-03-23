#ifndef OFL_EXP_FRESDWN_H
#define OFL_EXP_FRESDWN_H 1
/*
 *
 *  FRESDWN
 *
 */

#include "../oflib/ofl-structs.h"
#include "../oflib/ofl-messages.h"

struct ofl_exp_fresdwn_msg_header {
    struct ofl_msg_experimenter   header; /* FRESDWN_VENDOR_ID */
    uint32_t   type;
};

struct ofl_exp_fresdwn_msg_dummy {
    struct ofl_exp_fresdwn_msg_header   header; /* FRESDWN_VENDOR_ID */
    uint8_t dummy_content[1400];
};

struct ofl_exp_fresdwn_msg_role {
    struct ofl_exp_fresdwn_msg_header   header; /* FRESDWNT_ROLE_REQUEST|REPLY */
    uint32_t role;
};

int
ofl_exp_fresdwn_msg_pack(struct ofl_msg_experimenter *msg, uint8_t **buf, size_t *buf_len);

ofl_err
ofl_exp_fresdwn_msg_unpack(struct ofp_header *oh, size_t *len, struct ofl_msg_experimenter **msg);

int
ofl_exp_fresdwn_msg_free(struct ofl_msg_experimenter *msg);

char *
ofl_exp_fresdwn_msg_to_string(struct ofl_msg_experimenter *msg);

#endif 