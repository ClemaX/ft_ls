#include <file_iter.h>
#include <file_list.h>

#include <file_print.h>

int	file_list(t_list **list, const char *filepath, void *data)
{
	(void)list;
	int	err;

	err = file_iter(filepath, file_print, data);
	return (err);
}
