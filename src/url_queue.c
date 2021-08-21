#include <url_queue.h>

#include <logger.h>

// TODO: Fix meaningless return value
int			url_queue_add(t_url_queue *queue, char *url)
{
	int			status = 0;
	CURLMcode	mcode = CURLM_OK;
	t_url_dl	*dl = NULL;

	if (!(dl = url_dl_init(queue->cache, url)))
		goto failure_dl_init;
	if ((mcode = curl_multi_add_handle(queue->curl_multi, dl->handle)) != CURLM_OK)
	{
		error("Error adding curl handle: %s\n", curl_multi_strerror(mcode));
		goto failure_add_handle;
	}
	goto success;

	failure_add_handle:

	failure_dl_init:
	url_dl_cleanup(&dl);

	success:
	return (status);
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
				debug("Caching files to '%s'!\n", queue->cache);
				curl_multi_setopt(queue->curl_multi, CURLMOPT_MAXCONNECTS, MAXCONNECTS);
				queue->queue = urls;
				queue->current = queue->queue;
				if (urls)
				{
					while (*queue->current
					&& (size_t)(queue->current - queue->queue) < MAXCONNECTS)
						url_queue_add(queue, *queue->current++);
				}
			}
			else
			{
				error("Error initializing curl: %s\n", strerror(errno));
				free(queue);
				queue = NULL;
			}
		}
		else
			perror("Error creating cache");
	}
	return (queue);
}

static int	url_dl_handle_msg(CURLMsg *msg, char *url)
{
	char	*file_name = abasename(url);
	int		status = 0;

	if (msg->msg == CURLMSG_DONE)
	{
		if (msg->data.result == CURLE_OK)
		{
			debug("Downloaded '%s'!\n", file_name);
			goto done;
		}
		debug("Error: '%s': %s!",
			file_name, curl_easy_strerror(msg->data.result));
		status = 1;
		goto done;
	}
	error("%s: CURLMsg (%d)\n", url, msg->msg);
	status = 1;

	done:
	free(file_name);
	return (status);
}

int		url_dl_done(t_url_queue *queue, CURLMsg *msg)
{
	CURL		*eh = msg->easy_handle;
	t_url_dl	*dl = NULL;
	int			err = 0;

	curl_easy_getinfo(eh, CURLINFO_PRIVATE, &dl);

	err = url_dl_handle_msg(msg, dl->url);

	if (fclose(dl->file))
		perror("Error closing downloaded file");

	curl_multi_remove_handle(queue->curl_multi, eh);
	url_dl_cleanup(&dl);
	return (err);
}

int			url_queue_fetch(t_url_queue *queue)
{
	CURLMsg	*msg;
	int		msgs_left = -1;
	int		still_alive = 1;
	int		err = 0;

	do {
		curl_multi_perform(queue->curl_multi, &still_alive);

		while((msg = curl_multi_info_read(queue->curl_multi, &msgs_left)))
		{
			err += url_dl_done(queue, msg);

			if(*queue->current)
				url_queue_add(queue, *queue->current++);
		}
		if(still_alive)
			curl_multi_wait(queue->curl_multi, NULL, 0, 1000, NULL);
	} while(still_alive || *queue->current);
	return (err);
}

void		url_queue_free(t_url_queue **queue, char del_cache)
{
	if (del_cache && rmrf((*queue)->cache) == -1)
		error("Error deleting cache '%s': %s\n", (*queue)->cache, strerror(errno));
	free((*queue)->cache);
	free(*queue);
	*queue = NULL;

}

void		url_queue_cleanup(t_url_queue *queue)
{
	if (curl_multi_cleanup(queue->curl_multi))
		perror("Error during curl multi cleanup");
}
