/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// C/C++ logging functions.  See the logging documentation for API details.
//
// We'd like these to be available from C code (in case we import some from
// somewhere), so this has a C interface.
//
// The output will be correct when the log file is shared between multiple
// threads and/or multiple processes so long as the operating system
// supports O_APPEND.  These calls have mutex-protected data structures
// and so are NOT reentrant.  Do not use LOG in a signal handler.
//
#ifndef _LIBS_CUTILS_LOG_H
#define _LIBS_CUTILS_LOG_H

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef HAVE_PTHREADS
#include <pthread.h>
#endif
#include <stdarg.h>

#include <cutils/uio.h>
#include <cutils/logd.h>

#ifdef __cplusplus
extern "C" {
#endif

    // ---------------------------------------------------------------------

    /*
     * Normally we strip LOGV (VERBOSE messages) from release builds.
     * You can modify this (for example with "#define LOG_NDEBUG 0"
     * at the top of your source file) to change that behavior.
     */
#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG 1
#else
#define LOG_NDEBUG 0
#endif
#endif

    /*
     * This is the local tag used for the following simplified
     * logging macros.  You can change this preprocessor definition
     * before using the other macros to change the tag.
     */
#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

    // ---------------------------------------------------------------------


#define CONDITION(cond)     (__builtin_expect((cond)!=0, 0))

#ifndef LOGV_IF
#if LOG_NDEBUG
#define LOGV_IF(cond, ...)   ((void)0)
#else
#define LOGV_IF(cond, ...)                                      \
    ( (CONDITION(cond))                                         \
      ? ((void)DROID_LOG(LOG_VERBOSE, LOG_TAG, __VA_ARGS__))    \
      : (void)0 )
#endif
#endif

    /*
     * Simplified macro to send a debug log message using the current LOG_TAG.
     */
#ifndef LOGFL
#define LOGFL(fmt, ...)         LOG_FLT(LOG_ERROR, LOG_TAG, fmt, ## __VA_ARGS__)
#define LOGFLT(fmt, ...)        LOG_FL_TIMESTAMP(LOG_ERROR, LOG_TAG, fmt, ## __VA_ARGS__)
#define LOGFLV(fmt, ...)        LOG_FL_THIS_TID(LOG_ERROR, LOG_TAG, fmt, ## __VA_ARGS__)
#define LOGFL_this(fmt, ...)    LOG_FL_THIS_TID(LOG_ERROR, LOG_TAG, fmt, ## __VA_ARGS__)
    //#define LOGFL LOGV
#endif

#ifndef LOGD_IF
#define LOGD_IF(cond, ...)                                  \
    ( (CONDITION(cond))                                     \
      ? ((void)DROID_LOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__))  \
      : (void)0 )
#endif

#ifndef LOGI_IF
#define LOGI_IF(cond, ...)                                  \
    ( (CONDITION(cond))                                     \
      ? ((void)DROID_LOG(LOG_INFO, LOG_TAG, __VA_ARGS__))   \
      : (void)0 )
#endif

#ifndef LOGW_IF
#define LOGW_IF(cond, ...)                                  \
    ( (CONDITION(cond))                                     \
      ? ((void)DROID_LOG(LOG_WARN, LOG_TAG, __VA_ARGS__))   \
      : (void)0 )
#endif

    
#ifndef LOG_FL                   // log with file and line info

#include <sys/types.h>
#include <sys/syscall.h>  
#define gettid() syscall(__NR_gettid)

#include "base_file_and_line.h"
    
#define LOGFL_BUFFER_SIZE 1024
#define LOG_FL(priority, tag, fmt, ...)                     \
    do {                                                    \
        char _buf_[LOGFL_BUFFER_SIZE];                      \
        const char *_file_name_ = __FILE__;                 \
        const size_t _last_ = sizeof(_buf_)-1;              \
        snprintf(_buf_, _last_, "%s(%d)#%s " fmt,           \
                 BASE_FILE_NAME(_file_name_), __LINE__,     \
                 BASE_FUNC_NAME(__func__), ## __VA_ARGS__); \
        _buf_[_last_] = '\0';                               \
        LOG_PRI_PUTS(ANDROID_##priority, tag, _buf_);       \
    } while (0)

#define LOG_FLT(priority, tag, fmt, ...)                            \
    do {                                                            \
        char _buf_[LOGFL_BUFFER_SIZE];                              \
        const char *_file_name_ = __FILE__;                         \
        const size_t _last_ = sizeof(_buf_)-1;                      \
        snprintf(_buf_, _last_, "tid:%ld %s(%d)#%s " fmt,           \
                 gettid(), BASE_FILE_NAME(_file_name_), __LINE__,   \
                 BASE_FUNC_NAME(__func__), ## __VA_ARGS__);         \
        _buf_[_last_] = '\0';                                       \
        LOG_PRI_PUTS(ANDROID_##priority, tag, _buf_);               \
    } while (0)


#define LOG_FL_THIS_TID(priority, tag, fmt, ...)                    \
    do {                                                            \
        char _buf_[LOGFL_BUFFER_SIZE];                              \
        const char *_file_name_ = __FILE__;                         \
        const size_t _last_ = sizeof(_buf_)-1;                      \
        snprintf(_buf_, _last_, "tid:%ld this:%p %s(%d)#%s " fmt,   \
                 gettid(), this,                                    \
                 BASE_FILE_NAME(_file_name_), __LINE__,             \
                 BASE_FUNC_NAME(__func__), ## __VA_ARGS__);         \
        _buf_[_last_] = '\0';                                       \
        LOG_PRI_PUTS(ANDROID_##priority, tag, _buf_);               \
    } while (0)

#define LOG_FL_TIMESTAMP(priority, tag, fmt, ...)               \
    do {                                                        \
        char _buf_[LOGFL_BUFFER_SIZE];                          \
        const char *_file_name_ = __FILE__;                     \
        const size_t _last_ = sizeof(_buf_)-1;                  \
        struct timeval __tv__;                                  \
        gettimeofday(&__tv__, NULL);                            \
        __tv__.tv_sec %= 1000;                                  \
        snprintf(_buf_, _last_,                                 \
                 "tid:%ld this:%p ts:%ld.%03ld %s(%d)#%s " fmt,  \
                 gettid(), this,                                \
                 __tv__.tv_sec, (__tv__.tv_usec)/1000,          \
                 BASE_FILE_NAME(_file_name_), __LINE__,         \
                 BASE_FUNC_NAME(__func__), ## __VA_ARGS__);     \
        _buf_[_last_] = 0;                                      \
        LOG_PRI_PUTS(ANDROID_##priority, tag, _buf_);           \
    } while (0)

#endif  // LOG_FL


#ifndef LOGI
#if LOG_NDEBUG
#define LOGI(...)   do {} while(0)
#else
#define LOGI(fmt, ...) LOG_FL(LOG_INFO, LOG_TAG, fmt, ## __VA_ARGS__)
// #define LOGI(...) ((void)DROID_LOG(LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef LOGV
#if LOG_NDEBUG
#define LOGV(...)   do {} while(0)
#else
#define LOGV(fmt, ...) LOG_FL(LOG_VERBOSE, LOG_TAG, fmt, ## __VA_ARGS__)
// #define LOGV(...) ((void)DROID_LOG(LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef LOGD
#if LOG_NDEBUG
#define LOGD(...)   do {} while(0)
#else
#define LOGD(fmt, ...) LOG_FL(LOG_DEBUG, LOG_TAG, fmt, ## __VA_ARGS__)
// #define LOGD(...) ((void)DROID_LOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef LOGW
#if LOG_NDEBUG
#define LOGW(...)   do {} while(0)
#else
#define LOGW(fmt, ...) LOG_FL(LOG_WARN, LOG_TAG, fmt, ## __VA_ARGS__)
#endif
#endif

#ifndef LOGE
#if LOG_NDEBUG
#define LOGE(...)   do {} while(0)
#else
#define LOGE(fmt, ...) LOG_FL(LOG_ERROR, LOG_TAG, fmt, ## __VA_ARGS__)
// #define LOGE(fmt, ...) LOG_FL(LOG_ERROR, LOG_TAG, fmt, ## __VA_ARGS__)
#endif
#endif

#ifndef LOGE_IF
#define LOGE_IF(cond, ...)                                  \
    ( (CONDITION(cond))                                     \
      ? ((void)DROID_LOG(LOG_ERROR, LOG_TAG, __VA_ARGS__))  \
      : (void)0 )
#endif

    // ---------------------------------------------------------------------

    /*
     * Conditional based on whether the current LOG_TAG is enabled at
     * verbose priority.
     */
#ifndef IF_LOGV
#if LOG_NDEBUG
#define IF_LOGV() if (false)
#else
#define IF_LOGV() IF_LOG(LOG_VERBOSE, LOG_TAG)
#endif
#endif

    /*
     * Conditional based on whether the current LOG_TAG is enabled at
     * debug priority.
     */
#ifndef IF_LOGD
#define IF_LOGD() IF_LOG(LOG_DEBUG, LOG_TAG)
#endif

    /*
     * Conditional based on whether the current LOG_TAG is enabled at
     * info priority.
     */
#ifndef IF_LOGI
#define IF_LOGI() IF_LOG(LOG_INFO, LOG_TAG)
#endif

    /*
     * Conditional based on whether the current LOG_TAG is enabled at
     * warn priority.
     */
#ifndef IF_LOGW
#define IF_LOGW() IF_LOG(LOG_WARN, LOG_TAG)
#endif

    /*
     * Conditional based on whether the current LOG_TAG is enabled at
     * error priority.
     */
#ifndef IF_LOGE
#define IF_LOGE() IF_LOG(LOG_ERROR, LOG_TAG)
#endif


    // ---------------------------------------------------------------------

    /*
     * Simplified macro to send a verbose system log message using the current LOG_TAG.
     */
#ifndef SLOGV
#if LOG_NDEBUG
#define SLOGV(...)   ((void)0)
#else
#define SLOGV(...) ((void)__android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#endif
#endif

#define CONDITION(cond)     (__builtin_expect((cond)!=0, 0))

#ifndef SLOGV_IF
#if LOG_NDEBUG
#define SLOGV_IF(cond, ...)   ((void)0)
#else
#define SLOGV_IF(cond, ...)                                             \
    ( (CONDITION(cond))                                                 \
      ? ((void)__android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)) \
      : (void)0 )
#endif
#endif

    /*
     * Simplified macro to send a debug system log message using the current LOG_TAG.
     */
#ifndef SLOGD
#define SLOGD(...) ((void)__android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGD_IF
#define SLOGD_IF(cond, ...)                                             \
    ( (CONDITION(cond))                                                 \
      ? ((void)__android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)) \
      : (void)0 )
#endif

    /*
     * Simplified macro to send an info system log message using the current LOG_TAG.
     */
#ifndef SLOGI
#define SLOGI(...) ((void)__android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGI_IF
#define SLOGI_IF(cond, ...)                                             \
    ( (CONDITION(cond))                                                 \
      ? ((void)__android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)) \
      : (void)0 )
#endif

    /*
     * Simplified macro to send a warning system log message using the current LOG_TAG.
     */
#ifndef SLOGW
#define SLOGW(...) ((void)__android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGW_IF
#define SLOGW_IF(cond, ...)                                             \
    ( (CONDITION(cond))                                                 \
      ? ((void)__android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)) \
      : (void)0 )
#endif

    /*
     * Simplified macro to send an error system log message using the current LOG_TAG.
     */
#ifndef SLOGE
#define SLOGE(...) ((void)__android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGE_IF
#define SLOGE_IF(cond, ...)                                             \
    ( (CONDITION(cond))                                                 \
      ? ((void)__android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)) \
      : (void)0 )
#endif

    

    // ---------------------------------------------------------------------

    /*
     * Log a fatal error.  If the given condition fails, this stops program
     * execution like a normal assertion, but also generating the given message.
     * It is NOT stripped from release builds.  Note that the condition test
     * is -inverted- from the normal assert() semantics.
     */
#define LOG_ALWAYS_FATAL_IF(cond, ...)                              \
    ( (CONDITION(cond))                                             \
      ? ((void)android_printAssert(#cond, LOG_TAG, __VA_ARGS__))    \
      : (void)0 )

#define LOG_ALWAYS_FATAL(...)                                   \
    ( ((void)android_printAssert(NULL, LOG_TAG, __VA_ARGS__)) )

    /*
     * Versions of LOG_ALWAYS_FATAL_IF and LOG_ALWAYS_FATAL that
     * are stripped out of release builds.
     */
#if LOG_NDEBUG

#define LOG_FATAL_IF(cond, ...) ((void)0)
#define LOG_FATAL(...) ((void)0)

#else

#define LOG_FATAL_IF(cond, ...) LOG_ALWAYS_FATAL_IF(cond, __VA_ARGS__)
#define LOG_FATAL(...) LOG_ALWAYS_FATAL(__VA_ARGS__)

#endif

    /*
     * Assertion that generates a log message when the assertion fails.
     * Stripped out of release builds.  Uses the current LOG_TAG.
     */
#define LOG_ASSERT(cond, ...) LOG_FATAL_IF(!(cond), __VA_ARGS__)
    //#define LOG_ASSERT(cond) LOG_FATAL_IF(!(cond), "Assertion failed: " #cond)

    /*
     * Basic log message macro.
     *
     * Example:
     *  DROID_LOG(LOG_WARN, NULL, "Failed with error %d", errno);
     *
     * The second argument may be NULL or "" to indicate the "global" tag.
     */
#ifndef DROID_LOG
#define DROID_LOG(priority, tag, ...)               \
    LOG_PRI(ANDROID_##priority, tag, __VA_ARGS__)
#endif

    /*
     * Log macro that allows you to specify a number for the priority.
     */
#ifndef LOG_PRI
#define LOG_PRI(priority, tag, ...)                 \
    android_printLog(priority, tag, __VA_ARGS__)
#endif

#ifndef LOG_PRI_PUTS
#define LOG_PRI_PUTS(priority, tag, buf)        \
    android_printLog(priority, tag, "%s", buf)
#endif



    /*
     * Log macro that allows you to pass in a varargs ("args" is a va_list).
     */
#ifndef LOG_PRI_VA
#define LOG_PRI_VA(priority, tag, fmt, args)            \
    android_vprintLog(priority, NULL, tag, fmt, args)
#endif

    /*
     * Conditional given a desired logging priority and tag.
     */
#ifndef IF_LOG
#define IF_LOG(priority, tag)                       \
    if (android_testLog(ANDROID_##priority, tag))
#endif

    // ---------------------------------------------------------------------

    /*
     * Event logging.
     */

    /*
     * Event log entry types.  These must match up with the declarations in
     * java/android/android/util/EventLog.java.
     */
    typedef enum {
        EVENT_TYPE_INT      = 0,
        EVENT_TYPE_LONG     = 1,
        EVENT_TYPE_STRING   = 2,
        EVENT_TYPE_LIST     = 3,
    } AndroidEventLogType;


#define LOG_EVENT_INT(_tag, _value) {                               \
        int intBuf = _value;                                        \
        (void) android_btWriteLog(_tag, EVENT_TYPE_INT, &intBuf,    \
                                  sizeof(intBuf));                  \
    }
#define LOG_EVENT_LONG(_tag, _value) {                              \
        long long longBuf = _value;                                 \
        (void) android_btWriteLog(_tag, EVENT_TYPE_LONG, &longBuf,  \
                                  sizeof(longBuf));                 \
    }
#define LOG_EVENT_STRING(_tag, _value)                                  \
    ((void) 0)  /* not implemented -- must combine len with string */
    /* TODO: something for LIST */

    /*
     * ===========================================================================
     *
     * The stuff in the rest of this file should not be used directly.
     */

#define android_printLog(prio, tag, fmt...)     \
    __android_log_print(prio, tag, fmt)

#define android_vprintLog(prio, cond, tag, fmt...)  \
    __android_log_vprint(prio, tag, fmt)

#define android_printAssert(cond, tag, fmt...)  \
    __android_log_assert(cond, tag, fmt)

#define android_writeLog(prio, tag, text)       \
    __android_log_write(prio, tag, text)

#define android_bWriteLog(tag, payload, len)    \
    __android_log_bwrite(tag, payload, len)
#define android_btWriteLog(tag, type, payload, len) \
    __android_log_btwrite(tag, type, payload, len)
	
    // TODO: remove these prototypes and their users
#define android_testLog(prio, tag) (1)
#define android_writevLog(vec,num) do{}while(0)
#define android_write1Log(str,len) do{}while (0)
#define android_setMinPriority(tag, prio) do{}while(0)
    //#define android_logToCallback(func) do{}while(0)
#define android_logToFile(tag, file) (0)
#define android_logToFd(tag, fd) (0)

    typedef enum {
        LOG_ID_MAIN = 0,
        LOG_ID_RADIO = 1,
        LOG_ID_EVENTS = 2,
        LOG_ID_SYSTEM = 3,

        LOG_ID_MAX
    } log_id_t;

    /*
     * Send a simple string to the log.
     */
    int __android_log_buf_write(int bufID, int prio, const char *tag, const char *text);
    int __android_log_buf_print(int bufID, int prio, const char *tag, const char *fmt, ...);

#include <libgen.h>
#define ddprint(format, ...) do {                   \
        char path[sizeof __FILE__];                 \
        memset(path, 0, sizeof(path));              \
        size_t sz = sizeof(path);                   \
        strncpy(path, __FILE__, sizeof(path));      \
        char *pp = basename(path);                  \
        fprintf(stdout, "[%s:%d]> ", pp, __LINE__);	\
        fprintf(stdout, format, ## __VA_ARGS__);    \
        fputs("\n", stdout);                        \
        fflush(NULL);                               \
    } while(0)


#ifdef __cplusplus
}
#endif

#include <cutils/easyutils.h>

#endif // _LIBS_CUTILS_LOG_H
