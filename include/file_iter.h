#pragma once

#include <sys/stat.h>

#include <file.h>

typedef int (t_file_iter_fun(const char *filepath, const char *basename,
	const struct stat *st, void *data));

int	file_iter(const char *filepath, t_file_iter_fun *fun, void *fun_data,
	unsigned char filter);
