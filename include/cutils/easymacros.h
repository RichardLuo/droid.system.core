/******************************************************************
 * @file   easymacros.h
 * @author Richard Luo
 * @date   2011/10/22 09:55:18
 * 
 * @brief  
 * 
 ****************************************************************** 
 */

#ifndef _EASYMACROS_H
#define _EASYMACROS_H

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#define easyutil_hexdump(buf_addr, buf_size) do {} while(0)

#include "base_file_and_line.h"

#define hexdump_info(buf,size,fmt, ...)                             \
    do {                                                            \
        int _n_; char _info_[256];                                  \
        _n_ = snprintf(_info_, sizeof(_info_)-1, "%s(%d): " fmt,  \
                       BASE_FILE_NAME(__FILE__), __LINE__,          \
                       ## __VA_ARGS__);                             \
        _info_[_n_] = 0;                                            \
        hexdump_l(_info_, buf, size);                               \
    } while(0)

#define LOG_IF_RETURN_CODE(cond,code,info_fmt, ...) \
    do {                                            \
        if ((cond)) {                               \
            LOGFL(info_fmt, ## __VA_ARGS__);        \
            return (code);                          \
        }                                           \
    } while(0)

#define LOG_IF_RETURN_NULL(cond,info_fmt, ...)  \
    do {                                        \
        if ((cond)) {                           \
            LOGFL(info_fmt, ## __VA_ARGS__);    \
            return (NULL);                      \
        }                                       \
    } while(0)

#define LOG_IF_RETURN_FALSE(cond,info_fmt, ...) \
    do {                                        \
        if ((cond)) {                           \
            LOGFL(info_fmt, ## __VA_ARGS__);    \
            return false;                       \
        }                                       \
    } while (0)

#define LOG_IFE_RETURN(error,info_fmt, ...)     \
    do {                                        \
        int error_code = (error);               \
        if ((error_code) != 0) {                \
            LOGFL(info_fmt, ## __VA_ARGS__);    \
            return (error_code);                \
        }                                       \
    } while(0)


#define LOG_IF_RETURN_VOID(cond,info_fmt, ...)  \
    do {                                        \
        if ((cond) != 0) {                      \
            LOGFL(info_fmt, ## __VA_ARGS__);    \
            return;                             \
        }                                       \
    } while(0)

#define LOG_IF_RETURN(cond,info_fmt, ...)       \
    do {                                        \
        if ((cond)) {                           \
            LOGFL(info_fmt, ## __VA_ARGS__);    \
            return (-1);                        \
        }                                       \
    } while (0)

#define LOG_IF_EXIT(cond,info_fmt, ...)             \
    do {                                            \
        if ((cond)) {                               \
            LOGFL(info_fmt, ## __VA_ARGS__);        \
            LOG_ALWAYS_FATAL(" CRASHED HERE!!");    \
        }                                           \
    } while (0)

#define LOG_IF_EXECUTE(cond,code,info_fmt, ...) \
    do {                                        \
        if ((cond)) {                           \
            LOGFL(info_fmt, ## __VA_ARGS__);    \
            code;                               \
        }                                       \
    } while (0)

#define LOG_IF_GOTO(cond,label,info_fmt, ...)   \
    do {                                        \
        if ((cond)) {                           \
            LOGFL(info_fmt, ## __VA_ARGS__);    \
            goto label;                         \
        }                                       \
    } while (0)

#define LOG_IF(cond,info_fmt, ...)              \
    do {                                        \
        if ((cond)) {                           \
            LOGFL(info_fmt, ## __VA_ARGS__);    \
        }                                       \
    } while(0)


#ifdef __cplusplus

class Int32ToCString {
    char mBuf[5];
    
public:

    explicit Int32ToCString(int32_t value) {
        mBuf[0] = (char) (value >> 24) & 0xFF;
        mBuf[1] = (char) (value >> 16) & 0xFF;
        mBuf[2] = (char) (value >> 8) & 0xFF;
        mBuf[3] = (char) (value & 0xFF);
        mBuf[4] = 0;
    }

    const char *c_str() const {
        return mBuf;
    }

};


#endif




#endif /* _EASYMACROS_H */



