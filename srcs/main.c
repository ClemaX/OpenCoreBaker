#include <baker.h>

int main(int ac, char **av)
{
	if (ac == 3)
	{
		if (curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK)
		{
			t_recipe	*recipe = recipe_load(av[1]);
			int			ret = 1;

			if (recipe)
			{
				ret = recipe_bake(recipe, av[2]);
				recipe_free(&recipe);
			}
			curl_global_cleanup();
			return (ret);
		}
		perror("Error during curl init");
	}
	printf("Usage:	%s <recipe-plist> <destination-dir>\n", av[0]);
    return (1);
}
