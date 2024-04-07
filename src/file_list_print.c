#include <unistd.h>

#include <libft/strings.h>
#include <libft/numbers.h>
#include <libft/printf.h>

#include <file_list.h>
#include <file_print.h>

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
	const char	*fmt = list->options & LS_OONE ? "%s\n" : "%s";
	t_list		*curr;

	curr = list->files;

	while (curr != NULL)
	{
		while (curr != NULL)
		{
			if (list->parent != NULL)
				ft_printf(fmt, ((t_file *)curr->content)->name);
			else
				ft_printf(fmt, ((t_file *)curr->content)->path);

			curr = curr->next;

			// TODO: Determine field width by finding max_len in each column
			// TODO: column count is capped to 1 if max_len exceeds 42

			if (curr != NULL && !(list->options & LS_OONE))
				write(STDOUT_FILENO, " ", 1);
		}
		if (!(list->options & LS_OONE))
			write(STDOUT_FILENO, "\n", 1);
	}
}

static void	file_list_print_long(const t_file_list *list)
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

void		file_list_print(t_file_list *list)
{
	if (list->parent && ((list->options & LS_ORECURSE) || list->directories->next))
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
}
