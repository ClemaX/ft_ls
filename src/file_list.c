#include <stdlib.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>

#include <libft/memory.h>
#include <libft/printf.h>
#include <libft/strings.h>
#include <libft/numbers.h>

#include <file_iter.h>

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
		if (!err)
			err = hmap_i_set(users, uid, name) == NULL;
	}

	if (hmap_i_get(groups, gid) == NULL)
	{
		group_ent = getgrgid(gid);
		if (group_ent != NULL)
			name = group_ent->gr_name;
		else
			name = ft_itoa(gid);

		err = name == NULL;
		if (!err)
			err = hmap_i_set(groups, gid, name) == NULL;
	}
	return (err);
}

int		file_load(const char *filepath, const char *basename,
	const struct stat *st, t_file_list *data)
{
	t_list		*elem;
	int			err;
	int			is_forwardref;

	if ((data->options & LS_OALL) || *basename != '.')
	{
		is_forwardref = S_ISDIR(st->st_mode) && !FILE_ISBACKREF(basename);
		elem = file_new(filepath, basename, st);
		err = elem == NULL;
		if (!err)
		{
			err = file_load_ids(data->users, data->groups, st->st_uid,
				st->st_gid);
			if (!err && is_forwardref && (data->options & LS_ORECURSE))
			{
				t_file_list	list = {
					.users = data->users,
					.groups = data->groups,
				};

				err = file_list(&list, filepath);

				if (!err)
					((t_file *)elem->content)->children = list.files;
			}
			if (!err)
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

int			file_cmp_name(const char *basename_a, const char *basename_b)
{
#ifdef BONUS

#else
	return (ft_strcmpi(basename_b, basename_a));
#endif
}

int			file_cmp(t_file *a, t_file *b)
{
	const char	*basename_a = !S_ISDIR(a->mode)? a->name : dir_basename(a->name);
	const char	*basename_b = !S_ISDIR(a->mode) ? b->name : dir_basename(b->name);

	return (file_cmp_name(basename_a, basename_b));
}

int			file_list_init(t_file_list *list, t_ls_opt options)
{
	int	err;

	ft_bzero(list, sizeof(*list));

	list->options = options;

	list->users = hmap_i_new(FILE_HMAP_UID_SIZE);
	err = list->users == NULL;

	if (!err)
	{
		list->groups = hmap_i_new(FILE_HMAP_GID_SIZE);
		err = list->groups == NULL;

		if (!err)
		{
			// TODO: Use or remove
			list->field_widths = NULL;
		}
	}

	if (err)
		hmap_i_clr(&list->users, free);

	return (err);
}

int			file_list(t_file_list *list, const char *filepath)
{
	int			err;

	list->parent = filepath;
	list->options = list->options | LS_OFIRST;

	err = file_iter(filepath, (t_file_iter_fun*)file_load, list, DT_UNKNOWN);
	/* if ((list->options & LS_ORECURSE) == 0)
	else
		err = 0; */

	if (!err)
	{
		if ((list->options & LS_OREVERSE) == 0)
			ft_lstsort(&list->files, (t_cmp_fun*)file_cmp);
		else
			ft_lstsortrev(&list->files, (t_cmp_fun*)file_cmp);
	}
	return (err);
}
/*
static int		file_list_fw(const t_file_list *list)
{
	const t_list	*curr;
	const t_file	*file;
	int				fw;
	int				len;

	fw = 2;
	curr = list->files;
	while (curr)
	{
		file = curr->content;
		len = ft_strlen(file->name);
		if (len - 1 > fw)
			fw = len - 1;
		curr = curr->next;
	}
	return (fw);
} */

static blkcnt_t	file_list_fw_long(const t_file_list *list, t_field_widths fw)
{
	const t_list	*curr;
	const t_file	*file;
	const char		*name;
	unsigned		length;
	nlink_t			max_nlink;
	off_t			max_size;
	blkcnt_t		blocks;

	blocks = 0;
	max_nlink = 0;
	max_size = 0;
	curr = list->files;
	while (curr)
	{
		file = curr->content;

		name = hmap_i_get(list->users, file->uid)->value;
		length = ft_strlen(name);
		if (length > fw[LS_FUSER])
			fw[LS_FUSER] = length;

		name = hmap_i_get(list->groups, file->gid)->value;
		length = ft_strlen(name);
		if (length > fw[LS_FGROUP])
			fw[LS_FGROUP] = length;

		if (file->nlink > max_nlink)
			max_nlink = file->nlink;

		if (file->size > max_size)
			max_size = file->size;

		blocks += file->blocks * 512 / LS_BLOCK_SIZE;

		curr = curr->next;
	}
	fw[LS_FNLINKS] = ft_numlen(max_nlink, 10);
	fw[LS_FSIZE] = ft_numlen(max_size, 10);
	return (blocks);
}

static void	file_list_print_short(const t_file_list *list)
{
	t_list	*curr;

	curr = list->files;

	while (curr != NULL)
	{
		while (curr != NULL)
		{
			ft_printf("%s", ((t_file *)curr->content)->name);

			curr = curr->next;

			// TODO: Determine field width by finding max_len in each column
			// TODO: column count is capped to 1 if max_len exceeds 42

			if (curr != NULL)
				write(STDOUT_FILENO, " ", 1);
		}
		write(STDOUT_FILENO, "\n", 1);
	}
}

static void file_list_print_long(const t_file_list *list)
{
	t_field_widths	fw = {};
	const blkcnt_t	blocks = file_list_fw_long(list, fw);
	t_list			*curr;
	t_file			*file;

	curr = list->files;

	ft_printf("total: %ju\n", (uintmax_t)blocks);
	while (curr != NULL)
	{
		file = curr->content;

		file_print_long(file, hmap_i_get(list->users, file->uid)->value,
			hmap_i_get(list->groups, file->gid)->value, fw, list->options);

		curr = curr->next;
	}
}

void	file_list_print(t_file_list *list)
{
	t_list	*curr;
	t_file	*file;

	if (list->options & LS_ORECURSE)
	{
		if (list->options & LS_OFIRST)
		{
			list->options &= ~LS_OFIRST;
			ft_printf("%s:\n", list->parent);
		}
		else
			ft_printf("\n%s:\n", list->parent);
	}

	if ((list->options & LS_OLONGFMT) == 0)
		file_list_print_short(list);
	else
		file_list_print_long(list);

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
				};
				file_list_print(&children_list);
			}
			curr = curr->next;
		}
	}
}

void	file_list_clear(t_file_list *list)
{
	hmap_i_clr(&list->users, free);
	hmap_i_clr(&list->groups, free);
	ft_lstclear(&list->directories, NULL);
	ft_lstclear(&list->files, NULL);
}
