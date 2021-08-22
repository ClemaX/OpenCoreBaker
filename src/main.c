#include <unistd.h>

#include <baker.h>

#include <logger.h>

int main(int ac, char **av)
{
	if (ac == 3)
	{
		if (curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK)
		{
			t_recipe	*recipe;
			int			ret;

			recipe = recipe_load(av[1], "recipe.plist");
			if (recipe)
			{
				//ret = recipe_print(recipe);
				ret = recipe_bake(recipe, av[2]);
				recipe_free(&recipe);
			}
			else
				ret = 1;
			curl_global_cleanup();
			return (ret);
		}
		perror("Error during curl init");
	}
	error("Usage:	%s <recipe-dir> <destination-dir>\n", av[0]);
    return (1);
}
