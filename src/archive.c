#include <archive.h>

#include <logger.h>

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

int		copy_cb(zip_file_t *file, zip_stat_t *sb, const char *relative_path, const void *cb_data)
{
	t_ar_status	status = AR_SUCCESS;
	char		*dest_path = NULL;
	char		*dest_dir = NULL;
	FILE		*dest_file = NULL;

	if (is_folder(sb->name))
		goto done;

	if (asprintf(&dest_path, "%s/%s", (const char *)cb_data, relative_path) == -1)
	{
		status = AR_FAIL_ALLOC;
		perror("asprintf");
		goto failure_malloc_dest_path;
	}

	if (!(dest_dir = adirname(dest_path)))
	{
		status = AR_FAIL_ALLOC;
		perror("adirname");
		goto failure_malloc_dest_dir;
	}

	if (mkdir_p(dest_dir, AR_DEST_MODE))
	{
		status = AR_FAIL_OPEN_DEST;
		perror("mkdir");
		goto failure_open_dest;
	}

	if (!(dest_file = fopen(dest_path, "wb")))
	{
		status = AR_FAIL_OPEN_DEST;
		perror(dest_path);
		goto failure_open_dest;
	}

	debug("Copying '%s' to '%s'...\n", sb->name, dest_path);
	if (!zcopy(file, dest_file))
	{
		perror(dest_path);
		status = AR_FAIL_WRITE_DEST;
	}

	fclose(dest_file);

	failure_open_dest:
	free(dest_dir);

	failure_malloc_dest_dir:
	free(dest_path);

	failure_malloc_dest_path:
	done:
	return (status);
}

int		zip_ftw(zip_t *archive, const char *prefix, zip_ftw_cb cb, const void *cb_data)
{
	size_t		prefix_length = strlen(prefix);
	zip_int64_t	entries_count = zip_get_num_entries(archive, 0);
	t_ar_status	status = AR_SUCCESS;
	char		*dirname;
	size_t		dirname_length;
	zip_stat_t	sb;

	for (zip_uint64_t i = 0; i < (zip_uint64_t)entries_count; i++)
	{
		zip_file_t	*file = NULL;
		zip_stat_init(&sb);

		if (zip_stat_index(archive, i, ZIP_FL_MODE, &sb))
		{
			error("%s: %s!\n", prefix, zip_strerror(archive));
			status = AR_FAIL_LOCATE;
			goto done;
		}
		if (!strncmp(prefix, sb.name, prefix_length))
		{
			debug("Matched %s!\n", prefix);
			if (!(file = zip_fopen_index(archive, i, ZIP_FL_MODE)))
			{
				error("%s: %s!\n", prefix, zip_strerror(archive));
				status = AR_FAIL_OPEN_FILE;
				goto done;
			}

			if (!(dirname = adirname(prefix)))
			{
				status = AR_FAIL_ALLOC;
				goto failure_malloc_dirname;
			}

			dirname_length = strlen(dirname);
			if (dirname_length == 1 && *dirname == '.')
				dirname_length = 0;

			free(dirname);
			status = cb(file, &sb, sb.name + dirname_length, cb_data);

			failure_malloc_dirname:
			zip_fclose(file);
		}
	}
	done:
	return (status);
}

int		zip_load(const char **content,
	const char *archive_path, const char *file_path, size_t *size)
{
	int			status = AR_SUCCESS;
	int			error = 0;
	zip_t		*archive = zip_open(archive_path, ZIP_MODE, &error);
	zip_file_t	*file = NULL;
	zip_stat_t	st;

	if (!archive)
		goto failure_open_archive;
	zip_stat_init(&st);

	if (zip_stat(archive, file_path, ZIP_ST_MODE, &st))
		goto failure_locate_file;

	if (!(content =  malloc(sizeof(*content) * st.size)))
		goto failure_malloc_content;

	*size = st.size;
	if (!(file = zip_fopen(archive, file_path, ZIP_FL_MODE)))
		goto failure_open_file;
	if (zip_fread(file, content, st.size) != (zip_int64_t)st.size)
		goto failure_read_file;

	failure_read_file:
	zip_fclose(file);

	failure_open_file:
	free(content);

	failure_malloc_content:

	failure_locate_file:
	zip_close(archive);

	failure_open_archive:
	return (status);
}

int			archive_copy_file(zip_t *archive, const char *file_path,
	const char *dest_path)
{
	int			status = AR_SUCCESS;
	zip_int64_t	index;
	zip_file_t	*file;
	FILE		*dest;

	debug("Copying '%s' to '%s'!\n", file_path, dest_path);
	if ((index = zip_name_locate(archive, file_path, ZIP_ST_MODE)) == -1)
	{
		status = AR_FAIL_LOCATE;
		goto failure_locate_file;
	}
	if (!(file = zip_fopen_index(archive, index, ZIP_FL_MODE)))
	{
		status = AR_FAIL_OPEN_FILE;
		goto failure_open_file;
	}
	if (!(dest = fopen(dest_path, "wb")))
	{
		status = AR_FAIL_OPEN_DEST;
		goto failure_open_dest;
	}

	if (!zcopy(file, dest))
		status = AR_FAIL_WRITE_DEST;

	fclose(dest);

	failure_open_dest:
	zip_fclose(file);

	failure_open_file:

	failure_locate_file:
	return (status);
}

int			archive_extract_zip(const char *archive_path, const char *file_path,
	const char *dest_path, t_ar_opt opt)
{
	int		error = 0;
	zip_t	*archive = zip_open(archive_path, ZIP_MODE, &error);

	debug("Extracting '%s/%s'...\n", archive_path, file_path);
	if (!archive)
	{
		if (error == ZIP_ER_NOZIP)
			error("%s: Not a Zip file!\n", archive_path);
		else
			error("%s: Unexpected libzip error %d!\n", archive_path, error);
		return (AR_FAIL_OPEN);
	}

	if (opt & AR_RECURSIVE)
	{
		zip_ftw(archive, file_path, &copy_cb, dest_path);
		return (AR_SUCCESS);
	}

	return (archive_copy_file(archive, file_path, dest_path));
}

int			archive_extract(const char *archive_path, const char *file_path,
	const char *dest_path, int options)
{
	const char	*extension = strrchr(archive_path, '.');

	if (extension && is_zip(extension))
	{
		return archive_extract_zip(archive_path, file_path, dest_path,
			options);
	}
	return (0);
}

const char	*archive_strerror(t_ar_status error)
{
	switch(error)
	{
		case AR_SUCCESS: return ("Success");
		case AR_FAIL_ALLOC: return (strerror(errno));
		case AR_FAIL_OPEN: return ("Could not open archive");
		case AR_FAIL_LOCATE: return ("Could not locate archived file");
		case AR_FAIL_OPEN_FILE: return ("Could not open archived file");
		case AR_FAIL_OPEN_DEST: return ("Could not open destination file");
		case AR_FAIL_WRITE_DEST: return ("Could not write to destination file");
	}
}
