#include "iotjs_module_smartthings.h"
#include "iotjs_util.h"

static iotjs_stings_t stings_data = {.isInit = false};


iotjs_stings_t* get_stings_data() {
  return &stings_data;
}

//void handle_reset_result(bool result) {
//  printf("Reset %s.\n", result ? "succeeded" : "failed");
//}
//
//
//void handle_things_status_change(st_things_status_e things_status) {
//  printf("Things status is changed: %d\n", things_status);
//}
//
//
//bool handle_reset_request(void) {
//  printf("Received a request for RESET.\n");
//  return true;
//}
//
//
//bool handle_ownership_transfer_request(void) {
//  printf("Received a request for Ownership-transfer. \n");
//  return true;
//}


//static void wait_signal() {
//
//}

static void st_worker(uv_work_t* work_req) {
  DDDLOG("[ST] %s, SmartThings Worker", __func__);
  iotjs_stings_reqwrap_t* req_wrap = (iotjs_stings_reqwrap_t*)work_req->data;

  STOp op = req_wrap->op;

  if (op == kSTOpGet) {
    iotjs_stings_get_data* get_data = (iotjs_stings_get_data*)req_wrap->data;
    DDDLOG("[ST] GET %s, %s", __func__, get_data->req_msg->resource_uri);

    iotjs_stings_t* stings_data = get_stings_data();
    jerry_value_t jget_request = iotjs_jval_get_property(stings_data->jstings, "emitGetRequest");

    iotjs_jargs_t jargv = iotjs_jargs_create(4);
    iotjs_jargs_append_string_raw(&jargv,
        get_data->req_msg->resource_uri ? get_data->req_msg->resource_uri : "");
    iotjs_jargs_append_string_raw(&jargv,
        get_data->req_msg->property_key ? get_data->req_msg->property_key : "");
    iotjs_jargs_append_string_raw(&jargv,
        get_data->req_msg->query ? get_data->req_msg->query : "");
    iotjs_jargs_append_jval(&jargv, iotjs_jstrings_rep_create(get_data->resp_rep));


    iotjs_jhelper_call(jget_request, stings_data->jstings, &jargv);

    iotjs_jargs_destroy(&jargv);
    jerry_release_value(jget_request);
  } else if (op == kSTOpSet) {
    iotjs_stings_set_data* set_data = (iotjs_stings_set_data*)req_wrap->data;
    DDDLOG("[ST] SET %s, %s", __func__, set_data->req_msg->resource_uri);

    iotjs_stings_t* stings_data = get_stings_data();
    jerry_value_t jset_request = iotjs_jval_get_property(stings_data->jstings, "emitSetRequest");

    iotjs_jargs_t jargv = iotjs_jargs_create(4);
    iotjs_jargs_append_string_raw(&jargv,
        set_data->req_msg->resource_uri ? set_data->req_msg->resource_uri : "");
    iotjs_jargs_append_string_raw(&jargv,
        set_data->req_msg->query ? set_data->req_msg->query : "");
    iotjs_jargs_append_jval(&jargv, iotjs_jstrings_rep_create(set_data->req_msg->rep));
    iotjs_jargs_append_jval(&jargv, iotjs_jstrings_rep_create(set_data->resp_rep));

    iotjs_jhelper_call(jset_request, stings_data->jstings, &jargv);
    iotjs_jargs_destroy(&jargv);
    jerry_release_value(jset_request);

  } else {
    DLOG("[ST] %s, cannot find operation", __func__);
  }
  DDDLOG("[ST] %s, SmartThings Worker end~~~~~~", __func__);
}

static void st_after_worker(uv_work_t* work_req, int status) {
  iotjs_stings_reqwrap_t* req_wrap = (iotjs_stings_reqwrap_t*)work_req->data;
  DDDLOG("[ST] %s, SmartThings After Worker", __func__);
  STOp op = req_wrap->op;
  if (op == kSTOpGet) {
    iotjs_stings_get_data* get_data = (iotjs_stings_get_data*)req_wrap->data;

    IOTJS_RELEASE(get_data);
  } else if (op == kSTOpSet) {
    iotjs_stings_set_data* set_data = (iotjs_stings_set_data*)req_wrap->data;

    IOTJS_RELEASE(set_data);
  } else {
    DLOG("[ST] %s, cannot find operation", __func__);
  }

  IOTJS_RELEASE(req_wrap);

  pthread_mutex_lock(&stings_data.mutex);
  pthread_cond_signal(&stings_data.cond);
  pthread_mutex_unlock(&stings_data.mutex);
  DDDLOG("[ST] %s, SmartThings After Worker end~~~~~~~", __func__);

}


//static iotjs_string_t create_iotjs_string_t(const char* str) {
//  if (str == NULL) {
//    return iotjs_string_create();
//  }
//  return iotjs_string_create_with_size(str, strlen(str));
//}

bool iotjs_handle_st_get_request(st_things_get_request_message_s *req_msg,
    st_things_representation_s *resp_rep) {
  DDDLOG("[ST] Received a GET request on %s\n", req_msg->resource_uri);

  if (!stings_data.isInit) {
    DLOG("[IOTJS][ST] not initialize");
    return false;
  }

  uv_loop_t* loop = iotjs_environment_loop(iotjs_environment_get());
  iotjs_stings_reqwrap_t* req_wrap = IOTJS_ALLOC(iotjs_stings_reqwrap_t);
  req_wrap->op = kSTOpGet;
  iotjs_stings_get_data* get_data = IOTJS_ALLOC(iotjs_stings_get_data);
  req_wrap->data = get_data;

  DDDLOG("[ST] %s - %s, %s, %s", __func__, req_msg->resource_uri, req_msg->query, req_msg->property_key);

//  get_data->uri = create_iotjs_string_t(req_msg->resource_uri);
//  get_data->key = create_iotjs_string_t(req_msg->property_key);
//  get_data->query = create_iotjs_string_t(req_msg->query);
  get_data->req_msg = req_msg;
  get_data->resp_rep = resp_rep;


  req_wrap->request = (uv_req_t*)&req_wrap->req;
  req_wrap->request->data = req_wrap;
  uv_queue_work(loop, &req_wrap->req, st_worker, st_after_worker);

  pthread_mutex_lock(&stings_data.mutex);
  pthread_cond_wait(&stings_data.cond, &stings_data.mutex);
  pthread_mutex_unlock(&stings_data.mutex);
  DDDLOG("[ST] Received a GET request on end %s\n", req_msg->resource_uri);
  return true;
}


bool iotjs_handle_st_set_request(st_things_set_request_message_s *req_msg,
    st_things_representation_s *resp_rep) {
  DDDLOG("[ST] Received a SET request on %s\n", req_msg->resource_uri);

  if (!stings_data.isInit) {
    DLOG("[IOTJS][ST] not initialize");
    return false;
  }

  uv_loop_t* loop = iotjs_environment_loop(iotjs_environment_get());
  iotjs_stings_reqwrap_t* req_wrap = IOTJS_ALLOC(iotjs_stings_reqwrap_t);
  req_wrap->op = kSTOpSet;
  iotjs_stings_set_data* set_data = IOTJS_ALLOC(iotjs_stings_set_data);
  req_wrap->data = set_data;

  DDDLOG("[ST] %s - %s, %s", __func__, req_msg->resource_uri, req_msg->query);

//  set_data->uri = create_iotjs_string_t(req_msg->resource_uri);
//  set_data->query = create_iotjs_string_t(req_msg->query);
  set_data->req_msg = req_msg;
  set_data->resp_rep = resp_rep;

  req_wrap->request = (uv_req_t*)&req_wrap->req;
  req_wrap->request->data = req_wrap;
  uv_queue_work(loop, &req_wrap->req, st_worker, st_after_worker);

  pthread_mutex_lock(&stings_data.mutex);
  pthread_cond_wait(&stings_data.cond, &stings_data.mutex);
  pthread_mutex_unlock(&stings_data.mutex);
  DDDLOG("[ST] Received a SET request on end %s\n", req_msg->resource_uri);
  return true;
}

JS_FUNCTION(SmartThings) {
  DJS_CHECK_THIS();
  DJS_CHECK_ARGS(3, object, object, function);

  stings_data.jstings = jerry_acquire_value(JS_GET_ARG(0, object));
  stings_data.isInit = true;

  const jerry_value_t jcallback = JS_GET_ARG(2, function);
  iotjs_make_callback(jcallback, jerry_create_undefined(), iotjs_jargs_get_empty());

  stings_data.isInit = true;

  pthread_cond_init(&stings_data.cond, NULL);
  pthread_mutex_init(&stings_data.mutex, NULL);

  return jerry_create_undefined();
}


JS_FUNCTION(Close) {
  jerry_release_value(stings_data.jstings);
  DDDLOG("[IOTJS][ST] Close Smart Things\n");
  stings_data.isInit = false;
  return jerry_create_undefined();
}


JS_FUNCTION(NotifyObservers) {
  DJS_CHECK_THIS();
  DJS_CHECK_ARGS(1, string);

  iotjs_string_t res = JS_GET_ARG(0, string);

  st_things_notify_observers(iotjs_string_data(&res));

  return jerry_create_undefined();
}


jerry_value_t InitSmartThings() {
  jerry_value_t smart_things = jerry_create_external_function(SmartThings);

  jerry_value_t prototype = jerry_create_object();
  iotjs_jval_set_property_jval(smart_things, IOTJS_MAGIC_STRING_PROTOTYPE, prototype);
  iotjs_jval_set_method(prototype, "close", Close);
  iotjs_jval_set_method(prototype, "notifyObservers", NotifyObservers);

  jerry_release_value(prototype);

  return smart_things;
}
