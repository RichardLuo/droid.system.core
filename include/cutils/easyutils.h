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

#ifdef __cplusplus
extern "C" {
#endif

    int read_n(int fd, void *data, size_t size);

    void hexdump_l(const void *data, int len);


#ifdef __cplusplus
}
#endif


#endif /* _EASYUTIL_H */



