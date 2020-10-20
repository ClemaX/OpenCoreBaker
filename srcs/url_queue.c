#include <url_queue.h>

static FILE	*url_fopen_dest(char* dest, char *url)
{
	const char		*filename = basename(url);
	const size_t	length = strlen(dest) + strlen(filename) + 1;
	char			*filepath = asprintf("%s/%s", dest, filepath);
	FILE			*file = (filepath) ? fopen(filepath, "w") : NULL;

	if (!file)
		printf("Error: %s: %s\n", filepath, strerror(errno));
	free(filepath);
	return (file);
}

int			url_queue_add(t_url_queue *handle, char *url)
{
	FILE	*file = url_fopen_dest(handle->cache, url);

	if (file)
	{
		CURL	*eh = curl_easy_init();

		if (eh)
		{
			curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, NULL);
			curl_easy_setopt(eh, CURLOPT_WRITEDATA, file);
			curl_easy_setopt(eh, CURLOPT_URL, url);
			curl_easy_setopt(eh, CURLOPT_PRIVATE, url);

			if (!curl_multi_add_handle(handle->curl_multi, eh))
				return (0);
			perror("Error adding curl handle");
		}
		perror("Error creating curl handle");
	}
	return (1);
}

t_url_queue	*url_queue_init(char *cache_dest, char **urls)
{
	t_url_queue	*queue = NULL;

	if (cache_dest && (queue = malloc(sizeof(*queue))))
	{
		size_t	length = strlen(cache_dest) + strlen(CACHE_TEMPLATE) + 1;
		char	*template = malloc(sizeof(*template) * (length + 1));

		if (template
		&& (queue->cache = mkdtemp(template))
		&& (queue->curl_multi = curl_multi_init()))
		{
			printf("Caching files to '%s'!\n", queue->cache);
			curl_multi_setopt(queue->curl_multi, CURLMOPT_MAXCONNECTS, MAXCONNECTS);
			queue->queue = urls;
			while (*queue->current
			&& (size_t)(queue->current - queue->queue) < MAXCONNECTS)
				url_queue_add(queue, *queue->current++);
		}
		else
		{
			perror("Error initializing curl");
			free(queue);
			queue = NULL;
		}
		free(template);
	}
	return (queue);
}

int			url_queue_fetch(t_url_queue *queue, char *url)
{
	CURLMsg	*msg;
	int		msgs_left = -1;
	int		still_alive = 1;

	do {
		curl_multi_perform(queue->current, &still_alive);

		while((msg = curl_multi_info_read(queue->current, &msgs_left)))
		{
			if(msg->msg == CURLMSG_DONE)
			{
				char *url;
				CURL *e = msg->easy_handle;
				curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
				fprintf(stderr, "R: %d - %s <%s>\n",
						msg->data.result, curl_easy_strerror(msg->data.result), url);
				curl_multi_remove_handle(queue->curl_multi, e);
				curl_easy_cleanup(e);
			}
			else {
				fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
			}
			if(*queue->current)
				url_queue_add(queue, *queue->current++);
		}
		if(still_alive)
			curl_multi_wait(queue->curl_multi, NULL, 0, 1000, NULL);
	} while(still_alive || *queue->current);
	return (NULL);
}

void		url_queue_cleanup(t_url_queue **queue)
{
	if (curl_multi_cleanup((*queue)->curl_multi))
		perror("Error during curl multi cleanup");
	if (rmdir((*queue)->cache) == -1)
		perror("Error while deleting cache");
	free(*queue);
	*queue = NULL;
}
