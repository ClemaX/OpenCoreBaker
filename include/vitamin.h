#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include <plist/plist.h>

#include <url.h>
#include <archive.h>

#define VIT_REGULAR	1
#define VIT_DIRECTORY	2

typedef char	t_vitamin_t;

typedef struct	s_vitamin
{
	char		*name;
	char		*release_url;
	char		*path;
	t_vitamin_t	type;
}				t_vitamin;

t_vitamin		*vitamin_load(plist_t vitamin_dict, t_vitamin_t type);
t_vitamin		**vitamins_load(plist_t vitamins_array, t_vitamin_t type);
void			vitamin_free(t_vitamin **vitamin);
void			vitamins_free(t_vitamin ***vitamins);

size_t			vitamins_size(t_vitamin **vitamins);
char			**vitamins_urls(t_vitamin **vitamins, char **urls);

int				vitamin_install(t_vitamin *vitamin,
	const char *cache, const char *dest);
int				vitamins_install(t_vitamin **vitamin,
	const char *cache, const char *dest);

int				vitamin_print(t_vitamin *vitamin);
int				vitamins_print(t_vitamin **vitamins);
