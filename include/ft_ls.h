#pragma once

#include <stdint.h>

typedef int8_t	t_ls_opt;

# define LS_ISBACKREF(name) (name[0] == '.' && ((name[1] == '.' && name[2] == '\0') || name[1] == '\0'))

# define LS_OERROR		-1
# define LS_OLONGFMT	0b00000001
# define LS_ORECURSE	0b00000010
# define LS_OALL		0b00000100
# define LS_OREVERSE	0b00001000
