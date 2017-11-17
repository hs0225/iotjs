#include <tinyara/gpio.h>
#include <iotbus_gpio.h>
#include <st_things.h>

#include "iotjs_def.h"
#include "modules/smart_things/iotjs_module_smartthings.h"

#define TAG		"SMART_THINGS"

#ifdef CONFIG_RESET_BUTTON
extern void iotapi_initialize(void);

static bool check_reset_button_pin_number(void)
{
	printf("Current pin number : %d\n", CONFIG_RESET_BUTTON_PIN_NUMBER);
	return CONFIG_RESET_BUTTON_PIN_NUMBER >= 0 ? true : false;
}

static void gpio_callback_event(void *user_data)
{
	printf("gpio_callback_event!!\n");
	printf("reset :: %d\n", st_things_reset());
}
#endif

static void handle_reset_result(bool result)
{
	printf("[%s]Reset %s.\n", result ? "succeeded" : "failed", TAG);
}

static void handle_things_status_change(st_things_status_e things_status)
{
	printf("[%s]Things status is changed: %d\n", TAG, things_status);
}

static bool handle_reset_request(void)
{
	printf("[%s]Received a request for RESET.\n", TAG);
	return true;
}

static bool handle_ownership_transfer_request(void)
{
	printf("[%s]Received a request for Ownership-transfer. \n", TAG);
	return true;
}


static bool handle_get_request(st_things_get_request_message_s *req_msg, st_things_representation_s *resp_rep)
{
	printf("Received a GET request on %s\n", req_msg->resource_uri);

	iotjs_stings_t* stings_data = get_stings_data();
	iotjs_jval_t jget_request = iotjs_jval_get_property(stings_data->jstings, "emitGetRequest");

	iotjs_jval_t jnative_cont = iotjs_jval_create_object();
	iotjs_stings_rep_native_create(jnative_cont, resp_rep);

	iotjs_jargs_t jargv = iotjs_jargs_create(4);
	iotjs_jargs_append_string_raw(&jargv, req_msg->resource_uri ? req_msg->resource_uri : "");
	iotjs_jargs_append_string_raw(&jargv, req_msg->property_key ? req_msg->property_key : "");
	iotjs_jargs_append_string_raw(&jargv, req_msg->query ? req_msg->query : "");
	iotjs_jargs_append_jval(&jargv, jnative_cont);

  iotjs_jhelper_call_ok(jget_request, stings_data->jstings, &jargv);

  iotjs_jargs_destroy(&jargv);
  jerry_release_value(jget_request);

	return true;
}

static bool handle_set_request(st_things_set_request_message_s *req_msg, st_things_representation_s *resp_rep)
{
	printf("Received a SET request on %s\n", req_msg->resource_uri);

  iotjs_stings_t* stings_data = get_stings_data();
  iotjs_jval_t jset_request = iotjs_jval_get_property(stings_data->jstings, "emitSetRequest");


  iotjs_jval_t jnative_cont1 = iotjs_jval_create_object();
  iotjs_stings_rep_native_create(jnative_cont1, req_msg->rep);

  iotjs_jval_t jnative_cont2 = iotjs_jval_create_object();
  iotjs_stings_rep_native_create(jnative_cont2, resp_rep);

  iotjs_jargs_t jargv = iotjs_jargs_create(4);

  iotjs_jargs_append_string_raw(&jargv, req_msg->resource_uri ? req_msg->resource_uri : "");
  iotjs_jargs_append_string_raw(&jargv, req_msg->query ? req_msg->query : "");
  iotjs_jargs_append_jval(&jargv, jnative_cont1);
  iotjs_jargs_append_jval(&jargv, jnative_cont2);

  iotjs_jhelper_call_ok(jset_request, stings_data->jstings, &jargv);
  iotjs_jargs_destroy(&jargv);
  jerry_release_value(jset_request);

	return true;
}

void iotjs_smarttings_notify_observers(const char* res) {
  st_things_notify_observers(res);
}


int init_smart_things() {
  #ifdef CONFIG_RESET_BUTTON
    if (!check_reset_button_pin_number()) {
      printf("Error : Invalid pin number.\n");
      return 0;
    }
    iotapi_initialize();
  
    iotbus_gpio_context_h m_gpio = iotbus_gpio_open(CONFIG_RESET_BUTTON_PIN_NUMBER);
    iotbus_gpio_register_cb(m_gpio, IOTBUS_GPIO_EDGE_RISING, gpio_callback_event, (void *)m_gpio);
  #endif
  
    bool easysetup_complete = false;
    st_things_initialize("/rom/sampleDevice.json", &easysetup_complete);
  
    st_things_register_request_cb(handle_get_request, handle_set_request);
    st_things_register_reset_cb(handle_reset_request, handle_reset_result);
    st_things_register_user_confirm_cb(handle_ownership_transfer_request);
    st_things_register_things_status_change_cb(handle_things_status_change);
  
    st_things_start();
  
    printf("[%s]=====================================================\n", TAG);
    printf("[%s]                    Stack Started                    \n", TAG);
    printf("[%s]=====================================================\n", TAG);
  
    return 0;
  }
