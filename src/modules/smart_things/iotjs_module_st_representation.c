#include "iotjs_module_st_representation.h"

#include "iotjs_def.h"


IOTJS_DEFINE_NATIVE_HANDLE_INFO_THIS_MODULE(sthings_rep);
jerry_value_t create_js_prototype(jerry_value_t obj);


static iotjs_sthings_rep_t* iotjs_stings_rep_create(jerry_value_t jobj) {
  iotjs_sthings_rep_t* obj = IOTJS_ALLOC(iotjs_sthings_rep_t);
  IOTJS_VALIDATED_STRUCT_CONSTRUCTOR(iotjs_sthings_rep_t, obj);

  _this->jobject = jobj;
  jerry_set_object_native_pointer(jobj, obj, &this_module_native_info);

  return obj;
}


static void iotjs_sthings_rep_destroy(iotjs_sthings_rep_t* obj) {
  IOTJS_RELEASE(obj);
}


jerry_value_t iotjs_jstrings_rep_create(st_things_representation_s* rep) {
  jerry_value_t jobj = jerry_create_object();
  jerry_value_t jprototype = create_js_prototype(jobj);
  iotjs_jval_set_prototype(jobj, jprototype);
  jerry_release_value(jprototype);

  iotjs_sthings_rep_t* obj = iotjs_stings_rep_create(jobj);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, obj);

  _this->rep = rep;
  return jobj;
}


JS_FUNCTION(SmartThingsResponse) {
  DJS_CHECK_THIS();

  iotjs_stings_rep_create(JS_GET_THIS());

  return jerry_create_undefined();
}

static void req_set_data(const char* key, jerry_value_t jdata, st_things_representation_s* resp_rep) {
  // Get value
  if (jerry_value_is_string(jdata)) {
    iotjs_string_t jvalue_raw = iotjs_jval_as_string(jdata);
    const char* jvalue_string_raw = iotjs_string_data(&jvalue_raw);
    resp_rep->set_str_value(resp_rep, key, jvalue_string_raw);
    iotjs_string_destroy(&jvalue_raw);

  } else if (jerry_value_is_object(jdata)) {
    st_things_representation_s *temp_rep = st_things_create_representation_inst();

    jerry_value_t jdata_key = jerry_get_object_keys(jdata);
    jerry_value_t jdata_key_length = iotjs_jval_get_property(jdata_key, "length");
    size_t length = iotjs_jval_as_number(jdata_key_length);

    for (size_t i = 0; i < length; i++) {
      jerry_value_t jtemp_index = iotjs_jval_get_property_by_index(jdata_key, i);
      iotjs_string_t jtemp_key_string = iotjs_jval_as_string(jtemp_index);
      const char* jtemp_key_stirng_raw = iotjs_string_data(&jtemp_key_string);
      jerry_value_t jtemp_value = iotjs_jval_get_property(jdata, jtemp_key_stirng_raw);

      req_set_data(jtemp_key_stirng_raw, jtemp_value, temp_rep);

      jerry_release_value(jtemp_index);
      iotjs_string_destroy(&jtemp_key_string);
      jerry_release_value(jtemp_value);
    }

    jerry_release_value(jdata_key_length);
    jerry_release_value(jdata_key);

    resp_rep->set_object_value(resp_rep, key, temp_rep);

    st_things_destroy_representation_inst(temp_rep);

  } else if (jerry_value_is_number(jdata)) {
    double number_data = iotjs_jval_as_number(jdata);
    printf("[IOTJS] setInt: %lf - %lld\n", number_data, (int64_t)number_data);
    resp_rep->set_int_value(resp_rep, key, (int64_t)number_data);
  } else if (jerry_value_is_boolean(jdata)) {
    bool bool_data = jerry_get_boolean_value(jdata);
    resp_rep->set_bool_value(resp_rep, key, bool_data);
  } else {
    printf("[IOTJS][ST] cannot set value: not implementation");
  }
}

JS_FUNCTION(Set) {
  DJS_CHECK_THIS();

  JS_DECLARE_THIS_PTR(sthings_rep, st);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, st);

  iotjs_string_t jkey_string = JS_GET_ARG(0, string);
  const char* jkey_stirng_raw = iotjs_string_data(&jkey_string);
  req_set_data(jkey_stirng_raw, jargv[1], _this->rep);

  iotjs_string_destroy(&jkey_string);

  return jerry_create_undefined();
}


JS_FUNCTION(Get) {
  return jerry_create_undefined();
}


JS_FUNCTION(GetString) {
  DJS_CHECK_THIS();

  JS_DECLARE_THIS_PTR(sthings_rep, st);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, st);

  iotjs_string_t jkey_string = JS_GET_ARG(0, string);
  const char* jkey_stirng_raw = iotjs_string_data(&jkey_string);
  char* data;

  st_things_representation_s *resp_rep = _this->rep;
  resp_rep->get_str_value(resp_rep, jkey_stirng_raw, &data);

  iotjs_string_destroy(&jkey_string);
  return jerry_create_string((const jerry_char_t*)data);
}


JS_FUNCTION(GetBool) {
  DJS_CHECK_THIS();

  JS_DECLARE_THIS_PTR(sthings_rep, st);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, st);

  iotjs_string_t jkey_string = JS_GET_ARG(0, string);
  const char* jkey_stirng_raw = iotjs_string_data(&jkey_string);
  bool data;

  st_things_representation_s *resp_rep = _this->rep;
  resp_rep->get_bool_value(resp_rep, jkey_stirng_raw, &data);

  iotjs_string_destroy(&jkey_string);
  return jerry_create_boolean(data);
}


JS_FUNCTION(GetInt) {
  DJS_CHECK_THIS();

  JS_DECLARE_THIS_PTR(sthings_rep, st);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, st);

  iotjs_string_t jkey_string = JS_GET_ARG(0, string);
  const char* jkey_stirng_raw = iotjs_string_data(&jkey_string);
  int64_t data;

  st_things_representation_s *resp_rep = _this->rep;
  resp_rep->get_int_value(resp_rep, jkey_stirng_raw, &data);

  iotjs_string_destroy(&jkey_string);
  printf("[IOTJS] getInt: %lld - %lf\n", data, (double)data);
  return jerry_create_number((double)data);
}


JS_FUNCTION(GetDouble) {
  DJS_CHECK_THIS();

  JS_DECLARE_THIS_PTR(sthings_rep, st);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, st);

  iotjs_string_t jkey_string = JS_GET_ARG(0, string);
  const char* jkey_stirng_raw = iotjs_string_data(&jkey_string);
  double data;

  st_things_representation_s *resp_rep = _this->rep;
  resp_rep->get_double_value(resp_rep, jkey_stirng_raw, &data);

  iotjs_string_destroy(&jkey_string);
  return jerry_create_number(data);
}


JS_FUNCTION(GetByte) {
  DJS_CHECK_THIS();

  JS_DECLARE_THIS_PTR(sthings_rep, st);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, st);

  iotjs_string_t jkey_string = JS_GET_ARG(0, string);
  const char* jkey_stirng_raw = iotjs_string_data(&jkey_string);
  uint8_t* data;
  size_t size;

  st_things_representation_s *resp_rep = _this->rep;
  resp_rep->get_byte_value(resp_rep, jkey_stirng_raw, &data, &size);

  iotjs_string_destroy(&jkey_string);

  return iotjs_jval_create_byte_array(size, (const char*)data);;
}


JS_FUNCTION(GetObject) {
  DJS_CHECK_THIS();

  JS_DECLARE_THIS_PTR(sthings_rep, st);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, st);

  iotjs_string_t jkey_string = JS_GET_ARG(0, string);
  const char* jkey_stirng_raw = iotjs_string_data(&jkey_string);
  st_things_representation_s* data;

  st_things_representation_s *resp_rep = _this->rep;
  resp_rep->get_object_value(resp_rep, jkey_stirng_raw, &data);

  iotjs_string_destroy(&jkey_string);

  return iotjs_jstrings_rep_create(data);
}


JS_FUNCTION(GetStringArray) {
  DJS_CHECK_THIS();

  JS_DECLARE_THIS_PTR(sthings_rep, st);
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_sthings_rep_t, st);

  iotjs_string_t jkey_string = JS_GET_ARG(0, string);
  const char* jkey_stirng_raw = iotjs_string_data(&jkey_string);

  size_t length;
  char **data;

  st_things_representation_s *resp_rep = _this->rep;
  resp_rep->get_str_array_value(resp_rep, jkey_stirng_raw, &data, &length);
  iotjs_string_destroy(&jkey_string);

  jerry_value_t jarray = jerry_create_array(length);
  for (size_t i = 0; i < length; i++) {
   jerry_value_t val = jerry_create_string((const jerry_char_t*)data[i]);
   jerry_set_property_by_index(jarray, i, val);
   jerry_release_value(val);
  }

  return jarray;
}


jerry_value_t create_js_prototype(jerry_value_t obj) {
  jerry_value_t prototype = jerry_create_object();
  iotjs_jval_set_property_jval(obj, IOTJS_MAGIC_STRING_PROTOTYPE, prototype);
  iotjs_jval_set_method(prototype, "set", Set);
  iotjs_jval_set_method(prototype, "get", Get);
  iotjs_jval_set_method(prototype, "getBool", GetBool);
  iotjs_jval_set_method(prototype, "getInt", GetInt);
  iotjs_jval_set_method(prototype, "getDouble", GetDouble);
  iotjs_jval_set_method(prototype, "getString", GetString);
  iotjs_jval_set_method(prototype, "getObject", GetObject);
  iotjs_jval_set_method(prototype, "getByte", GetByte);
//  iotjs_jval_set_method(prototype, "getIntArray", GetIntArray);
//  iotjs_jval_set_method(prototype, "getDoubleArray", GetDoubleArray);
 iotjs_jval_set_method(prototype, "getStringArray", GetStringArray);
//  iotjs_jval_set_method(prototype, "getObjectArray", GetObjectArray);

  return prototype;
}


jerry_value_t InitSTRepresentation() {
  jerry_value_t smart_things_rep = jerry_create_external_function(SmartThingsResponse);

  jerry_value_t prototype = create_js_prototype(smart_things_rep);

  jerry_release_value(prototype);

  return smart_things_rep;
}
