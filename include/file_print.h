#pragma once

#include <sys/stat.h>

#include <file.h>

int	file_print(const char *filepath, const char *basename,
	const struct stat *st, void *data);
