#ifndef RECIPE_H
# define RECIPE_H

# include <sys/stat.h>

# include <stdlib.h>
# include <stdio.h>
# include <libgen.h>

# include <errno.h>
# include <string.h>

# include <plist/plist.h>

# include <vitamin.h>
# include <config.h>

# include <url_queue.h>

# define OC_DIST				"RELEASE"
# define OC_REPO				"https://github.com/acidanthera/OpenCorePkg"
# define OC_URL_TEMPLATE		OC_REPO"/releases/download/%s/OpenCore-%s-"OC_DIST".zip"

# define DEST_MODE				(S_IRWXU | S_IRGRP | S_IROTH)
# define CACHE_DIR				"/tmp"

# define RECIPE_GET(root, key)	plist_dict_get_item(root, key);


typedef struct	s_recipe {
	char		*name;
	char		*oc_version;
	t_vitamin	**drivers;
	t_vitamin	**kexts;
	t_vitamin	**ssdts;
	plist_t		config;
	char		**urls;
}				t_recipe;

t_recipe	*recipe_load(const char *filepath);
void		recipe_free(t_recipe **recipe);

char		**recipe_urls(t_recipe *recipe);

int			recipe_print(t_recipe *recipe);
int			recipe_bake(t_recipe *recipe, char *destination);

#endif
