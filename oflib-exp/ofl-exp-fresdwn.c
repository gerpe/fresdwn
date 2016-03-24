#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include "openflow/openflow.h"
#include "openflow/fresdwn-ext.h"
#include "ofl-exp-fresdwn.h"
#include "../oflib/ofl-print.h"
#include "../oflib/ofl-log.h"

#define LOG_MODULE ofl_exp_fdwn
OFL_LOG_INIT(LOG_MODULE)

/*
 *
 *  FRESDWN
 *
 */

int
ofl_exp_fresdwn_msg_pack(struct ofl_msg_experimenter *msg, uint8_t **buf, size_t *buf_len) {
    struct fresdwn_header *fw;
    struct ofl_exp_fresdwn_msg_header *exp = (struct ofl_exp_fresdwn_msg_header *)msg;
    switch (exp->type) {
        case (FRESDWNT_DUMMY):{
            struct ofl_exp_fresdwn_msg_dummy *dummy = (struct ofl_exp_fresdwn_msg_dummy*) exp;
            struct fresdwn_dummy *ofp_dummy; 

            *buf_len = sizeof(struct fresdwn_dummy);
            *buf     = (uint8_t *)malloc(*buf_len);
            ofp_dummy = (struct fresdwn_dummy*)(*buf);

            /* Copy the array contents*/
            memcpy(ofp_dummy->dummy_content, dummy->dummy_content, FRESDWN_ARRAY_SIZE);
            break;
        }
        case (FRESDWNT_FEATURE_REQUEST):{
            return 0;
        } 
        case (FRESDWNT_FEATURE_REPLY):{
            return 0;
        } // IDENTIFICADOR 1
        default: {
            OFL_LOG_WARN(LOG_MODULE, "Trying to print unknown FRESDWN Experimenter message.");
            return -1;
        }
    }
    /* Pack experimenter header */
    fw = (struct fresdwn_header *)(*buf);
    fw->fdwnh.experimenter = htonl(exp->header.experimenter_id);
    fw->fdwnh.exp_type =  htonl(exp->type);
    return 0;
}

ofl_err
ofl_exp_fresdwn_msg_unpack(struct ofp_header *oh, size_t *len, struct ofl_msg_experimenter **msg) {
/*
    Criadas para tentar enviar a mensagem experimenter
    uint8_t **buf = NULL;
    size_t *buf_len = NULL;
*/
    struct fresdwn_header *exp;
    if (*len < sizeof(struct fresdwn_header)) {
        OFL_LOG_WARN(LOG_MODULE, "Received EXPERIMENTER message has invalid length (%zu).", *len);
        return ofl_error(OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);
    }
    exp = (struct fresdwn_header *)oh;
    switch (ntohl(exp->fdwnh.exp_type)) {
        case (FRESDWNT_DUMMY):{
            struct fresdwn_dummy *src;
            struct ofl_exp_fresdwn_msg_dummy *dst;

            if (*len < sizeof(struct fresdwn_dummy)) {
                OFL_LOG_WARN(LOG_MODULE, "Received DUMMY message has invalid length (%zu).", *len);
                return ofl_error(OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);
            }

            *len -= sizeof(struct fresdwn_dummy);
            src = (struct fresdwn_dummy *)exp;
            dst = (struct ofl_exp_fresdwn_msg_dummy *)malloc(sizeof(struct ofl_exp_fresdwn_msg_dummy));

            dst->header.header.experimenter_id = ntohl(exp->fdwnh.experimenter);
            dst->header.type = ntohl(exp->fdwnh.exp_type);
            memcpy(dst->dummy_content, src->dummy_content, FRESDWN_ARRAY_SIZE);

            (*msg) = (struct ofl_msg_experimenter *)dst;

            return 0;
        }
        case (FRESDWNT_FEATURE_REQUEST):{
            return 0;
        }
        case (FRESDWNT_FEATURE_REPLY): {
            return 0;
        }
        // IDENTIFICADOR 1
        //case (FRESDWNT_STATUS_REQUEST): // IDENTIFICADOR 1
        case (FRESDWNT_STATUS_REPLY):{
            return 0;
        } // IDENTIFICADOR 1
        default: {
            OFL_LOG_WARN(LOG_MODULE, "Trying to unpack unknown FRESDWN Experimenter message.");
            //return ofl_error(OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
            return ofl_error(OFPET_BAD_REQUEST, OFPBRC_BAD_EXP_TYPE);
        }
    }
    // (*msg).experimenter_id = exp
    free(msg);
    return 0;
}

int
ofl_exp_fresdwn_msg_free(struct ofl_msg_experimenter *msg) {
    if (msg->experimenter_id == FRESDWN_VENDOR_ID) {
        struct ofl_exp_fresdwn_msg_header *exp = (struct ofl_exp_fresdwn_msg_header *)msg;
        switch (exp->type) {
            case (FRESDWNT_DUMMY):{
                break;
            }
            default: {
                OFL_LOG_WARN(LOG_MODULE, "Trying to free unknown FRESDWN Experimenter message.");
            }
        }
    } else {
        OFL_LOG_WARN(LOG_MODULE, "Trying to free non-FRESDWN Experimenter message.");
    }
    free(msg);
    return 0;
}

char *
ofl_exp_fresdwn_msg_to_string(struct ofl_msg_experimenter *msg) {
    char *str;
    size_t str_size;
    FILE *stream = open_memstream(&str, &str_size);
    if (msg->experimenter_id == FRESDWN_VENDOR_ID) {
        struct ofl_exp_fresdwn_msg_header *exp = (struct ofl_exp_fresdwn_msg_header *)msg;
        switch (exp->type) {
            case FRESDWNT_DUMMY:{
                OFL_LOG_WARN(LOG_MODULE, "Printing a dummy message");
                fprintf(stream, "ofexp{type=\"%u\"}", exp->type); 
            }
            default: {
                OFL_LOG_WARN(LOG_MODULE, "Trying to print unknown FRESDWN Experimenter message.");
                fprintf(stream, "ofexp{type=\"%u\"}", exp->type);
            }
        }
    } else {
        OFL_LOG_WARN(LOG_MODULE, "Trying to print non-FRESDWN Experimenter message.");
        fprintf(stream, "exp{exp_id=\"%u\"}", msg->experimenter_id);
    }

    fclose(stream);
    return str;
}
