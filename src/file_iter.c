#include <stdlib.h>
#include <dirent.h>

#include <libft/paths.h>

#include <file_iter.h>

int	file_iter(const char *filepath, t_file_iter_fun *fun, void *fun_data,
	unsigned char filter)
{
	char			full_path[PATH_MAX];
	struct stat 	st;
	DIR				*dir;
	struct dirent	*ent;
	char			*basename;
	int				err;

	err = stat(filepath, &st);
	if (err == 0)
	{
		if (S_ISDIR(st.st_mode))
		{
			dir = opendir(filepath);
			err = dir == NULL;
			if (err == 0)
			{
				do
				{
					ent = readdir(dir);
					if (ent != NULL
					&& (filter == DT_UNKNOWN || filter == ent->d_type))
					{
						err = path_cat(full_path, filepath, ent->d_name) == NULL;
						if (err == 0)
						{
							err = stat(full_path, &st);
							if (err == 0)
								err = fun(full_path, ent->d_name, &st, fun_data);
						}
						// TODO: Handle MAX_PATH error
					}
				}
				while (err == 0 && ent != NULL);
				closedir(dir);
			}
		}
		else
		{
			basename = ft_basename(filepath);
			err = basename == NULL;
			if (err == 0)
			{
				err = fun(filepath, basename, &st, fun_data);
				free(basename);
			}
		}
	}
	// TODO: Handle access errors
	return (err);
}
