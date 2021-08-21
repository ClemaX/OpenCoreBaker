#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <url_queue.h>

#include <logger.h>
#include <file_utils.h>

static FILE	*url_fopen_dest(char* dest, char *url)
{
	char			*filename = abasename(url);
	char			*filepath = NULL;
	FILE			*file = NULL;

	if (filename)
	{
		asprintf(&filepath, "%s/%s", dest, filename);
		free(filename);

		if (filepath)
		{
			file = fopen(filepath, "w");
			free(filepath);
		}
	}

	if (!file)
		error("%s: %s\n", filepath, strerror(errno));
	return (file);
}

t_url_dl	*url_dl_init(char* dest, char *url)
{
	t_url_dl	*dl = NULL;
	CURLcode	code;

	if (!(dl = malloc(sizeof(*dl))))
	{
		perror("Error");
		goto failure_malloc_dl;
	}
	dl->url = url;
	if (!(dl->file = url_fopen_dest(dest, url)))
	{
		error("%s: %s\n", dest, strerror(errno));
		goto failure_open_file;
	}

	if (!(dl->handle = curl_easy_init()))
	{
		perror("Error creating curl handle");
		goto failure_init_handle;
	}

	if ((code = curl_easy_setopt(dl->handle, CURLOPT_WRITEFUNCTION, NULL))
	|| (code = curl_easy_setopt(dl->handle, CURLOPT_WRITEDATA, dl->file))
	|| (code = curl_easy_setopt(dl->handle, CURLOPT_URL, url))
	|| (code = curl_easy_setopt(dl->handle, CURLOPT_PRIVATE, dl))
	|| (code = curl_easy_setopt(dl->handle, CURLOPT_FOLLOWLOCATION, 1)))
	{
		error("Error setting curl handle options: %s\n", curl_easy_strerror(code));
		goto failure_setopt_handle;
	}
	goto success;

	failure_setopt_handle:
	curl_easy_cleanup(dl->handle);

	failure_init_handle:
	fclose(dl->file);

	failure_open_file:
	free(dl);
	dl = NULL;

	failure_malloc_dl:

	success:
	return (dl);
}

void		url_dl_cleanup(t_url_dl **dl)
{
	t_url_dl	*content = *dl;

	if (!fclose(content->file))
		perror("Error closing downloaded file");
	curl_easy_cleanup(content->handle);
	free(content);
	*dl = NULL;
}
