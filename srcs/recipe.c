#include <recipe.h>

static plist_t	plist_load(const char *filepath)
{
	FILE		*file;
	struct stat	file_stat;

	if ((file = fopen(filepath, "rb")))
	{
		size_t	file_size;
		char	*plist_xml;
		plist_t	plist_root = NULL;

		stat(filepath, &file_stat);
		file_size = file_stat.st_size;

		if ((plist_xml = malloc(sizeof(*plist_xml) * (file_size + 1))))
		{
			printf("Loading '%s' (%zu bytes)...\n", filepath, file_size);
			fread(plist_xml, sizeof(*plist_xml), file_size, file);
			plist_from_xml(plist_xml, file_size, &plist_root);
		}
		fclose(file);
		return (plist_root);
    }
	perror("Error");
	return (NULL);
}

void			recipe_free(t_recipe **recipe)
{
	if (recipe && *recipe)
	{
		t_recipe	*content = *recipe;

		free(content->name);
		free(content->oc_version);
		vitamins_free(&content->drivers);
		vitamins_free(&content->kexts);
		vitamins_free(&content->ssdts);
		urls_free(&content->urls);
		plist_free(content->config);
		*recipe = NULL;
	}
}

t_recipe		*recipe_load(const char *filepath)
{
	plist_t		recipe_root;
	t_recipe	*recipe = NULL;

	if ((recipe_root = plist_load(filepath)))
	{
		if ((recipe = malloc(sizeof(*recipe))))
		{
			plist_t name_node = plist_dict_get_item(recipe_root, "Name");
			plist_t oc_version_node = plist_dict_get_item(recipe_root, "OCVersion");
			plist_t	drivers_node = plist_dict_get_item(recipe_root, "Drivers");
			plist_t	kexts_node = plist_dict_get_item(recipe_root, "Kexts");
			plist_t	ssdts_node = plist_dict_get_item(recipe_root, "SSDT");
			plist_t	config_node = plist_dict_get_item(recipe_root, "Config");

			plist_get_string_val(name_node, &recipe->name);
			plist_get_string_val(oc_version_node, &recipe->oc_version);
			recipe->drivers = vitamins_load(drivers_node);
			recipe->kexts = vitamins_load(kexts_node);
			recipe->ssdts = vitamins_load(ssdts_node);
			recipe->config = plist_copy(config_node);
			recipe->urls = recipe_urls(recipe);
		}
		plist_free(recipe_root);
	}
	return (recipe);
}

char			**recipe_urls(t_recipe *recipe)
{
	size_t	size = vitamins_size(recipe->drivers)
		+ vitamins_size(recipe->kexts)
		+ vitamins_size(recipe->ssdts);
	char	**urls = calloc(size + 1, sizeof(*urls));

	vitamins_urls(recipe->drivers, urls);
	vitamins_urls(recipe->kexts, urls);
	vitamins_urls(recipe->ssdts, urls);
	return (urls);
}

int				recipe_bake(t_recipe *recipe, char *destination)
{
	struct stat	st;
	char		*enclosing_dir = dirname(destination);

	if (stat(enclosing_dir, &st) == 0 && S_ISDIR(st.st_mode))
	{
		if (!mkdir(destination, DEST_MODE) || errno == EEXIST)
		{
			printf("Baking '%s' to '%s'...\n", recipe->name, destination);
			recipe_print(recipe);
			return (0);
		}
		printf("Error: %s: %s\n", destination, strerror(errno));
		return (1);
	}
	printf("'%s' is not a valid directory!\n", enclosing_dir);
	return (1);
}

int				recipe_print(t_recipe *recipe)
{
	if (recipe)
	{
		printf("\nName:		'%s'\n", recipe->name);
		printf("OCVersion:	'%s'\n", recipe->oc_version);
		printf("\nDrivers:\n");
		vitamins_print(recipe->drivers);
		printf("\nKexts:\n");
		vitamins_print(recipe->kexts);
		printf("\nSSDTs:\n");
		vitamins_print(recipe->ssdts);
		printf("\nConfig:\n");
		config_print(recipe->config);
		printf("\nDownloads:\n");
		urls_print(recipe->urls);
		return (1);
	}
	return (0);
}
