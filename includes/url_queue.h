#ifndef URL_QUEUE_H
# define URL_QUEUE_H

# ifndef WIN32
#  include <unistd.h>
# endif

# include <stdlib.h>
# include <stdio.h>

# include <errno.h>
# include <string.h>

# include <curl/curl.h>

# define CACHE_TEMPLATE	"OpenCoreBaker.XXXXXX"

# define MAXCONNECTS	10L
# define MAXREDIRS		50L
# define USERAGENT		"OpenCoreBaker/1.0"
# define TCP_KEEPALIVE	1L

typedef	struct	s_url_queue
{
	CURLM	*curl_multi;
	char	*cache;
	char	**queue;
	char	**current;
}				t_url_queue;


t_url_queue		*url_queue_init(char *cache_dest, char **urls);
void			url_queue_cleanup(t_url_queue **queue);

int				url_queue_add(t_url_queue *handle, char *url);
int				url_queue_fetch(t_url_queue *queue, char *url);

#endif
