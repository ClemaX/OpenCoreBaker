#include <baker.h>

int main(int ac, char **av)
{
	if (ac == 3)
	{
		t_recipe	*recipe = recipe_load(av[1]);
		int			ret = 1;

		if (recipe)
		{
			ret = recipe_bake(recipe, av[2]);
			recipe_free(&recipe);
		}
		return (ret);
	}
	printf("Usage:	%s <recipe-plist> <destination-dir>\n", av[0]);
    return (1);
}
