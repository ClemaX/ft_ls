#pragma once

#include <stddef.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdint.h>

#include <libft/lists.h>
#include <libft/hmap_i.h>

#include <file.h>
#include <time.h>

typedef int8_t	t_ls_opt;

/*
#define TIME_SECOND 1
#define TIME_MINUTE (TIME_SECOND * 60)
#define TIME_HOUR (TIME_MINUTE * 60)
#define TIME_DAY (TIME_MINUTE * 24)
#define TIME_MONTH (TIME_DAY * 30)
*/

#ifndef LS_BLOCK_SIZE
# define LS_BLOCK_SIZE 1024
#endif

#define LS_OERROR		-1
// Long format
#define LS_OLONGFMT	0b00000001
// Recurse into subdirectories
#define LS_ORECURSE	0b00000010
// Show dotfiles
#define LS_OALL		0b00000100
// Reverse the sort order
#define LS_OREVERSE	0b00001000
// Show one file per line
#define LS_OONE		0b00010000
// Color special files
#define LS_OCOLOR	0b01000000
// First entry, do not write empty line
#define LS_OFIRST	0b10000000

extern struct winsize	ws;

enum			e_ls_fields
{
	LS_FMODE, LS_FNLINKS, LS_FUSER, LS_FGROUP, LS_FSIZE, LS_FMTIME, LS_FNAME,
};

typedef struct	s_file
{
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
	t_list		*directories;
	t_hmap_i	*users;
	t_hmap_i	*groups;
	const char	*parent;
	int			*field_widths;
	t_ls_opt	options;
}				t_file_list;

int		file_cmp_name(const char *basename_a, const char *basename_b);


int		file_load(const char *filepath, const char *basename,
	const struct stat *st, t_file_list *data);

int		file_list_init(t_file_list *list, t_ls_opt options);

int		file_list(t_file_list *list, const char *filepath);

void	file_list_clear(t_file_list *list);

void	file_list_print(t_file_list *list);
