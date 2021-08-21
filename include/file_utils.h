#pragma once

#ifdef __linux__
# include <linux/limits.h>
#else
# include <limits.h>
# ifdef WIN32
#  define mkdir(path) _mkdir(path)
# else
#  include <unistd.h>
# endif
#endif

#include <sys/stat.h>
#include <stdio.h>

#include <zip.h>

#ifndef FILE_BUFFER_SIZE
# define FILE_BUFFER_SIZE	8192
#endif

int	fcopy(FILE *source, FILE *destination);
int	copy(const char *source_path, const char *destination_path);

int	zcopy(zip_file_t *source, FILE *destination);

char	*abasename(const char *str);
char	*adirname(const char *str);

int		rmrf(char *path);
int		mkdir_p(const char *path, mode_t mode);

