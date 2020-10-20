#include <url_queue.h>

static FILE	*url_fopen_dest(char* dest, char *url)
{
	const char		*filename = basename(url);
	char			*filepath = NULL;

	asprintf(&filepath, "%s/%s", dest, filename);
	FILE			*file = (filepath) ? fopen(filepath, "w") : NULL;

	if (!file)
		printf("Error: %s: %s\n", filepath, strerror(errno));
	free(filepath);
	return (file);
}

int			url_queue_add(t_url_queue *queue, char *url)
{
	FILE	*file = url_fopen_dest(queue->cache, url);

	if (file)
	{
		CURL	*eh = curl_easy_init();

		if (eh)
		{
			curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, NULL);
			curl_easy_setopt(eh, CURLOPT_WRITEDATA, file);
			curl_easy_setopt(eh, CURLOPT_URL, url);
			curl_easy_setopt(eh, CURLOPT_PRIVATE, url);

			if (!curl_multi_add_handle(queue->curl_multi, eh))
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
		char *template = NULL;
		
		asprintf(&template, "%s/%s", cache_dest, CACHE_TEMPLATE);

		if (template && (queue->cache = mkdtemp(template)))
		{
			if ((queue->curl_multi = curl_multi_init()))
			{
				printf("Caching files to '%s'!\n", queue->cache);
				curl_multi_setopt(queue->curl_multi, CURLMOPT_MAXCONNECTS, MAXCONNECTS);
				queue->queue = urls;
				queue->current = queue->queue;
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
		}
		else
			perror("Error creating cache");
	}
	return (queue);
}

int			url_queue_fetch(t_url_queue *queue)
{
	CURLMsg	*msg;
	int		msgs_left = -1;
	int		still_alive = 1;

	do {
		curl_multi_perform(queue->curl_multi, &still_alive);

		while((msg = curl_multi_info_read(queue->curl_multi, &msgs_left)))
		{
			if(msg->msg == CURLMSG_DONE)
			{
				char *url;
				CURL *e = msg->easy_handle;
				curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
				fprintf(stderr, "R: %d - %s <%s>\n",
						msg->data.result, curl_easy_strerror(msg->data.result), basename(url));
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
	return (0);
}

int			unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	(void) sb;
	(void) typeflag;
	(void) ftwbuf;
    int	ret = remove(fpath);

    if (ret)
		perror(fpath);

    return (ret);
}

int			rmrf(char *path)
{
	return (nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS));
}

void		url_queue_cleanup(t_url_queue **queue)
{
	if (curl_multi_cleanup((*queue)->curl_multi))
		perror("Error during curl multi cleanup");
	if (rmrf((*queue)->cache) == -1)
		printf("Error deleting cache '%s': %s\n", (*queue)->cache, strerror(errno));
	free((*queue)->cache);
	free(*queue);
	*queue = NULL;
}
