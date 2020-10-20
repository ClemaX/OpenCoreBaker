# include <vitamin.h>

int			url_is_http(char *url)
{
	return (!strncmp(url, "http://", 7) || !strncmp(url, "https://", 8));
}

void		urls_append(char **urls, char *url)
{
	while (*urls && strcmp(*urls, url))
		urls++;
	if (!*urls)
		*urls = url;
}
