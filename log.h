/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#define LOG_VERSION "0.1.0"

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  const char *funcname;
  struct tm *time;
  void *udata;
  int line;
  int level;
} log_Event;

typedef void (*log_LogFn)(log_Event *ev);
typedef void (*log_LockFn)(bool lock, void *udata);

enum { TMLOG_TRACE, TMLOG_DEBUG, TMLOG_INFO, TMLOG_WARN, TMLOG_ERROR, TMLOG_FATAL };

#define log_trace(...) log_log(TMLOG_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_debug(...) log_log(TMLOG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_info(...)  log_log(TMLOG_INFO,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_warn(...)  log_log(TMLOG_WARN,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_error(...) log_log(TMLOG_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_fatal(...) log_log(TMLOG_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

const char* log_level_string(int level);
void log_set_lock(log_LockFn fn, void *udata);
void log_set_level(int level);
void log_set_quiet(bool enable);
int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(FILE *fp, int level);

void log_log(
  int level,
  const char *file,
  int line,
  const char *funcname,
  const char *fmt,
  ...
);

#endif
