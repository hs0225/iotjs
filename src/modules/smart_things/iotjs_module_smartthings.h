#ifndef IOTJS_MODULE_SMART_THINGS_H
#define IOTJS_MODULE_SMART_THINGS_H

#include <pthread.h>
#include "iotjs_def.h"
#include "iotjs_module_st_representation.h"
#include "iotjs_reqwrap.h"

typedef enum {
  kSTOpGet,
  kSTOpSet
} STOp;

typedef struct {
  bool isInit;
  jerry_value_t jstings;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
} iotjs_stings_t;

typedef struct {
  st_things_get_request_message_s *req_msg;
  st_things_representation_s* resp_rep;
} iotjs_stings_get_data;


typedef struct {
  st_things_set_request_message_s *req_msg;
  st_things_representation_s *resp_rep;
} iotjs_stings_set_data;

typedef struct {
  uv_req_t* request;
  uv_work_t req;
  STOp op;
  bool result;
  void* data;
} iotjs_stings_reqwrap_t;

iotjs_stings_t* get_stings_data();

bool handle_set_request(st_things_set_request_message_s *req_msg,
    st_things_representation_s *resp_rep);

bool handle_get_request(st_things_get_request_message_s *req_msg,
    st_things_representation_s *resp_rep);

void handle_reset_result(bool result);

void handle_things_status_change(st_things_status_e things_status);


bool handle_reset_request(void);


bool handle_ownership_transfer_request(void);
#endif
