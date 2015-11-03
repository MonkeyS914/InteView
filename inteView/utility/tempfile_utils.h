
/*
* $Id: tempfile_utils.h,v 1.2 2008-11-12 02:38:59 chad Exp $
*/

#ifndef _TEMPFILE_UTILS
#define _TEMPFILE_UTILS

#include <wchar.h>

#define alter
#define TEMPFILE_UTILS_BUFFER_SIZE 4096

// Populates the passed in buffer which will be populated with a path to a temporary file (with optional extension)
extern void GenerateTempFileName ( char alter * tempFileNameOut, char const * extension = 0 );

// Populates the passed in buffer which will be populated with a path to a temporary file (with optional extension)
extern void GenerateTempFileName ( wchar_t alter * tempFileNameOut, wchar_t const * extension = 0 );

// Populates the passed in buffer with a path to a temporary directory
extern void GenerateTempDirectoryName ( wchar_t alter * tempDirectoryNameOut );

#endif // _TEMPFILE_UTILS
