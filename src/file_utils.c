#include <file_utils.h>

static char	buffer[FILE_BUFFER_SIZE];

int	fcopy(FILE *source, FILE *destination)
{
	size_t	bytes_read;

	while ((bytes_read = fread(buffer, sizeof(*buffer), FILE_BUFFER_SIZE, source)) > 0)
		fwrite(buffer, sizeof(*buffer), bytes_read, destination);
	return (bytes_read == 0);
}

int	zcopy(zip_file_t *source, FILE *destination)
{
	size_t	bytes_read;

	while ((bytes_read = zip_fread(source, buffer, FILE_BUFFER_SIZE)) > 0)
		fwrite(buffer, sizeof(*buffer), bytes_read, destination);
	return (bytes_read == 0);
}

char	*adirname(const char *str)
{
	size_t	length = strlen(str);
	size_t	end = (length) ? length - 1 : 0;

	while (end > 0 && str[end] == '/')
		end--;
	while (end > 0 && str[end - 1] != '/')
		end--;
	while (end > 0 && str[end - 1] == '/')
		end--;
	if (end == 0)
	{
		if (str[end] == '/')
			end++;
		else
			return (strndup(".", 1));
	}
	return (strndup(str, end));
}

char	*abasename(const char *str)
{
	size_t	length = strlen(str);
	size_t	start = length ? length - 1 : 0;
	size_t	end = start;

	while (start > 0 && str[start] == '/')
		start--;

	end = start;

	while (start > 0 && str[start - 1] != '/')
		start--;

	return (strndup(str + start, end - start + 1));
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
