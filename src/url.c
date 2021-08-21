# include <url.h>

void		urls_free(char ***urls)
{
	free(*urls);
	*urls = NULL;
}

int			url_is_http(char *url)
{
	return (!strncmp(url, "http://", 7) || !strncmp(url, "https://", 8));
}

// TODO: Quick Sort, Remove Dupplicates and Swap to the Left
void		urls_append(char **urls, char *url)
{
	while (*urls && (strcmp(*urls, url)))
		urls++;
	if (!*urls)
		*urls = url;
}

int			urls_print(char **urls)
{
	char	**current = urls;
	while (*current)
	{
		printf("  - %s\n", *current);
		current++;
	}
	return (0);
}
