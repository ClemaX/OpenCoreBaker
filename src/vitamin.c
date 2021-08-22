#include <vitamin.h>

#include <file_utils.h>
#include <logger.h>

t_vitamin		**vitamins_load(plist_t vitamins_array, t_vitamin_t type)
{
	const size_t	vitamins_length = plist_array_get_size(vitamins_array);
	t_vitamin		**vitamins = malloc(sizeof(*vitamins) * (vitamins_length + 1));
	t_vitamin		**current = vitamins;

	if (vitamins)
	{
		plist_array_iter	iterator;
		plist_t				vitamin_entry;

		plist_array_new_iter(vitamins_array, &iterator);
		do
			plist_array_next_item(vitamins_array, iterator, &vitamin_entry);
		while ((*current++ = vitamin_load(vitamin_entry, type)));

		free(iterator);

		if ((size_t)(current - vitamins) != vitamins_length + 1)
		{
			error("Error: Could not load vitamins!\n");
			current = vitamins;
			while ((*current))
				free(*current++);
			free(vitamins);
			vitamins = NULL;
		}
	}
	return (vitamins);
}

t_vitamin		*vitamin_load(plist_t vitamin_dict, t_vitamin_t type)
{
	t_vitamin	*vitamin = NULL;

	if (vitamin_dict && (vitamin = malloc(sizeof(*vitamin))))
	{
		plist_t	name_node = plist_dict_get_item(vitamin_dict, "Name");
		plist_t	path_node = plist_dict_get_item(vitamin_dict, "Path");
		plist_t	release_url_node = plist_dict_get_item(vitamin_dict, "ReleaseUrl");

		plist_get_string_val(name_node, &vitamin->name);
		plist_get_string_val(path_node, &vitamin->path);
		plist_get_string_val(release_url_node, &vitamin->release_url);

		if (url_is_http(vitamin->release_url))
			type |= VIT_HTTP;
		else
			type |= VIT_LOCAL;
		vitamin->type = type;
	}
	return (vitamin);
}

size_t		vitamins_size(t_vitamin **vitamins)
{
	size_t	size = 0;

	while (*vitamins)
	{
		if ((*vitamins)->type & VIT_HTTP)
			size++;
		vitamins++;
	}
	return (size);
}

char		**vitamins_urls(t_vitamin **vitamins, char **urls)
{
	while (*vitamins)
	{
		if ((*vitamins)->type & VIT_HTTP)
			urls_append(urls, (*vitamins)->release_url);
		vitamins++;
	}
	return (urls);
}

void		vitamin_free(t_vitamin **vitamin)
{
	free((*vitamin)->name);
	free((*vitamin)->release_url);
	free((*vitamin)->path);
	free(*vitamin);
	*vitamin = NULL;
}

void		vitamins_free(t_vitamin ***vitamins)
{
	t_vitamin	**current = *vitamins;

	if (current)
	{
		while (*current)
			vitamin_free(current++);
		free(*vitamins);
		*vitamins = NULL;
	}
}

char	*vitamin_location(t_vitamin *vitamin, const char *work_dir, const char *cache)
{
	char	*file_name;
	char	*location;

	if (vitamin->type & VIT_HTTP)
	{
		file_name = abasename(vitamin->release_url);
		if (file_name)
		{
			asprintf(&location, "%s/%s", cache, file_name);
			free(file_name);
		}
		else
			location = NULL;
	}
	else
		asprintf(&location, "%s/%s/%s", work_dir, vitamin->release_url, vitamin->path);

	return (location);
}

int	vitamin_install(t_vitamin *vitamin, const char *work_dir, const char *cache, const char *dest)
{
	t_ar_status status = AR_SUCCESS;

	if (cache)
	{
		char	*file_location = NULL;

		if (!(file_location = vitamin_location(vitamin, work_dir, cache)))
		{
			perror("malloc");
			goto failure_malloc_file_location;
		}

		if (is_archive(vitamin->release_url))
		{
			t_ar_opt	options =
				(vitamin->type & VIT_DIRECTORY) ? AR_RECURSIVE : 0;

			if ((status = archive_extract(file_location, vitamin->path, dest,
				options)))
			{
				error("%s: %s!\n", file_location, archive_strerror(status));
				goto failure_extract_archive;
			}

			failure_extract_archive:
			free(file_location);

			failure_malloc_file_location:
			;
		}
		else
		{
			char	*dest_path;

			if (asprintf(&dest_path, "%s/%s", dest, basename(file_location)) != -1)
			{
				if (mkdir_p(dest, AR_DEST_MODE))
					status = AR_FAIL_OPEN_DEST;
				else
					status = copy(file_location, dest_path);
				free(dest_path);
			}
		}
		return (status);
	}
	error("Invalid cache: '%s'!\n", cache);
	return (AR_FAIL_OPEN_FILE);
}

int	vitamins_install(t_vitamin **vitamins, const char *work_dir, const char *cache,
	const char *dest, const char *sub_dir)
{
	if (vitamins)
	{
		if (sub_dir && *sub_dir && !(sub_dir[0] == '.' && sub_dir[1] == '\0'))
		{
			char *dest_path;

			if (asprintf(&dest_path, "%s/%s", dest, sub_dir) == -1)
				return (0);

			while (*vitamins)
				vitamin_install(*vitamins++, work_dir, cache, dest_path);

			free(dest_path);
		}

		while (*vitamins)
			vitamin_install(*vitamins++, work_dir, cache, dest);
	}
	return (1);
}

int	vitamin_print(t_vitamin *vitamin)
{
	printf("%s\n	%s\n	%s\n",
		vitamin->name, vitamin->release_url, vitamin->path);
	return (1);
}

int	vitamins_print(t_vitamin **vitamins)
{
	if (vitamins)
	{
		while (*vitamins)
		{
			printf("  - ");
			vitamin_print(*vitamins++);
		}
	}
	return (1);
}
