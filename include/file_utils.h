#pragma once

#include <stdio.h>
#include <string.h>
#include <ftw.h>

#include <zip.h>

#ifndef FILE_BUFFER_SIZE
# define FILE_BUFFER_SIZE	8192
#endif

int	fcopy(FILE *source, FILE *destination);
int	zcopy(zip_file_t *source, FILE *destination);

char	*abasename(const char *str);
char	*adirname(const char *str);

int		rmrf(char *path);
