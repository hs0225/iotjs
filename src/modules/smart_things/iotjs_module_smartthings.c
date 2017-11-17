#include "iotjs_module_smartthings.h"
#include <time.h>

//IOTJS_DEFINE_NATIVE_HANDLE_INFO_THIS_MODULE(stings);

static iotjs_stings_t stings_data;

//static void iotjs_stings_destroy(iotjs_stings_t* stings) {
//
//  iotjs_jobjectwrap_destroy(&stings->jobjectwrap);
//}


iotjs_stings_t* get_stings_data() {
  return &stings_data;
}


int time_set(unsigned int second, unsigned int microsecond)
{
	struct timespec current_time;

	current_time.tv_sec = second;
	current_time.tv_nsec = (long int)microsecond * 1000;

	if (clock_settime(CLOCK_REALTIME, &current_time) != 0) {
		return 0;
	}

	return 1;
}

JS_FUNCTION(SmartThings) {
  DJS_CHECK_THIS(object);
  DJS_CHECK_ARGS(3, object, object, function);

  stings_data.jstings = jerry_acquire_value(JS_GET_ARG(0, object));
  
  // set system default time (2017/01/01 0:0:0)
	time_set(1495089694, 0);

  init_smart_things();


  const iotjs_jval_t jcallback = JS_GET_ARG(2, function);
  iotjs_make_callback(jcallback, jerry_create_undefined(), iotjs_jargs_get_empty());
  return jerry_create_undefined();
}


JS_FUNCTION(Close) {
  jerry_release_value(stings_data.jstings);


  return jerry_create_undefined();
}


JS_FUNCTION(Test) {
  iotjs_stings_t* stings_data = get_stings_data();
//
  iotjs_jval_t jget_request = iotjs_jval_get_property(stings_data->jstings, "emitGetRequest");
//
  iotjs_jargs_t jargv2 = iotjs_jargs_create(4);
  iotjs_jval_t jnative_cont = iotjs_jval_create_object();
  iotjs_stings_rep_native_create(jnative_cont, NULL);

  iotjs_jargs_append_string_raw(&jargv2, "/switch/main/0");
  iotjs_jargs_append_string_raw(&jargv2, "power;");
  iotjs_jargs_append_string_raw(&jargv2, "");
  iotjs_jargs_append_jval(&jargv2, jnative_cont);

  iotjs_jhelper_call_ok(jget_request, stings_data->jstings, &jargv2);


  iotjs_jargs_destroy(&jargv2);
  jerry_release_value(jget_request);
  return jerry_create_undefined();
}


JS_FUNCTION(NotifyObservers) {
  DJS_CHECK_THIS(object);
  DJS_CHECK_ARGS(1, string);

  iotjs_string_t res = JS_GET_ARG(0, string);

  iotjs_smarttings_notify_observers(iotjs_string_data(&res));

  return jerry_create_undefined();
}

iotjs_jval_t InitSmartThings() {
  iotjs_jval_t smart_things = jerry_create_external_function(SmartThings);
  
  iotjs_jval_t prototype = iotjs_jval_create_object();
  iotjs_jval_set_property_jval(smart_things, IOTJS_MAGIC_STRING_PROTOTYPE, prototype);
  iotjs_jval_set_method(prototype, "close", Close);
  iotjs_jval_set_method(prototype, "notifyObservers", NotifyObservers);
  iotjs_jval_set_method(prototype, "test", Test);

  jerry_release_value(prototype);

  return smart_things;
}
