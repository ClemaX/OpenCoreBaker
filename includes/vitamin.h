#ifndef VITAMIN_H
# define VITAMIN_H

# include <stdlib.h>
# include <stdio.h>

# include <plist/plist.h>

typedef struct	s_vitamin
{
	char	*name;
	char	*release_url;
	char	*path;
}				t_vitamin;

t_vitamin		*vitamin_load(plist_t vitamin_dict);
t_vitamin		**vitamins_load(plist_t vitamins_array);

int				vitamin_print(t_vitamin *vitamin);
int				vitamins_print(t_vitamin **vitamins);

void			vitamin_free(t_vitamin **vitamin);
void			vitamins_free(t_vitamin ***vitamins);

#endif
