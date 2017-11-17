#include "iotjs_def.h"
#include "iotjs_module_smartthings_response.h"


IOTJS_DEFINE_NATIVE_HANDLE_INFO_THIS_MODULE(sthings_rep);
void iotjs_sthings_rep_native_destroy(iotjs_sthings_rep_native_t* obj);


static iotjs_sthings_rep_t* iotjs_stings_rep_create(iotjs_jval_t jobj, iotjs_sthings_rep_native_t* native_cont) {
  iotjs_sthings_rep_t* obj = IOTJS_ALLOC(iotjs_sthings_rep_t);
  IOTJS_VALIDATED_STRUCT_CONSTRUCTOR(iotjs_sthings_rep_t, obj);

  _this->jnative_cont = native_cont;
  iotjs_jobjectwrap_initialize(&_this->jobjectwrap, jobj,
                               &this_module_native_info);

  return obj;
}


static void iotjs_sthings_rep_destroy(iotjs_sthings_rep_t* obj) {
  IOTJS_VALIDATED_STRUCT_DESTRUCTOR(iotjs_sthings_rep_t, obj);
  iotjs_jobjectwrap_destroy(&_this->jobjectwrap);

  IOTJS_RELEASE(obj);
}


iotjs_sthings_rep_native_t* iotjs_stings_rep_native_create(iotjs_jval_t jobj, void* rep) {
  iotjs_sthings_rep_native_t* obj = IOTJS_ALLOC(iotjs_sthings_rep_native_t);
  IOTJS_VALIDATED_STRUCT_CONSTRUCTOR(iotjs_sthings_rep_native_t, obj);

  static const jerry_object_native_info_t native_info = {
      .free_cb = (jerry_object_native_free_callback_t)iotjs_sthings_rep_native_destroy
  };

  iotjs_jobjectwrap_initialize(&_this->jobjectwrap, jobj, &native_info);

  _this->rep = rep;

  return obj;
}


void iotjs_sthings_rep_native_destroy(iotjs_sthings_rep_native_t* obj) {
  IOTJS_VALIDATED_STRUCT_DESTRUCTOR(iotjs_sthings_rep_native_t, obj);
  iotjs_jobjectwrap_destroy(&_this->jobjectwrap);

  IOTJS_RELEASE(obj);
}

static void* get_native_rep_handler(iotjs_sthings_rep_native_t* st) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_native_t, st);
  return _this->rep;
}

JS_FUNCTION(SmartThingsResponse) {
  DJS_CHECK_THIS(object);
  DJS_CHECK_ARGS(1, object);

  iotjs_jval_t jobject = JS_GET_ARG(0, object);
  iotjs_sthings_rep_native_t* srep = (iotjs_sthings_rep_native_t*)iotjs_jval_get_object_native_handle(jobject);

  iotjs_stings_rep_create(JS_GET_THIS(object), srep);

  return jerry_create_undefined();
}

static void req_set_data(const char* key, iotjs_jval_t jdata, void *resp_rep) {

#if defined(__TIZENRT__)
  st_things_representation_s* native_rep = resp_rep;
#endif

  // Get value
  if (jerry_value_is_string(jdata)) {
    iotjs_string_t jvalue_raw = iotjs_jval_as_string(jdata);
    const char* jvalue_string_raw = iotjs_string_data(&jvalue_raw);
#if defined(__TIZENRT__)
    native_rep->set_str_value(resp_rep, key, jvalue_string_raw);
#endif
    iotjs_string_destroy(&jvalue_raw);

  } else if(jerry_value_is_object(jdata)) {
#if defined(__TIZENRT__)
    st_things_representation_s *temp_rep = st_things_create_representation_inst();
#else
    void* temp_rep = NULL;
#endif
    iotjs_jval_t jdata_key = jerry_get_object_keys(jdata);
    iotjs_jval_t jdata_key_length = iotjs_jval_get_property(jdata_key, "length");
    size_t length = iotjs_jval_as_number(jdata_key_length);

    for (size_t i = 0; i < length; i++) {
      iotjs_jval_t jtemp_index = iotjs_jval_get_property_by_index(jdata_key, i);
      iotjs_string_t jtemp_key_string = iotjs_jval_as_string(jtemp_index);
      const char* jtemp_key_stirng_raw = iotjs_string_data(&jtemp_key_string);
      iotjs_jval_t jtemp_value = iotjs_jval_get_property(jdata, jtemp_key_stirng_raw);

      req_set_data(jtemp_key_stirng_raw, jtemp_value, temp_rep);

      jerry_release_value(jtemp_index);
      iotjs_string_destroy(&jtemp_key_string);
      jerry_release_value(jtemp_value);
    }

    jerry_release_value(jdata_key_length);
    jerry_release_value(jdata_key);

#if defined(__TIZENRT__)
    native_rep->set_object_value(resp_rep, key, temp_rep);

    st_things_destroy_representation_inst(temp_rep);
#endif
  }
}

JS_FUNCTION(Set) {
  DJS_CHECK_THIS(object);

  JS_DECLARE_THIS_PTR(sthings_rep, st);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, st);

  void *resp_rep = get_native_rep_handler(_this->jnative_cont);

  iotjs_string_t jkey_string = JS_GET_ARG(0, string);
  const char* jkey_stirng_raw = iotjs_string_data(&jkey_string);
  req_set_data(jkey_stirng_raw, jargv[1], resp_rep);

  iotjs_string_destroy(&jkey_string);

  return jerry_create_undefined();
}


JS_FUNCTION(Get) {
  return jerry_create_undefined();
}

JS_FUNCTION(GetString) {
  DJS_CHECK_THIS(object);

  JS_DECLARE_THIS_PTR(sthings_rep, st);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, st);

#if defined(__TIZENRT__)
  iotjs_string_t jkey_string = JS_GET_ARG(0, string);
  const char* jkey_stirng_raw = iotjs_string_data(&jkey_string);
  char* data;

  st_things_representation_s *resp_rep = get_native_rep_handler(_this->jnative_cont);
  resp_rep->get_str_value(resp_rep, jkey_stirng_raw, &data);

  iotjs_string_destroy(&jkey_string);
  return iotjs_jval_create_string_raw(data);

#else
  return jerry_create_undefined();
#endif
}


iotjs_jval_t InitSmartThingsResponse() {
  iotjs_jval_t smart_things_response = jerry_create_external_function(SmartThingsResponse);

  iotjs_jval_t prototype = iotjs_jval_create_object();
  iotjs_jval_set_property_jval(smart_things_response, IOTJS_MAGIC_STRING_PROTOTYPE, prototype);
  iotjs_jval_set_method(prototype, "set", Set);
  iotjs_jval_set_method(prototype, "get", Get);
  iotjs_jval_set_method(prototype, "getString", GetString);

  jerry_release_value(prototype);

  return smart_things_response;
}
