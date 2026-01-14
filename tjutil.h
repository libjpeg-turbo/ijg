/*
 * Copyright (C)2011, 2022-2023 D. R. Commander.  All Rights Reserved.
 * Copyright (C)2021 Alex Richardson.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the libjpeg-turbo Project nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS",
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#define JPEG_INTERNALS
#include "jconfig.h"

#ifdef _WIN32
#ifndef strcasecmp
#define strcasecmp  stricmp
#endif
#ifndef strncasecmp
#define strncasecmp  strnicmp
#endif
#endif


/* JUINTPTR must hold pointer values. */
#ifdef __UINTPTR_TYPE__
/*
 * __UINTPTR_TYPE__ is GNU-specific and available in GCC 4.6+ and Clang 3.0+.
 * Fortunately, that is sufficient to support the few architectures for which
 * sizeof(void *) != sizeof(size_t).  The only other options would require C99
 * or Clang-specific builtins.
 */
typedef __UINTPTR_TYPE__ JUINTPTR;
#else
typedef size_t JUINTPTR;
#endif


/*
 * These macros/inline functions facilitate using Microsoft's "safe string"
 * functions with Visual Studio builds without the need to scatter #ifdefs
 * throughout the code base.
 */


#ifdef _MSC_VER

#define SNPRINTF(str, n, format, ...) \
  _snprintf_s(str, n, _TRUNCATE, format, ##__VA_ARGS__)

#else

#define SNPRINTF  snprintf

#endif


#ifndef NO_GETENV

#ifdef _MSC_VER

static INLINE int GETENV_S(char *buffer, size_t buffer_size, const char *name)
{
  size_t required_size;

  return (int)getenv_s(&required_size, buffer, buffer_size, name);
}

#else /* _MSC_VER */

#include <errno.h>

/* This provides a similar interface to the Microsoft/C11 getenv_s() function,
 * but other than parameter validation, it has no advantages over getenv().
 */

static INLINE int GETENV_S(char *buffer, size_t buffer_size, const char *name)
{
  char *env;

  if (!buffer) {
    if (buffer_size == 0)
      return 0;
    else
      return (errno = EINVAL);
  }
  if (buffer_size == 0)
    return (errno = EINVAL);
  if (!name) {
    *buffer = 0;
    return 0;
  }

  env = getenv(name);
  if (!env)
  {
    *buffer = 0;
    return 0;
  }

  if (strlen(env) + 1 > buffer_size) {
    *buffer = 0;
    return ERANGE;
  }

  strncpy(buffer, env, buffer_size);

  return 0;
}

#endif /* _MSC_VER */

#endif /* NO_GETENV */


#ifndef NO_PUTENV

#ifdef _WIN32

#define PUTENV_S(name, value)  _putenv_s(name, value)

#else

#include <errno.h>

/* This provides a similar interface to the Microsoft _putenv_s() function, but
 * other than parameter validation, it has no advantages over setenv().
 */

static INLINE int PUTENV_S(const char *name, const char *value)
{
  if (!name || !value)
    return (errno = EINVAL);

  setenv(name, value, 1);

  return errno;
}

#endif /* _WIN32 */

#endif /* NO_PUTENV */


#ifndef min
#define min(a, b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b)  ((a) > (b) ? (a) : (b))
#endif

extern double getTime(void);
