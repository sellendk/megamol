/*
 * types.h
 *
 * Copyright (C) 2006 by Universitaet Stuttgart (VIS). Alle Rechte vorbehalten.
 */

#ifndef VISLIB_TYPES_H_INCLUDED
#define VISLIB_TYPES_H_INCLUDED
#if (_MSC_VER > 1000)
#pragma once
#endif /* (_MSC_VER > 1000) */


#ifdef _WIN32
#include <windows.h>

#else /* _WIN32 */

#include <inttypes.h>

typedef char CHAR;
typedef char INT8;
typedef unsigned char UCHAR;
typedef unsigned char UINT8;
typedef unsigned char BYTE;

typedef wchar_t WCHAR;

typedef int16_t SHORT;
typedef int16_t INT16;
typedef uint16_t USHORT;
typedef uint16_t UINT16;
typedef uint16_t WORD;

typedef int32_t INT;
typedef int32_t INT32;
typedef int32_t LONG;
typedef uint32_t UINT;
typedef uint32_t UINT32;
typedef uint32_t ULONG;
typedef uint32_t DWORD;

typedef int64_t LONGLONG;
typedef int64_t INT64;
typedef uint64_t ULONGLONG;
typedef uint64_t UINT64;


// TODO: Remove float/double or add bool etc.?

typedef float FLOAT;
typedef double DOUBLE;

#ifdef _LIN64
typedef INT64 INT_PTR;
typedef UINT64 UINT_PTR;
#else /* _LIN64 */
typedef INT32 INT_PTR;
typedef UINT32 UINT_PTR;
#endif /* _LIN64 */

#endif /* _WIN32 */

typedef UINT64 EXTENT;

#endif /* VISLIB_TYPES_H_INCLUDED */
