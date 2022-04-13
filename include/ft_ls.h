#pragma once

#include <stdint.h>

typedef int8_t	t_ls_opt;

# define LS_ISBACKREF(name) (name[0] == '.' && ((name[1] == '.' && name[2] == '\0') || name[1] == '\0'))

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
