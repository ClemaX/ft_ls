#include <linux/limits.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

#include <libft.h>

#include <ft_ls.h>

static const char       *option_descriptions[] =
{
	"Display more information about each file",
	"Recursively list subdirectories encountered",
	"Include directory entries whose name begin with a dot",
	"Reverse the order of sort",
};

static const char	*option_names[] =
{
	"long-format",
	"recursive",
	"all",
	"reverse-sort",
};

static const char	option_short_names[] =
{
	'l',
	'R',
	'a',
	'r',
};

static t_ls_opt		get_long_option(const char *name)
{
	t_ls_opt	option;

	option = 0;
	while ((unsigned)option < sizeof(option_names) / sizeof(*option_names)
	&& ft_strcmp(option_names[option], name) != 0)
			option++;
	if (option != sizeof(option_names) / sizeof(*option_names))
			option = 1 << option;
	else
			option = LS_OERROR;
	return (option);
}

static t_ls_opt		get_short_option(char c)
{
	const int	i = ft_strpos(option_short_names, c);
	t_ls_opt	option;

	if (i != -1)
			option = 1 << i;
	else
			option = LS_OERROR;
	return (option);

}

static t_ls_opt		get_options(int *i , const char *const *const av)
{
	t_ls_opt	options;
	t_ls_opt	option;
	size_t		j;

	options = 0;
	j = 0;
	while (options != LS_OERROR && av[*i] != NULL && av[*i][j] == '-')
	{
		++j;
		if (av[*i][j] == '-')
		{
			++j;
			option = get_long_option(av[*i] + j);
			if (option != LS_OERROR)
				options |= option;
			else
			{
				options = option;
				ft_dprintf(STDERR_FILENO, "%s: illegal option -- %s\n",
					av[0], av[*i] + j);
			}
		}
		else
		{
			while (options != LS_OERROR && av[*i][j] != '\0')
			{
				option = get_short_option(av[*i][j]);
				if (option != LS_OERROR)
				{
					options |= option;
					++j;
				}
				else
				{
					options = option;
					ft_dprintf(STDERR_FILENO, "%s: illegal option -- %c\n",
						av[0], av[*i][j]);
				}
			}
		}
		(*i)++;
		j = 0;
	}
	return (options);
}

static void		print_usage(const char *prog)
{
	const char      *fmt;
	unsigned        i;

	ft_printf(
"Usage: %s [-lRart] [FILE]...\n\
\
List files in [FILE]... (the current working directory by default).\n\n\
The options are:\n\
", prog
	);
	i = 0;
	while (i < sizeof(option_short_names))
	{
		if (option_names[i] == NULL)
			fmt =  " -%c%-22.0s%s\n";
		else
			fmt = " -%c, --%-18s%s\n";
		ft_dprintf(2, fmt, option_short_names[i], option_names[i],
			option_descriptions[i]);
		i++;
	}
}

typedef int (t_file_iter_fun(const char *filepath, const struct stat *st,
	void *data));

int	file_iter(const char *filepath, t_file_iter_fun *fun, void *fun_data)
{
	static char		full_path[PATH_MAX];
	struct stat 	st;
	DIR				*dir;
	struct dirent	*ent;
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
				ent = readdir(dir);
				while (ent != NULL && err == 0)
				{
					if (ent->d_type == DT_DIR)
					{
						if (!LS_ISBACKREF(ent->d_name))
							err = fun(ent->d_name, &st, fun_data);
					}
					else
					{
						err = path_cat(full_path, filepath, ent->d_name) == NULL;
						if (err == 0)
						{	
							err = stat(full_path, &st);
							if (err == 0)
								err = fun(ent->d_name, &st, fun_data);	
						}
						// TODO: Handle MAX_PATH error
					}
					ent = readdir(dir);
				} 
				closedir(dir);
			}
		}
		else
			err = fun(filepath, &st, fun_data);
	}
	// TODO: Handle access errors
	return (err);
}

int	file_print(const char *filepath, const struct stat *st, void *data)
{
	t_ls_opt	options = *(t_ls_opt*)data;

	if (options & LS_OALL || *filepath != '.')
		ft_dprintf(STDOUT_FILENO, "path: %-24s mode: %d\n", filepath, st->st_mode);
	return (0);
}

int	file_list(t_list **list, const char *filepath, t_ls_opt options)
{
	(void)list;
	struct stat	st;
	int			err;

	err = 0;
	if ((options & LS_ORECURSE) == 0)
		err = file_iter(filepath, file_print, &options);
	else
	{
		err = stat(filepath, &st);
		if (err == 0)
			file_print(filepath, &st, &options);
		// TODO: Handle access errors
	}
	return (err);
}

int	ft_ls(const char *filepath, t_ls_opt options)
{
	t_list	*files;
	int		err;


	err = file_list(&files, filepath, options);

	return (err);
}

/*
 * Usage: ft_ls [-lRart]
 * 	-l	Long format.
 * 	-R	Recursively list subdirectories encountered.
 * 	-a	Include directory entries whose name begin with a dot.
 * 	-r	Reverse the order of sort.
 *
 * Long format:
 * MODE NLINK OWNER_NAME GROUP_NAME SIZE MODIFICATION_TIME NAME
 *
 * Owner name:
 * If the owner or group names are not a known user or group name,
 * or the -n option is given, the numeric ID's are displayed.
 *
 *
 * Modification time:
 * abbreviated month, day-of-month file was last modified, hour file last
 * modified, minute file last modified.
 *
 * If time is more than 6 months in the past or future, then the year of the
 * last modification is displayed in place of the hour and minute fields.
 *
 *
 * Directories:
 * Print 'filename:' if multiple files are listed and 'total [TOTAL_BLOCKS]'
 * where TOTAL_BLOCKS is the sum of the directories immediate files allocated
 * 512-bytes blocks.
 *
 *
 * Extended attributes:
 * The MODE field is followed by an '@' if extended attributes are present.
 *
 *
 * Extended security attributes:
 * The MODE field is followed by a '+' if the file has extended security
 * information, such as an access control list or the SUID bit.
 *
 *
 * Character devices and special block files:
 * The major and minor device numbers for the file are displayed in the size
 * field.
 *
 *
 * Symbolic links:
 * The pathname of the linked-to file is preceded by ``->''.
 *
 *
 * Mode:
 * TYPE OWNER_PERM GROUP_PERM
 *
 * TYPE can be one of:
 * 	b	Block special file.
 * 	c	Character special file.
 * 	d	Directory.
 * 	l	Symbolic link.
 * 	s	Socket link.
 * 	p	FIFO.
 * 	-	Regular file.
 *
 * PERM fields are composed of:
 * 	r	if the file is readable, or - otherwise.
 * 	w	if the file is writable, or - otherwise.
 * 	the first one of:
 *  	S	if the file is not executable, and the user or group set-user-ID mode
 * 		is set.
 *		s	if the file is executable, and set-user-ID or set-group-ID mode is set.
 *		x	if the file is executable, or the directory is searchable.
 *		-	if the file is not executable and has no sticky bit.
 *		T	if the sticky bit is set, but not execute or search permission.
 *		t	if the sticky bit is set, and is searchable or executable.
 */

int	main(int ac, const char **av)
{
	int			i;
	t_ls_opt	options;
	int			err;

	i = 1;
	options = get_options(&i, av);
	err = options == LS_OERROR;
	if (err == 0)
	{
		if (i == ac)
			err = ft_ls(".", options);
		else
		{
			while (i < ac)
			{
				err |= ft_ls(av[i], options);
				i++;
			}
		}
		if (err != 0)
			perror(av[0]);
	}
	else
		print_usage(av[0]);
	return (err);
}
