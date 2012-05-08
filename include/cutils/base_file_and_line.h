/******************************************************************
 * @file   base_file_and_line.h
 * @author Richard Luo
 * @date   2011/11/28 12:18:41
 * 
 * @brief  
 * 
 ****************************************************************** 
 */

#ifndef _BASE_FILE_AND_LINE_H
#define _BASE_FILE_AND_LINE_H

#include <string.h>             // for strrchr

#define BASE_FILE_NAME(full_name) ((strrchr(full_name, '/') ? : full_name - 1) + 1)

static inline const char* BASE_FUNC_NAME (const char *func_name)
{
    const char *p = strrchr(func_name, '(');
    if (!p) return "";

    --p;
    while (
        (*p >= 'a' && *p <= 'z')  ||
        (*p >= 'A' && *p <= 'Z')  ||
        (*p == '_')             ||
        (*p >= '0' && *p <= '9')
        )
        --p;
    return p;
}

#endif /* _BASE_FILE_AND_LINE_H */



