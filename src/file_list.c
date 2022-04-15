#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <libft/lists.h>
#include <libft/memory.h>
#include <libft/printf.h>
#include <libft/strings.h>

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
		ft_memcpy(file->name, name, name_size);
		file->mode = st->st_mode;
		file->nlink = st->st_nlink;
		file->uid = st->st_uid;
		file->gid = st->st_gid;
		file->size = st->st_size;
		file->blocks = st->st_blocks;
		file->atime = st->st_atim.tv_sec;
		file->mtime = st->st_mtim.tv_sec;
		file->ctime = st->st_ctim.tv_sec;
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

int		file_cmp_type(t_file *a, t_file *b)
{
	if (S_ISDIR(a->mode) && !S_ISDIR(b->mode))
		return (-1);
	if (S_ISDIR(b->mode) && !S_ISDIR(a->mode))
		return (1);
	return (0);
}

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

int		file_cmp_name(t_file *a, t_file *b)
{
	const char	*basename_a = dir_basename(a->name);
	const char	*basename_b = dir_basename(b->name);

	return (ft_strcmpi(basename_b, basename_a));
}

int		file_list(t_list **list, const char *filepath, t_ls_opt options)
{
	t_file_list_data	data;
	int					err;

	data = (t_file_list_data){list, options};
	err = file_iter(filepath, (t_file_iter_fun*)file_load, &data);

	if (err == 0)
	{
		if ((options & LS_OREVERSE) == 0)
			ft_lstsort(list, (t_cmp_fun*)file_cmp_name);
		else
			ft_lstsortrev(list, (t_cmp_fun*)file_cmp_name);
	}

	return (err);
}

void	file_list_print(t_list *list, t_ls_opt options, const char *parent)
{
	t_list			*curr;
	t_file			*file;
	t_field_widths	fw = {};

	if (options & LS_ORECURSE && parent != NULL)
		ft_printf("\n%s:\n", parent);

	curr = list;
	while (curr != NULL)
	{
		file = curr->content;
		file_print(file, options, fw);
		curr = curr->next;
	}

	if (options & LS_ORECURSE)
	{
		curr = list;
		while (curr != NULL)
		{
			file = curr->content;
			if (file->children != NULL)
				file_list_print(file->children, options, file->name);
			curr = curr->next;
		}
	}
}

void	file_list_clear(t_list **list)
{
	ft_lstclear(list, (t_lstcontent_fun*)file_del);
}
