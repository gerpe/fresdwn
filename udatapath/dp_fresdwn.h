#ifndef DP_FRESDWN_H
#define DP_FRESDWN_H 1

#include "datapath.h"
#include "oflib-exp/ofl-exp-fresdwn.h"

ofl_err
dp_fresdwn_handle_dummy(struct datapath *dp, struct ofl_exp_fresdwn_msg_dummy *msg,const struct sender *sender);

#endif