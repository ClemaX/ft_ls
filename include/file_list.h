#pragma once

#include <sys/types.h>
#include <stdint.h>

#include <libft.h>

#include <file.h>

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
	struct s_file	*children;
	mode_t			mode;
	uid_t			uid;
	gid_t			gid;
	off_t			size;
	blkcnt_t		blocks;
}				t_file;

int	file_list(t_list **list, const char *filepath, void *data);
