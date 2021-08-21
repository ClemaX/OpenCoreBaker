#include <file_utils.h>

#include <string.h>

int		is_folder(const char *url)
{
	const char *slash = strrchr(url, '/');
	return ((url[0] == '.' && url[1] == '\0') || (slash && (slash[1] == '\0' || (slash[1] == '.' && slash[2] == '\0'))));
}

int		is_zip(const char *extension)
{
	return (!strcmp(extension, ".zip"));
}

int		is_archive(const char *url)
{
	const char	*extension = strrchr(url, '.');

	return (extension && is_zip(extension));
}
