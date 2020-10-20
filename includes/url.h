#ifndef URL_H
# define URL_H

# include <stdlib.h>
# include <stdio.h>
# include <string.h>

void	urls_free(char ***urls);

int		url_is_http(char *url);
void	urls_append(char **urls, char *url);
int		urls_print(char **urls);

#endif
