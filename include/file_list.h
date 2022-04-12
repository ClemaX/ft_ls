#pragma once

#include <sys/types.h>

#include <libft.h>

typedef struct	s_file
{
	struct s_file	*children;
	mode_t			mode;
	uid_t			uid;
	gid_t			gid;
	off_t			size;
	blkcnt_t		blocks;
}				t_file;
