#pragma once

#include <sys/stat.h>

#include <file_list.h>
#include <file.h>

// Regular file
#define FILE_TREG '-'
// Block special file
#define FILE_TBLK 'b'
// Character special file
#define FILE_TCHR 'c'
// Directory
#define FILE_TDIR 'd'
// Symbolic link
#define FILE_TLNK 'l'
// Socket link
#define FILE_TSOCK 's'
// FIFO
#define FILE_TFIFO 'p'

// Not readable, writable or executable, no sticky bits
#define FILE_PNONE '-'
// Readable
#define FILE_PREAD 'r'
// Writable
#define FILE_PWRITE 'w'
// Not executable, set-user-ID or set-group-ID mode is set
#define FILE_PSID_NOEXEC 'S'
// Executable, set-user-ID or set-group-ID mode is set
#define FILE_PSID_EXEC 's'
// Executable
#define FILE_PEXEC 'x'
// Not executable, sticky bit is set
#define FILE_PSTICKY_NOEXEC 'T'
// Executable, sticky bit is set
#define FILE_PSTICKY_EXEC 't'

// Size of the file mode field in bytes
#define FILE_MODE_SIZE (1 + 3 * 3 + 1)

// Long format field count
# define FILE_FIELD_COUNT 6

typedef char		t_file_mode[FILE_MODE_SIZE];

typedef unsigned	t_field_widths[FILE_FIELD_COUNT];


int					file_print(t_file *file, t_ls_opt options, int fw);

int					file_print_long(t_file *file, const char *user,
	const char *group, const t_field_widths fw, t_ls_opt options);

