#include <stdlib.h>
#include <unistd.h>

#include <libft.h>

#include <file_list.h>
#include <file_iter.h>
#include <file_print.h>

static const char	*dir_basename(const char *path)
{
	int			i;
	const char	*basename;

	// TODO: Check safety and error cases
	i = 0;
	basename = path;
	while (path[i] != '\0')
	{
		if (path[i] == '/' && path[i + 1])
			basename = (char *)(path + i + 1);
		i++;
	}
	return (basename);
}

int	file_print(t_file *file, t_ls_opt options)
{
	(void)options;
	const char	*basename;
	int			err;

	err = 0;
	if (S_ISDIR(file->mode))
		basename = dir_basename(file->name);
	else
		basename = file->name;
	
	ft_printf("%-24s\n", basename);
	return (err);
}
