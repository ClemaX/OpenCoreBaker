#include <config.h>

int	config_print(plist_t config_dict)
{
	uint32_t config_size = plist_dict_get_size(config_dict);
	printf("  - Size: %u\n", config_size);
	return (0);
}
