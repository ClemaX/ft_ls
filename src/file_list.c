#include <grp.h>
#include <pwd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <libft/hmap_i.h>
#include <libft/lists.h>
#include <libft/memory.h>
#include <libft/printf.h>
#include <libft/strings.h>
#include <libft/numbers.h>

#include <file_iter.h>
#include <file_list.h>

#include <file_print.h>

t_list	*file_new(const char *path, const char *name, const struct stat *st)
{
	t_list	*elem;
	t_file	*file;
	size_t	name_size;
	size_t	path_size;

	name_size = ft_strlen(name) + 1;
	path_size = ft_strlen(path) + 1;
	elem = malloc(sizeof(*elem) + sizeof(*file) + name_size + path_size);
	if (elem != NULL)
	{
		file = (t_file*)(elem + 1);
		elem->next = NULL;
		elem->content = file;
		file->children = NULL;
		file->name = (char *)(file + 1);
		file->path = file->name + name_size;
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
		ft_memcpy(file->name, name, name_size);
		ft_memcpy(file->path, path, path_size);
	}
	return (elem);
}

static int	file_load_ids(t_hmap_i *users, t_hmap_i *groups, uid_t uid,
	gid_t gid)
{
	struct passwd	*user_ent;
	struct group	*group_ent;
	char			*name;
	int				err;

	err = 0;
	if (hmap_i_get(users, uid) == NULL)
	{
		user_ent = getpwuid(uid);
		if (user_ent != NULL)
			name = user_ent->pw_name;
		else
			name = ft_itoa(uid);

		err = name == NULL;
		if (err == 0)
		{
			err = hmap_i_set(users, uid, name) == NULL;
			ft_dprintf(2, "Setting uid: %u name: %s\n", uid, name);
		}
	}

	if (hmap_i_get(groups, gid) == NULL)
	{
		group_ent = getgrgid(gid);
		if (group_ent != NULL)
			name = group_ent->gr_name;
		else
			name = ft_itoa(gid);

		err = name == NULL;
		if (err == 0)
		{
			err = hmap_i_set(groups, gid, name) == NULL;
			ft_dprintf(2, "Setting gid: %u name: %s\n", gid, name);
		}
	}
	return (err);
}

int		file_load(const char *filepath, const char *basename,
	const struct stat *st, t_file_list *data)
{
	t_list		*elem;
	int			err;
	int			is_forwardref;

	if (data->options & LS_OALL || *basename != '.')
	{
		is_forwardref = S_ISDIR(st->st_mode) && !FILE_ISBACKREF(basename);
		elem = file_new(filepath, basename, st);
		err = elem == NULL;
		if (err == 0)
		{
			err = file_load_ids(data->users, data->groups, st->st_uid,
				st->st_gid);
			if (err == 0 && is_forwardref && data->options & LS_ORECURSE)
			{
				t_file_list	list = {
					.users = data->users,
					.groups = data->groups,
				};

				err = file_list(&list, filepath, data->options);

				if (err == 0)
					((t_file *)elem->content)->children = list.files;
			}
			if (err == 0)
				ft_lstadd_back(&data->files, elem);
			else
				free(elem);
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

int		file_list_init(t_file_list *list)
{
	int	err;

	ft_bzero(list, sizeof(*list));
	list->users = hmap_i_new(FILE_HMAP_UID_SIZE);
	err = list->users == NULL;
	if (err == 0)
	{
		list->groups = hmap_i_new(FILE_HMAP_GID_SIZE);
		err = list->groups == NULL;
	}
	else
		hmap_i_clr(&list->users, free);
	return (err);
}

int		file_list(t_file_list *list, const char *filepath, t_ls_opt options)
{
	int			err;

	list->parent = filepath;
	list->options = options;
	err = file_iter(filepath, (t_file_iter_fun*)file_load, list);
	if (err == 0)
	{
		if ((options & LS_OREVERSE) == 0)
			ft_lstsort(&list->files, (t_cmp_fun*)file_cmp_name);
		else
			ft_lstsortrev(&list->files, (t_cmp_fun*)file_cmp_name);
	}
	return (err);
}

void	file_list_print(t_file_list *list)
{
	t_field_widths	fw = {};
	t_list			*curr;
	t_file			*file;
	const char		*user;
	const char		*group;

	if (list->options & LS_ORECURSE && list->parent != NULL)
		ft_printf("\n%s:\n", list->parent);

	curr = list->files;
	while (curr != NULL)
	{
		file = curr->content;
		user = hmap_i_get(list->users, file->uid)->value;
		group = hmap_i_get(list->groups, file->gid)->value;
		file_print(file, user, group, fw, list->options);
		curr = curr->next;
	}

	if (list->options & LS_ORECURSE)
	{
		curr = list->files;
		while (curr != NULL)
		{
			file = curr->content;
			if (file->children != NULL)
			{
				t_file_list	children_list = {
					.files = file->children,
					.users = list->users,
					.groups = list->groups,
					.parent = file->path,
					.options = list->options,
				};
				file_list_print(&children_list);
			}
			curr = curr->next;
		}
	}
}

void	file_del(t_file *file)
{
	if (file->children)
		ft_lstclear(&file->children, (t_lstcontent_fun*)file_del);
}

void	file_list_clear(t_file_list *list)
{
	hmap_i_clr(&list->users, free);
	hmap_i_clr(&list->groups, free);
	ft_lstclear(&list->files, (t_lstcontent_fun*)file_del);
}
