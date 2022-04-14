#include <stddef.h>
#include <stdlib.h>

#include <libft.h>

#include <file_iter.h>
#include <file_list.h>

#include <file_print.h>

t_list	*file_new(const char *name, const struct stat *st)
{
	t_list	*elem;
	t_file	*file;
	size_t	name_size;

	name_size = ft_strlen(name) + 1;
	elem = malloc(sizeof(*elem) + sizeof(*file) + name_size);
	if (elem != NULL)
	{
		file = (t_file*)(elem + 1);
		elem->next = NULL;
		elem->content = file;
		file->children = NULL;
		file->name = (char *)(file + 1);
		ft_strlcpy(file->name, name, name_size);
		file->mode = st->st_mode;
		file->uid = st->st_uid;
		file->gid = st->st_gid;
		file->size = st->st_size;
		file->blocks = st->st_blocks;
		// TODO: Check if we need to handle block size
	}
	return (elem);
}

void	file_del(t_file *file)
{
	if (file->children != NULL)
		file_list_clear(&file->children);
}

int		file_load(const char *filepath, const char *basename,
	const struct stat *st, t_file_list_data *data)
{
	t_list	*elem;
	int		err;
	int		is_forwardref;

	if (data->options & LS_OALL || *basename != '.')
	{
		is_forwardref = S_ISDIR(st->st_mode) && !FILE_ISBACKREF(basename);
		elem = file_new(is_forwardref ? filepath : basename, st);
		err = elem == NULL;
		if (err == 0)
		{
			if (is_forwardref && data->options & LS_ORECURSE)
				err = file_list(&((t_file *)elem->content)->children, filepath, data->options);
			ft_lstadd_back(data->files, elem);
		}
	}
	else
		err = 0;
	return (err);
}

int		file_list(t_list **list, const char *filepath, t_ls_opt options)
{
	t_file_list_data	data;

	data = (t_file_list_data){list, options};
	return (file_iter(filepath, (t_file_iter_fun*)file_load, &data));
}

void	file_list_print(t_list *list, t_ls_opt options)
{
	t_file	*file;

	while (list != NULL)
	{
		file = list->content;
		if (file->children == NULL)
			file_print(file, options);
		else
		{
			ft_printf("\n%s:\n", file->name);
			file_list_print(file->children, options);
		}
		list = list->next;
	}
}

void	file_list_clear(t_list **list)
{
	ft_lstclear(list, (t_lstcontent_fun*)file_del);
}
