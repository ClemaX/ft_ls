#pragma once

#include "libft/hmap/hmap_i.h"
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>

#include <libft/lists.h>
#include <libft/hmap_i.h>

#include <file.h>
#include <time.h>

typedef int8_t	t_ls_opt;

# define LS_OERROR		-1
// Long format
# define LS_OLONGFMT	0b00000001
// Recurse into subdirectories
# define LS_ORECURSE	0b00000010
// Show dotfiles
# define LS_OALL		0b00000100
// Reverse the sort order
# define LS_OREVERSE	0b00001000
// Show one file per line
# define LS_OONE		0b00010000
// Color special files
# define LS_OCOLOR		0b10000000

typedef struct	s_file
{
	t_list		*children;
	char		*name;
	char		*path;
	mode_t		mode;
	nlink_t		nlink;
	uid_t		uid;
	gid_t		gid;
	off_t		size;
	blkcnt_t	blocks;
	time_t		atime;
	time_t		mtime;
	time_t		ctime;
}				t_file;

// Sizes to optimize the hashmaps for
#ifndef FILE_HMAP_UID_SIZE
# define FILE_HMAP_UID_SIZE 50
#endif
#ifndef FILE_HMAP_GID_SIZE
# define FILE_HMAP_GID_SIZE 100
#endif

typedef struct	s_file_list
{
	t_list		*files;
	t_hmap_i	*users;
	t_hmap_i	*groups;
	const char	*parent;
	t_ls_opt	options;
}				t_file_list;

int		file_list_init(t_file_list *list);

int		file_list(t_file_list *list, const char *filepath, t_ls_opt options);

void	file_list_clear(t_file_list *list);

void	file_list_print(t_file_list *list);
