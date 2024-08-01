#ifndef __WEB_UTIL_H
#define __WEB_UTIL_H
#include "http_response.h"
#include "network_util.h"
#include "http_request.h"
#include "http_response.h"
#include <sys/stat.h>
#include <stdio.h>


char *wutil_get_filename_ext(const char *filename);
ssize_t wutil_get_file_size(FILE *f);
char *wutil_get_resolved_path(request_t *request);
response_code_t wutil_check_resolved_path(char* resolved_path);
mime_type_t wutil_get_mime_from_extension(char *ext);

#endif // __WEB_UTIL_H
