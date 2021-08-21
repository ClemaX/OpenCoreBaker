#pragma once

#include <zip.h>

#define ZIP_MODE		ZIP_RDONLY
#define ZIP_ST_MODE	0
#define ZIP_FL_MODE	0

#ifdef DEST_MODE
# define AR_DEST_MODE DEST_MODE
#else
# define AR_DEST_MODE (S_IRWXU | S_IRGRP | S_IROTH)
#endif

typedef enum	e_ar_status
{
	AR_SUCCESS = 0,
	AR_FAIL_ALLOC = -1,
	AR_FAIL_OPEN = -2,
	AR_FAIL_LOCATE = -3,
	AR_FAIL_OPEN_FILE = -4,
	AR_FAIL_OPEN_DEST = -5,
	AR_FAIL_WRITE_DEST = -6,
}				t_ar_status;

typedef	uint8_t t_ar_opt;

typedef int(*zip_ftw_cb)(zip_file_t *file, zip_stat_t *sb, const char *basename, const void *data);

#define AR_RECURSIVE	1

int		is_archive(const char *url);
int		archive_extract(const char *archive_path, const char *file_path,
	const char *dest_path, int options);

const char	*archive_strerror(t_ar_status error);
