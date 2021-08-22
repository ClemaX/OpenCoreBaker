#include <stdlib.h>
#include <stdio.h>

#include <config.h>

#include <logger.h>

int	config_print(plist_t config_dict)
{
	uint32_t config_size = plist_dict_get_size(config_dict);
	info("  - Size: %u\n", config_size);
	return (0);
}

int	config_write(char *filename, plist_t config_dict)
{
	char		*config_xml;
	uint32_t	config_length;
	FILE		*file = fopen(filename, "w");
	int			ret;

	ret = 1;
	if (file)
	{
		plist_to_xml(config_dict, &config_xml, &config_length);
		if (config_xml)
		{
			if (fwrite(config_xml, sizeof(*config_xml), config_length, file) == config_length)
			{
				debug("Config written to '%s'!\n", filename);
				ret = 0;
			}
			free(config_xml);
		}
		else
			error("Could not export config!\n");
		fclose(file);
	}
	return (ret);
}
