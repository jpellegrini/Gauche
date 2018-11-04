/*
 * paths.c - get 'known' pathnames, such as the system's library directory.
 *
 *   Copyright (c) 2005-2018  Shiro Kawai  <shiro@acm.org>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* This file is used by both libgauche and gauche-config.  The latter
 * doesn't use ScmObj, so the function works on bare C strings.
 */

#define LIBGAUCHE_BODY
#include "gauche.h"
#include "paths_arch.c"         /* generated by genconfig */

#if !defined(PATH_ALLOC)
#define PATH_ALLOC(n)  SCM_MALLOC_ATOMIC(n)
#endif

#if defined(GAUCHE_WINDOWS)
#include "getdir_win.c"
#elif defined(GAUCHE_MACOSX_FRAMEWORK)
#include "getdir_darwin.c"
#else
#include "getdir_dummy.c"
#endif

/* The configure-generated path macros might have '@' at the beginning,
   indicating the runtime directory. */
const char *maybe_prepend_install_dir(const char *orig,
                                      void (*errfn)(const char *, ...))
{
    if (*orig == '@') {
        const char *d = get_install_dir(errfn);
        size_t dlen = strlen(d);
        size_t olen = strlen(orig);
        char *buf = (char*)PATH_ALLOC(olen+dlen);
        strncpy(buf, d, dlen);
        strncpy(buf+dlen, orig+1, olen); /* includes terminator */
        return buf;
    } else {
        return orig;
    }
}

/* External API */

const char *Scm_HostArchitecture(void)
{
    return gauche_arch;
}

const char *Scm_GetLibraryDirectory(void (*errfn)(const char *, ...))
{
    return maybe_prepend_install_dir(gauche_lib_dir, errfn);
}

const char *Scm_GetArchitectureDirectory(void (*errfn)(const char *, ...))
{
    return maybe_prepend_install_dir(gauche_arch_dir, errfn);
}

const char *Scm_GetSiteLibraryDirectory(void (*errfn)(const char *, ...))
{
    return maybe_prepend_install_dir(gauche_site_lib_dir, errfn);
}

const char *Scm_GetSiteArchitectureDirectory(void (*errfn)(const char *, ...))
{
    return maybe_prepend_install_dir(gauche_site_arch_dir, errfn);
}

/* On windows and darwin, this returns the runtime's prefix directory
   that can be replaced with '@'.  Note that it calls errfn on other
   platforms.  Eventually we want to cover more platforms, for getting
   directory of the running binary is sometimes useful in general. */
const char *Scm_GetRuntimeDirectory(void (*errfn)(const char *, ...))
{
    return get_install_dir(errfn);
}
