#include <stdlib.h>
#include <errno.h>

#include <recipe.h>
#include <url_queue.h>

#include <logger.h>
#include <file_utils.h>

static plist_t	plist_load(const char *filepath)
{
	FILE		*file;
	struct stat	file_stat;

	if ((file = fopen(filepath, "r")))
	{
		size_t	file_size;
		char	*plist_xml;
		plist_t	plist_root = NULL;

		stat(filepath, &file_stat);
		file_size = file_stat.st_size;

		if ((plist_xml = malloc(sizeof(*plist_xml) * (file_size + 1))))
		{
			debug("Loading '%s' (%zu bytes)...\n", filepath, file_size);
			fread(plist_xml, sizeof(*plist_xml), file_size, file);
			plist_from_xml(plist_xml, file_size, &plist_root);
		}
		fclose(file);
		return (plist_root);
    }
	perror("Error");
	return (NULL);
}

t_recipe		*recipe_load(const char *filepath)
{
	plist_t		recipe_root;
	t_recipe	*recipe = NULL;

	if ((recipe_root = plist_load(filepath)))
	{
		if ((recipe = malloc(sizeof(*recipe))))
		{
			plist_t name_node = RECIPE_GET(recipe_root, "Name");
			plist_t oc_node = RECIPE_GET(recipe_root, "OpenCore");
			plist_t	drivers_node = RECIPE_GET(recipe_root, "Drivers");
			plist_t	kexts_node = RECIPE_GET(recipe_root, "Kexts");
			plist_t	ssdts_node = RECIPE_GET(recipe_root, "SSDT");
			plist_t	config_node = RECIPE_GET(recipe_root, "Config");

			plist_get_string_val(name_node, &recipe->name);
			oc_load(&recipe->oc, oc_node, config_node);
			recipe->drivers = vitamins_load(drivers_node, VIT_DRIVER);
			recipe->kexts = vitamins_load(kexts_node, VIT_KEXT);
			recipe->ssdts = vitamins_load(ssdts_node, VIT_SSDT);
			recipe->urls = recipe_urls(recipe);
		}
		plist_free(recipe_root);
	}
	return (recipe);
}

void			recipe_free(t_recipe **recipe)
{
	if (recipe && *recipe)
	{
		t_recipe	*content = *recipe;

		*recipe = NULL;

		free(content->name);
		oc_free(&content->oc);
		vitamins_free(&content->drivers);
		vitamins_free(&content->kexts);
		vitamins_free(&content->ssdts);
		urls_free(&content->urls);
	}
}

size_t			recipe_size(t_recipe *recipe)
{
	return (
		vitamins_size(recipe->drivers)
		+ vitamins_size(recipe->kexts)
		+ vitamins_size(recipe->ssdts)
		+ 1
	);
}

char			**recipe_urls(t_recipe *recipe)
{
	char	**urls = calloc(recipe_size(recipe) + 1, sizeof(*urls));
	char	*oc_release_url = NULL;

	if (urls && oc_url(&oc_release_url, recipe->oc.version, recipe->oc.distribution) != -1)
	{
		vitamins_urls(recipe->drivers, urls);
		vitamins_urls(recipe->kexts, urls);
		vitamins_urls(recipe->ssdts, urls);
		urls_append(urls, oc_release_url);
	}
	else
	{
		free(urls);
		urls = NULL;
		perror("malloc");
	}
	return (urls);
}

int				recipe_bake(t_recipe *recipe, const char *destination)
{
	int			status = 0;
	struct stat	st;
	char		*enclosing_dir = adirname(destination);
	char		**urls = NULL;
	t_url_queue	*queue = NULL;

	if (!enclosing_dir)
	{
		perror("Error");
		status = 1;
		goto failure_malloc_dirname;
	}
	if (stat(enclosing_dir, &st) || !S_ISDIR(st.st_mode))
	{
		status = 1;
		goto failure_mkdir;
	}
	if (mkdir(destination, DEST_MODE) && errno != EEXIST)
	{
		perror(destination);
		status = 1;
		goto failure_mkdir;
	}
	if (!(urls = recipe_urls(recipe)))
	{
		status = 1;
		goto failure_malloc_urls;
	}
	if (!(queue = url_queue_init(CACHE_DIR, urls)))
	{
		status = 1;
		goto failure_init_queue;
	}

	info("Baking '%s' to '%s'...\n", recipe->name, destination);
	if ((status = url_queue_fetch(queue)))
		goto failure_fetch_queue;

	debug("\nInstall: \n");
	oc_install(&recipe->oc, queue->cache, destination);
	vitamins_install(recipe->kexts, queue->cache, destination, OC_KEXTS_PATH);
	vitamins_install(recipe->drivers, queue->cache, destination, OC_DRIVERS_PATH);

	failure_fetch_queue:
	url_queue_cleanup(queue);

	url_queue_free(&queue, 1);

	failure_init_queue:
	free(urls);

	failure_malloc_urls:

	failure_mkdir:
	free(enclosing_dir);

	failure_malloc_dirname:

	return (status);
}

int				recipe_print(t_recipe *recipe)
{
	if (recipe)
	{
		printf("\nName:		'%s'\n", recipe->name);
		oc_print(&recipe->oc);
		printf("\nDrivers:\n");
		vitamins_print(recipe->drivers);
		printf("\nKexts:\n");
		vitamins_print(recipe->kexts);
		printf("\nSSDTs:\n");
		vitamins_print(recipe->ssdts);
		printf("\nDownloads:\n");
		urls_print(recipe->urls);
		return (0);
	}
	return (1);
}
