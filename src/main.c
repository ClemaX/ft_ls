#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <libft/printf.h>
#include <libft/opts.h>
#include <libft/dirs.h>
#include <libft/paths.h>

#include <file_print.h>

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

static const opt_spec	opt_specs[] = {
	{
		.short_flag = 'l',
		.long_flag = "long-format",
		.description = "Display more information about each file",
	},
	{
		.short_flag = 'R',
		.long_flag = "recursive",
		.description = "Recursively list subdirectories encountered",
	},
	{
		.short_flag = 'a',
		.long_flag = "all",
		.description = "Include directory entries whose name begin with a dot",
	},
	{
		.short_flag = 'r',
		.long_flag = "reverse-sort",
		.description = "Reverse the order of sort",
	},
	{
		.short_flag = '1',
		.long_flag = "one",
		.description = "Display one file per line",
	},

};

/*
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
} */

static int ft_ls_load(t_file_list *ls, const char *progname, const char **files,
	t_ls_opt options)
{
	const char	*default_files[] = {".", NULL};
	struct stat	st;
	int			err;

	err = 0;

	// Use default files when given an empty file array.
	if (*files == NULL)
		files = default_files;

	// Enumerate each accessible file and directory.
	while (!err && *files != NULL)
	{
		err = stat(*files, &st);
		if (!err)
		{
			if (S_ISDIR(st.st_mode))
			{
				// Add subdirectories to the list if recursion is enabled.
				if (options & LS_ORECURSE)
					err = dir_list(&ls->directories, *files);

				// Add the directory to the list.
				if (!err)
					err = dir_add(&ls->directories, *files);
			}
			else
			{
				char	*basename = ft_basename(*files);
				// Add the file to the list
				err = basename == NULL || file_load(*files, basename, &st, ls);
				free(basename);
			}
		}
		else
		{
			err = 0;
			ft_dprintf(2, "%s: cannot access '%s': %s\n",
				progname, *files, strerror(errno));
		}
		++files;
	}
	return (err);
}

static void	ft_ls_sort(t_file_list *ls)
{
	if ((ls->options & LS_OREVERSE) == 0)
	{
		ft_lstsort(&ls->directories, (t_cmp_fun*)file_cmp_name);
		ft_lstsort(&ls->files, (t_cmp_fun*)file_cmp_name);
	}
	else
	{
		ft_lstsortrev(&ls->directories, (t_cmp_fun*)file_cmp_name);
		ft_lstsortrev(&ls->files, (t_cmp_fun*)file_cmp_name);
	}
}
// TODO: Fix ./ft_ls -

static int	ft_ls_print(t_file_list *ls)
{
	t_list	*curr;
	//t_file	*file;
	int		err;

	if (ls->files != NULL)
	{
		file_list_print(ls);
		ft_lstclear(&ls->files, NULL);
	}
	else
		ls->options |= LS_OFIRST;

	err = 0;
	curr = ls->directories;
	while (!err && curr != NULL)
	{
		//ft_dprintf(STDERR_FILENO, "Loading %s\n", (char*)curr->content);
		//files = NULL;

		// Load files from the current directory.
		//err = dir_load(&files, (char*)curr->content, DT_UNKNOWN, DIR_OBASENAME);

		err = file_list(ls, (const char *)curr->content);

		if (!err)
		{
			file_list_print(ls);
		}
		else
		{
			ft_dprintf(STDERR_FILENO, "file_list: err: %d: %s\n", err, strerror(errno));
		}

		ft_lstclear(&ls->files, NULL);

		curr = curr->next;
	}
	return (err);
}

// TODO: Fix listing with access errors
static int	ft_ls(const char *progname, const char **files, t_ls_opt options)
{
	t_file_list	ls;
	int			err;

	err = file_list_init(&ls, options);

	if (!err)
	{
		err = ft_ls_load(&ls, progname, files, options);

		if (!err)
		{
			ft_ls_sort(&ls);
			ft_ls_print(&ls);
		}

		file_list_clear(&ls);
	}
	return (err);
}

#ifdef BONUS
struct winsize	ws;
#endif

int	main(int ac, const char **av)
{
	int			i;
	t_ls_opt	options;
	int			err;

	(void)ac;

#ifdef BONUS
	// TODO: Use winsize to determine collumn formatting
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0)
	{
		ws.ws_col = -1;
		ws.ws_row = -1;
	}

	// TODO: Use locale to determine sort order
	setlocale(LC_COLLATE, "");
#endif

	i = 1;
	options = opts_get(opt_specs, sizeof(opt_specs) / sizeof(*opt_specs), av, &i, NULL);
	err = options == OPT_ERROR;
	if (!err)
	{
		err = ft_ls(av[0], av + i, options);
		if (err != 0)
			perror(av[0]);
	}
	else
		opts_usage(opt_specs, sizeof(opt_specs) / sizeof(*opt_specs), av[0], " [FILE]...");
	return (err);
}
