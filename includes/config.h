#ifndef CONFIG_H
# define CONFIG_H

# include <stdio.h>

# include <plist/plist.h>

int	config_print(plist_t config_dict);
int config_write(char *filename, plist_t config);

#endif
