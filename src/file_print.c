#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <libft/paths.h>
#include <libft/printf.h>

#include <file_list.h>
#include <file_iter.h>
#include <file_print.h>

static char file_type(const t_file *file)
{
	char	type;

	if (S_ISREG(file->mode))
		type = FILE_TREG;
	else if (S_ISBLK(file->mode))
		type = FILE_TBLK;
	else if (S_ISCHR(file->mode))
		type = FILE_TCHR;
	else if (S_ISDIR(file->mode))
		type = FILE_TDIR;
	else if (S_ISLNK(file->mode))
		type = FILE_TLNK;
	else if (S_ISSOCK(file->mode))
		type = FILE_TSOCK;
	else if (S_ISFIFO(file->mode))
		type = FILE_TFIFO;
	else
		type = FILE_TREG;
	return (type);
}

static char file_perm_special_user(const t_file *file)
{
	const bool	exec = file->mode & S_IXUSR;
	char		perm;

	if (!S_ISDIR(file->mode) && file->mode & S_ISUID)
		perm = exec ? FILE_PSID_EXEC : FILE_PSID_NOEXEC;
	else if (exec)
		perm = FILE_PEXEC;
	else
		perm = FILE_PNONE;
	return (perm);
}

static char file_perm_special_group(const t_file *file)
{
	const bool	exec = file->mode & S_IXGRP;
	char		perm;

	if (!S_ISDIR(file->mode) && file->mode & S_ISGID)
		perm = exec ? FILE_PSID_EXEC : FILE_PSID_NOEXEC;
	else if (exec)
		perm = FILE_PEXEC;
	else
		perm = FILE_PNONE;
	return (perm);
}

static char file_perm_special_other(const t_file *file)
{
	const bool	exec = file->mode & S_IXOTH;
	char		perm;

	if (file->mode & S_ISVTX)
		perm = exec ? FILE_PSTICKY_EXEC : FILE_PSTICKY_NOEXEC;
	else if (exec)
		perm = FILE_PEXEC;
	else
		perm = FILE_PNONE;
	return (perm);
}

static void	file_mode(t_file_mode dst, const t_file *file)
{
	dst[0] = file_type(file);

	dst[1] = file->mode & S_IRUSR ? FILE_PREAD : FILE_PNONE;
	dst[2] = file->mode & S_IWUSR ? FILE_PWRITE : FILE_PNONE;
	dst[3] = file_perm_special_user(file);

	dst[4] = file->mode & S_IRGRP ? FILE_PREAD : FILE_PNONE;
	dst[5] = file->mode & S_IWGRP ? FILE_PWRITE : FILE_PNONE;
	dst[6] = file_perm_special_group(file);

	dst[7] = file->mode & S_IROTH ? FILE_PREAD : FILE_PNONE;
	dst[8] = file->mode & S_IWOTH ? FILE_PWRITE : FILE_PNONE;
	dst[9] = file_perm_special_other(file);

	dst[10] = '\0';
}

const char	*file_time(t_file *file)
{
	char		*time;

	time = ctime(&file->mtime);
	time[16] = '\0';
	return (time);
}
/*
void	file_print_fw(t_file *files, t_field_widths fw)
{

}
 */
int	file_print(t_file *file, const char *user, const char *group,
	const t_field_widths fw, t_ls_opt options)
{
	(void)options;
	t_file_mode			mode;
	const char			*time;
	int					err;

	err = 0;

	file_mode(mode, file);

	time = file_time(file);

	// TODO: Prefetch user and group names in hashmap

	// TODO: Use macros for lu type identifiers
	ft_printf("%*s %*lu %*s %*s %*lu %*s %s\n",
		fw[0], mode,
		fw[1], file->nlink,
		fw[2], user,
		fw[3], group,
		fw[4], file->size,
		fw[5], time,
		file->name);
	return (err);
}
