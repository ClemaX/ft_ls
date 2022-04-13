#include <stdlib.h>
#include <unistd.h>

#include <libft.h>

#include <file_list.h>
#include <file_iter.h>
#include <file_print.h>

int	file_print(const char *filepath, const char *basename,
	const struct stat *st, void *data)
{
	const t_ls_opt	options = *(t_ls_opt*)data;
	int				err;

	err = 0;
	if (options & LS_OALL || *basename != '.')
	{
		if (options & LS_ORECURSE && S_ISDIR(st->st_mode))
		{
			ft_printf("\n%s:\n", filepath);
			err = file_iter(filepath, file_print, data);
		}
		else
			ft_printf("%-24s\n", basename);
	}
	return (err);
}
