#include <config.h>

int	config_print(plist_t config_dict)
{
	uint32_t config_size = plist_dict_get_size(config_dict);
	printf("  - Size: %u\n", config_size);
	return (0);
}

int	config_write(char *filename, plist_t config_dict)
{
	char		*config_xml;
	uint32_t	config_length;
	FILE		*file = fopen(filename, "wb");

	if (file)
	{
		plist_to_xml(config_dict, &config_xml, &config_length);
		if (config_xml)
		{
			if (fwrite(config_xml, sizeof(*config_xml), config_length, file) == config_length)
			{
				printf("Config written to '%s'\n", filename);
				return (1);
			}
		}
		printf("Could not export config!\n");
	}
	return (0);
}
