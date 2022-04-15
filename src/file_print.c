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

static const char	*dir_basename(const char *path)
{
	int			i;
	const char	*basename;

	// TODO: Check safety and error cases
	i = 0;
	basename = path;
	while (path[i] != '\0')
	{
		if (path[i] == '/' && path[i + 1])
			basename = (char *)(path + i + 1);
		i++;
	}
	return (basename);
}

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

int	file_print(t_file *file, t_ls_opt options, const int fw[FILE_FIELD_COUNT])
{
	(void)options;
	t_file_mode	mode;
	const char	*basename;
	const struct passwd	*user;
	const struct group	*group;
	const char			*time;
	int			err;

	err = 0;
	if (S_ISDIR(file->mode))
		basename = dir_basename(file->name);
	else
		basename = file->name;

	file_mode(mode, file);

	user = getpwuid(file->uid);
	group = getgrgid(file->gid);

	time = file_time(file);

	// TODO: Use macros for lu type identifiers
	ft_printf("%*s %*lu %*s %*s %*lu %*s %s\n",
		fw[0], mode,
		fw[1], file->nlink,
		fw[2], user->pw_name,
		fw[3], group->gr_name,
		fw[4], file->size,
		fw[5], time,
		basename);
	return (err);
}
