/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.4.0-dev */

#include "igvc.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

const float RequestMessage_speed_l_default = 0;
const float RequestMessage_speed_r_default = 0;

const pb_field_t ResponseMessage_fields[12] = {
  PB_FIELD(1, FLOAT, OPTIONAL, STATIC, FIRST, ResponseMessage, p_l, p_l, 0),
  PB_FIELD(2, FLOAT, OPTIONAL, STATIC, OTHER, ResponseMessage, p_r, p_l, 0),
  PB_FIELD(3, FLOAT, OPTIONAL, STATIC, OTHER, ResponseMessage, i_l, p_r, 0),
  PB_FIELD(4, FLOAT, OPTIONAL, STATIC, OTHER, ResponseMessage, i_r, i_l, 0),
  PB_FIELD(5, FLOAT, OPTIONAL, STATIC, OTHER, ResponseMessage, d_l, i_r, 0),
  PB_FIELD(6, FLOAT, OPTIONAL, STATIC, OTHER, ResponseMessage, d_r, d_l, 0),
  PB_FIELD(7, FLOAT, OPTIONAL, STATIC, OTHER, ResponseMessage, speed_l, d_r, 0),
  PB_FIELD(8, FLOAT, OPTIONAL, STATIC, OTHER, ResponseMessage, speed_r, speed_l, 0),
  PB_FIELD(9, FLOAT, OPTIONAL, STATIC, OTHER, ResponseMessage, dt_sec, speed_r, 0),
  PB_FIELD(10, FLOAT, OPTIONAL, STATIC, OTHER, ResponseMessage, voltage, dt_sec, 0),
  PB_FIELD(11, BOOL, OPTIONAL, STATIC, OTHER, ResponseMessage, estop, voltage, 0),
  PB_LAST_FIELD
};

const pb_field_t RequestMessage_fields[9] = {
  PB_FIELD(1, FLOAT, OPTIONAL, STATIC, FIRST, RequestMessage, p_l, p_l, 0),
  PB_FIELD(2, FLOAT, OPTIONAL, STATIC, OTHER, RequestMessage, p_r, p_l, 0),
  PB_FIELD(3, FLOAT, OPTIONAL, STATIC, OTHER, RequestMessage, i_l, p_r, 0),
  PB_FIELD(4, FLOAT, OPTIONAL, STATIC, OTHER, RequestMessage, i_r, i_l, 0),
  PB_FIELD(5, FLOAT, OPTIONAL, STATIC, OTHER, RequestMessage, d_l, i_r, 0),
  PB_FIELD(6, FLOAT, OPTIONAL, STATIC, OTHER, RequestMessage, d_r, d_l, 0),
  PB_FIELD(7, FLOAT, OPTIONAL, STATIC, OTHER, RequestMessage, speed_l, d_r, &RequestMessage_speed_l_default),
  PB_FIELD(8, FLOAT, OPTIONAL, STATIC, OTHER, RequestMessage, speed_r, speed_l, &RequestMessage_speed_r_default),
  PB_LAST_FIELD
};

/* @@protoc_insertion_point(eof) */
