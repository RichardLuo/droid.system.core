/******************************************************************
 * @file   easyutil.h
 * @author Richard Luo
 * @date   2011/08/29 07:37:38
 * 
 * @brief  
 * 
 ****************************************************************** 
 */

#ifndef _EASYUTIL_H
#define _EASYUTIL_H

#include <cutils/easymacros.h>
#include <string>
#include "format.h"

int read_n_l(int fd, void *data, size_t size);

std::string hexdump_l(const void *data, int len);

#define hexdump_info(buf,size,strfmt, ...)                  \
    do {                                                    \
        char _info_[256];                                   \
        const size_t _max_ = sizeof(_info_) - 1;            \
        const int _len_ = snprintf(_info_, _max_,           \
                                   strfmt, ## __VA_ARGS__); \
        _info_[_len_] = '\0';                               \
        std::string _res_(_info_, _len_);                   \
        _res_ += hexdump_l(buf, size);                      \
        LOGW("%s", _res_.c_str());                          \
    } while(0)


#endif /* _EASYUTIL_H */



