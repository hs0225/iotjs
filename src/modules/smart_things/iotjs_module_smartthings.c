#include "iotjs_module_smartthings.h"
#include <time.h>

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
  DJS_CHECK_ARGS(1, object);
  printf("start SmartThings native\n");

  // set system default time (2017/01/01 0:0:0)
	time_set(1495089694, 0);
  init_smart_things();

  return jerry_create_undefined();
}


iotjs_jval_t InitSmartThings() {
  iotjs_jval_t smart_things = jerry_create_external_function(SmartThings);
  
  iotjs_jval_t prototype = iotjs_jval_create_object();
  iotjs_jval_set_property_jval(smart_things, IOTJS_MAGIC_STRING_PROTOTYPE, prototype);

  // iotjs_jval_set_method(prototype, IOTJS_MAGIC_STRING_COMPARE, Compare);

  jerry_release_value(prototype);

  return smart_things;
}