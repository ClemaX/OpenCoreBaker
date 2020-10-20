#ifndef URL_H
# define URL_H

# include <stdlib.h>
# include <stdio.h>

typedef	char * t_url;

int	url_open(t_url url);
int	url_close(t_url url);

int	url_print(t_url url);

#endif
