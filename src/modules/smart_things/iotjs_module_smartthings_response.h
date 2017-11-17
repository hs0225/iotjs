#ifndef IOTJS_MODULE_SMART_THINGS_RESPONSE_H
#define IOTJS_MODULE_SMART_THINGS_RESPONSE_H

#include "iotjs_def.h"
#include "iotjs_objectwrap.h"
#include "iotjs_reqwrap.h"

#if defined(__TIZENRT__)
#include <st_things_types.h>
#include <st_things.h>
#endif

typedef struct {
  iotjs_jobjectwrap_t jobjectwrap;

#if defined(__TIZENRT__)
  st_things_representation_s* rep;
#else
  void* rep;
#endif
} IOTJS_VALIDATED_STRUCT(iotjs_sthings_rep_native_t);

typedef struct {
  iotjs_jobjectwrap_t jobjectwrap;
  iotjs_sthings_rep_native_t* jnative_cont;
} IOTJS_VALIDATED_STRUCT(iotjs_sthings_rep_t);

iotjs_sthings_rep_native_t* iotjs_stings_rep_native_create(iotjs_jval_t jobj, void* rep);

#endif
