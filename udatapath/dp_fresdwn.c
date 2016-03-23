#include "dp_fresdwn.h"
#include "openflow/fresdwn-ext.h" 

ofl_err
dp_fresdwn_handle_dummy(struct datapath *dp, struct ofl_exp_fresdwn_msg_dummy *msg,const struct sender *sender){
    uint8_t *dummy_arr;       
    uint8_t test_block[FRESDWN_ARRAY_SIZE];
    struct ofl_exp_fresdwn_msg_dummy reply = {{.type = FRESDWNT_DUMMY},
             .dummy_content = {0}};
    /* Nesting craziness... consider a change to the name of the struct field name */
    reply.header.header.experimenter_id = FRESDWN_VENDOR_ID;
    reply.header.header.header.type = OFPT_EXPERIMENTER;  

    memset(test_block,0x0, FRESDWN_ARRAY_SIZE);
    /* Check received message array*/
    dummy_arr = msg->dummy_content;
    /* Dummy condition to send a reply */
    if (!memcmp(dummy_arr, test_block, FRESDWN_ARRAY_SIZE)){
        /* Reply. Set a diferent value for the dummy content*/
        memset(reply.dummy_content, 0xff, FRESDWN_ARRAY_SIZE);
        dp_send_message(dp, (struct ofl_msg_header *)&reply, sender);
    }
    /* Send error. You can define your own error codes, since this one do not make sense and is just an illustrative example*/
    return ofl_error(OFPET_QUEUE_OP_FAILED, OFPQOFC_BAD_PORT);
}