//
// Copyright (c) 2000 by Tech Soft 3D, LLC.
// The information contained herein is confidential and proprietary to
// Tech Soft 3D, LLC., and considered a trade secret as defined under
// civil and criminal statutes.  Tech Soft 3D shall pursue its civil
// and criminal remedies in the event of unauthorized use or misappropriation
// of its trade secrets.  Use of this information by anyone other than
// authorized employees of Tech Soft 3D, LLC. is granted only under a
// written non-disclosure agreement, expressly prescribing the scope and
// manner of such use.
//
// $Header: /files/homes/master/cvs/hoops_master/shared/stream_common/BInternal.h,v 1.21 2008-05-06 17:22:23 reuben Exp $
//




#ifndef BSTREAM_INTERNALS
#define BSTREAM_INTERNALS


#ifndef null
#   define null         0
#endif
#ifndef nobreak
#   define nobreak      
#endif
#ifndef countof
#   define countof(x)   (int)(sizeof(x) / sizeof(x[0]))
#endif
#ifndef forever
#   define forever      while (1)
#endif
#ifndef until
#   define until(x)     while (!(x))
#endif


#define streq(a,b)      (!strcmp(a,b))
#define streqn(a,b,n)   (!strncmp(a,b,n))

#ifdef _MSC_VER	    
#   pragma warning (disable: 4127 4996)  
#endif

#ifndef POINTER_SIZED_INT
#if defined(WIN64) || defined(_M_X64) || defined(_WIN64)
#   define POINTER_SIZED_INT __int64
#   define POINTER_SIZED_UINT unsigned __int64
#else
#   define POINTER_SIZED_INT long
#   define POINTER_SIZED_UINT unsigned long
#endif
#endif

#define UL2VP(x) (void*)(POINTER_SIZED_UINT)x
#define SL2VP(x) (void*)(POINTER_SIZED_INT)x
#define VP2UL(x) (unsigned long)(POINTER_SIZED_UINT)x
#define VP2SL(x) (long)(POINTER_SIZED_INT)x


#ifndef UNREFERENCED
  #define UNREFERENCED(x) (void)(x)
#endif


#ifdef _DEBUG
	#ifndef _WINDEF_
		extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA (char const * str);
		#define OutputDebugString OutputDebugStringA
	#endif

    #include <stdio.h>
    #include <stdarg.h>
    inline void dprintf (char const * format, ...) {
        auto        char        buffer[1024];
        auto        va_list     arguments;

        va_start (arguments, format);  
        vsprintf (buffer, format, arguments);
        va_end (arguments);  
        #ifdef _MSC_VER
#ifndef _WIN32_WCE
            OutputDebugStringA (buffer);
#endif
        #else
            fprintf (stderr, buffer);
        #endif
    }
#else
    inline void dprintf (char const * format, ...) {
        UNREFERENCED(format);
    }
#endif


	#include <wchar.h>	
	#include <stdlib.h>
	#include <locale.h>
	#include <string.h>

	
	inline void wcsstrcpy (wchar_t * dp, char const * sp) {
		char * old_loc = setlocale(LC_CTYPE, "");
		mbstowcs(dp, sp, strlen(sp) +1);
		setlocale(LC_CTYPE, old_loc);
	}

	
	
	inline wchar_t const * wcschr2 (wchar_t const * str, wchar_t const c1, wchar_t const c2) {
		do
			if (*str == c1 || *str == c2)
				return str;
		while (*str++ != 0);
		return null;
	}
	inline wchar_t * wcschr2 (wchar_t * str, wchar_t const c1, wchar_t const c2) {
		do
			if (*str == c1 || *str == c2)
				return str;
		while (*str++ != 0);
		return null;
	}
	
	
	inline wchar_t const * wcsrchr2 (wchar_t const * str, wchar_t const c1, wchar_t const c2) {
		wchar_t const * p = null;
		do
			if (*str == c1 || *str == c2)
				p = str;
		while (*str++ != 0);
		return p;
	}
	inline wchar_t * wcsrchr2 (wchar_t * str, wchar_t const c1, wchar_t const c2) {
		wchar_t * p = null;
		do
			if (*str == c1 || *str == c2)
				p = str;
		while (*str++ != 0);
		return p;
	}

#endif  
