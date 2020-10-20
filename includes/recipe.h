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

# include <url.h>

# define DEST_MODE	(S_IRWXU | S_IRGRP | S_IROTH)

typedef struct	s_recipe {
	char		*name;
	char		*oc_version;
	t_vitamin	**drivers;
	t_vitamin	**kexts;
	t_vitamin	**ssdts;
	plist_t		*config;
}				t_recipe;

t_recipe	*recipe_load(const char *filepath);
void		recipe_free(t_recipe **recipe);

int			recipe_print(t_recipe *recipe);
int			recipe_bake(t_recipe *recipe, t_url destination);

#endif
