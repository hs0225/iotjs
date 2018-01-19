/* Copyright 2018-present Samsung Electronics Co., Ltd. and other contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "jerryscript-port.h"
#include "jerryscript.h"
#include <stdlib.h>
#include <dlog/dlog.h>
#define USE_IOTJS_THREAD 1

/**
 * Aborts the program.
 */
void
jerry_port_default_set_log_level (jerry_log_level_t level) /**< log level */
{
} /* jerry_port_default_set_log_level */

void jerry_port_fatal(jerry_fatal_code_t code) {
#ifndef DISABLE_EXTRA_API
  if (code != 0
      && code != ERR_OUT_OF_MEMORY)
  {
    abort ();
  }
  else
  {
#endif /* !DISABLE_EXTRA_API */
    exit (code);
#ifndef DISABLE_EXTRA_API
  }
#endif /* !DISABLE_EXTRA_API */
} /* jerry_port_fatal */

/**
 * Provide log message implementation for the engine.
 */
void jerry_port_log(jerry_log_level_t level, /**< log level */
                    const char *format,      /**< format string */
                    ...) {                   /**< parameters */
  va_list args;
  va_start(args, format);
  dlog_vprint(DLOG_INFO, "JERRY", format, args);
  va_end(args);
} /* jerry_port_log */

/**
 * Dummy function to get the time zone.
 *
 * @return true
 */
bool jerry_port_get_time_zone(jerry_time_zone_t *tz_p) {
  /* We live in UTC. */
  tz_p->offset = 0;
  tz_p->daylight_saving_time = 0;

  return true;
} /* jerry_port_get_time_zone */

/**
 * Dummy function to get the current time.
 *
 * @return 0
 */
double jerry_port_get_current_time(void) {
  return 0;
} /* jerry_port_get_current_time */

/**
 * Provide the implementation of jerryx_port_handler_print_char.
 * Uses 'printf' to print a single character to standard output.
 */
void jerryx_port_handler_print_char(char c) { /**< the character to print */
  // printf("%c", c);
} /* jerryx_port_handler_print_char */
