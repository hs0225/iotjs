#ifndef IOTJS_MODULE_SMART_THINGS_H
#define IOTJS_MODULE_SMART_THINGS_H

#include "iotjs_def.h"
#include "iotjs_objectwrap.h"
#include "iotjs_reqwrap.h"
#include "iotjs_module_smartthings_response.h"

typedef struct {
//  iotjs_jobjectwrap_t jobjectwrap;
  iotjs_jval_t jstings;

//  uv_timer_t timer;
} iotjs_stings_t;

iotjs_stings_t* get_stings_data();
int init_smart_things();
void iotjs_smarttings_notify_observers(const char* res);

#endif
