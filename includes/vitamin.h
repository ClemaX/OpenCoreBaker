#ifndef VITAMIN_H
# define VITAMIN_H

# include <stdlib.h>
# include <stdio.h>
# include <string.h>

# include <plist/plist.h>

# include <url.h>

typedef struct	s_vitamin
{
	char	*name;
	char	*release_url;
	char	*path;
}				t_vitamin;

t_vitamin		*vitamin_load(plist_t vitamin_dict);
t_vitamin		**vitamins_load(plist_t vitamins_array);

size_t			vitamins_size(t_vitamin **vitamins);
char			**vitamins_urls(t_vitamin **vitamins, char **urls);

int				vitamin_print(t_vitamin *vitamin);
int				vitamins_print(t_vitamin **vitamins);

void			vitamin_free(t_vitamin **vitamin);
void			vitamins_free(t_vitamin ***vitamins);

#endif
