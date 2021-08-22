#pragma once

#include <plist/plist.h>

#include <vitamin.h>

#define OC_DIST					"RELEASE"
#define OC_REPO					"https://github.com/acidanthera/OpenCorePkg"

#define OC_PATH					"EFI/OC"
#define OC_CONFIG_PATH			OC_PATH"/config.plist"
#define OC_DRIVERS_PATH			OC_PATH"/Drivers"
#define OC_KEXTS_PATH			OC_PATH"/Kexts"
#define OC_SSDTS_PATH			OC_PATH"/SSDTS"

/// version, distribution
#define OC_FILENAME_TEMPLATE	"OpenCore-%s-%s.zip"
/// version, version, distribution
#define OC_URL_TEMPLATE			OC_REPO"/releases/download/%s/"OC_FILENAME_TEMPLATE
/// architecture
#define OC_EFI_PATH_TEMPLATE	"%s/EFI"

#define oc_url(dest, version, dist) asprintf(dest, OC_URL_TEMPLATE, version, version, dist)
#define oc_filename(dest, version, dist) asprintf(dest, OC_FILENAME_TEMPLATE, version, dist)
#define oc_efi_path(dest, architecture) asprintf(dest, OC_EFI_PATH_TEMPLATE, architecture)

typedef struct	s_oc
{
	char		*version;
	char		*distribution;
	char		*architecture;
	t_vitamin	vitamin;
	plist_t		config;
}				t_oc;

void	oc_load(t_oc *oc, plist_t oc_dict, plist_t config_dict);
void	oc_free(t_oc *oc);

void	oc_print(t_oc *oc);
int		oc_install(t_oc *oc, const char *cache, const char *destination);
