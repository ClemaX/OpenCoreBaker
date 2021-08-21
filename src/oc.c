#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <oc.h>

#include <logger.h>

void	oc_load(t_oc *oc, plist_t oc_dict)
{
	plist_t	version_node = plist_dict_get_item(oc_dict, "Version");
	plist_t	distribution_node = plist_dict_get_item(oc_dict, "Distribution");
	plist_t	architecture_node = plist_dict_get_item(oc_dict, "Architecture");

	plist_get_string_val(version_node, &oc->version);
	plist_get_string_val(distribution_node, &oc->distribution);
	plist_get_string_val(architecture_node, &oc->architecture);

	oc->vitamin.name = "OpenCore";
	oc->vitamin.type = VIT_DIRECTORY;
	oc_efi_path(&oc->vitamin.path, oc->architecture);
	oc_url(&oc->vitamin.release_url, oc->version, oc->distribution);
	// TODO: Validation
}

void	oc_free(t_oc *oc)
{
	free(oc->version);
	free(oc->distribution);
	free(oc->architecture);
	free(oc->vitamin.path);
	free(oc->vitamin.release_url);

	bzero(oc, sizeof(*oc));
}

void	oc_print(t_oc *oc)
{
	printf("OpenCore:	'%s-%s/%s'\n", oc->version, oc->distribution, oc->architecture);
}

int		oc_install(t_oc *oc, const char *cache, const char *destination)
{
	return vitamin_install(&oc->vitamin, cache, destination);
}
