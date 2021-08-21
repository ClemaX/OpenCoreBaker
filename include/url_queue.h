#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>

#include <errno.h>
#include <string.h>

#include <curl/curl.h>

#include <file_utils.h>

#define CACHE_TEMPLATE	"OpenCoreBaker.XXXXXX"

#define MAXCONNECTS	10L
#define MAXREDIRS		50L
#define USERAGENT		"OpenCoreBaker/1.0"
#define TCP_KEEPALIVE	1L

typedef	struct	s_url_queue
{
	CURLM	*curl_multi;
	char	*cache;
	char	**queue;
	char	**current;
}				t_url_queue;

typedef struct	s_url_dl
{
	CURL	*handle;
	FILE	*file;
	char	*url;
}				t_url_dl;


t_url_dl		*url_dl_init(char* dest, char *url);
void			url_dl_cleanup(t_url_dl **dl);

t_url_queue		*url_queue_init(char *cache_dest, char **urls);
void			url_queue_free(t_url_queue **queue, char del_cache);
void			url_queue_cleanup(t_url_queue *queue);

int				url_queue_add(t_url_queue *handle, char *url);
int				url_queue_fetch(t_url_queue *queue);
