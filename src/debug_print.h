#ifndef __DEBUG_PRINT_H
#define __DEBUG_PRINT_H

#if CONFIG_DEBUG_PRINT
void debug_print(const char *s);
#else
#  define debug_print(s)
#endif

#endif // debug_print.h
