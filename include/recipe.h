#pragma once

#include <sys/stat.h>

#include <oc.h>
#include <vitamin.h>
#include <config.h>

#define VIT_KEXT	VIT_DIRECTORY
#define VIT_DRIVER	VIT_REGULAR
#define VIT_SSDT	VIT_REGULAR

#define DEST_MODE				(S_IRWXU | S_IRGRP | S_IROTH)
#define CACHE_DIR				"/tmp"

#define RECIPE_GET(root, key)	plist_dict_get_item(root, key);


typedef struct	s_recipe {
	const char	*work_dir;
	char		*name;
	t_oc		oc;
	t_vitamin	**drivers;
	t_vitamin	**kexts;
	t_vitamin	**ssdts;
	char		**urls;
}				t_recipe;

t_recipe	*recipe_load(const char *work_dir, const char *filepath);
void		recipe_free(t_recipe **recipe);

char		**recipe_urls(t_recipe *recipe);

int			recipe_print(t_recipe *recipe);
int			recipe_bake(t_recipe *recipe, const char *destination);
