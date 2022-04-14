#include <stdlib.h>
#include <unistd.h>

#include <libft.h>

#include <file_list.h>
#include <file_iter.h>
#include <file_print.h>

int	file_print(t_file *file, t_ls_opt options)
{
	(void) options;
	int	err;

	err = 0;
	ft_printf("%-24s\n", file->name);
	return (err);
}
