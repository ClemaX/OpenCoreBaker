#pragma once

#include <stdio.h>

#ifdef DEBUG
# define debug(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
# define debug(fmt, ...) ((void)0)
/*
# define info(fmt, ...) ((void)0)
# define warning(fmt, ...) ((void)0)
# define error(fmt, ...) ((void)0)
*/
#endif

# define info(fmt, ...) printf(fmt, ##__VA_ARGS__)
# define warning(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
# define error(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
