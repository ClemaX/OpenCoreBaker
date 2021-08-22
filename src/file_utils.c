#include <string.h>
#include <errno.h>
#include <ftw.h>

#include <file_utils.h>

#include <logger.h>

static char	buffer[FILE_BUFFER_SIZE];

int		fcopy(FILE *source, FILE *destination)
{
	size_t	bytes_read;

	while ((bytes_read = fread(buffer, sizeof(*buffer), FILE_BUFFER_SIZE, source)) > 0)
		fwrite(buffer, sizeof(*buffer), bytes_read, destination);
	return (bytes_read == 0);
}

int		copy(const char *src_path, const char *dest_path)
{
	FILE	*src_file;
	FILE	*dest_file;
	int		ret = 0;

	if (!(src_file = fopen(src_path, "r")))
		error("%s: %s\n", src_path, strerror(errno));
	else if (!(dest_file = fopen(dest_path, "w")))
	{
		error("%s: %s\n", dest_path, strerror(errno));
		fclose(dest_file);
	}
	else
	{
		ret = fcopy(src_file, dest_file);
		fclose(src_file);
		fclose(dest_file);
	}
	return ret;
}

int		zcopy(zip_file_t *source, FILE *destination)
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
		error("%s: %s\n", fpath, strerror(errno));

	return (ret);
}

int			rmrf(char *path)
{
	return (nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS));
}

int			mkdir_p(const char *path, mode_t mode)
{
	char			buff[PATH_MAX];
	const size_t	len = strlen(path);
	char			*end;

	if (len == 0)
		return 0;

	if (len > sizeof(buff) - 1)
		return -1;

	strcpy(buff, path);
/*
	if (buff[len - 1] == '/')
		buff[len - 1] = '\0'; */

	for (end = buff + 1; *end; end++)
	{
		if (*end == '/')
		{
			*end = '\0';
			if (mkdir(buff, mode) && errno != EEXIST)
				return -1;
			*end = '/';
		}
	}

	if (mkdir(buff, mode) && errno != EEXIST)
		return -1;

	return 0;
}
