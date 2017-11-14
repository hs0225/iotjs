#include <tinyara/gpio.h>

#include "iotjs_def.h"
#include <iotbus_gpio.h>
#include <st_things.h>

#define TAG		"LIGHT_THINGS"

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

	return false;
}

static bool handle_set_request(st_things_set_request_message_s *req_msg, st_things_representation_s *resp_rep)
{
	printf("Received a SET request on %s\n", req_msg->resource_uri);

	return false;
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