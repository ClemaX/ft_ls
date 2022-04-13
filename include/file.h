#pragma once

# define FILE_ISBACKREF(name) (name[0] == '.' && ((name[1] == '.' && name[2] == '\0') || name[1] == '\0'))
