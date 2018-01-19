#ifndef IOTJS_MODULE_SMART_THINGS_RESPONSE_H
#define IOTJS_MODULE_SMART_THINGS_RESPONSE_H

#include "iotjs_def.h"
#include "iotjs_reqwrap.h"

#include <ocpayload.h>
#include <st_things_types.h>
#include <st_things.h>
#include <st_things_representation.h>

#if defined(__TIZEN__)
#include <app_common.h>
#endif

typedef struct {
  jerry_value_t jobject;
  st_things_representation_s* rep;
} IOTJS_VALIDATED_STRUCT(iotjs_sthings_rep_t);

jerry_value_t iotjs_jstrings_rep_create();

#endif
