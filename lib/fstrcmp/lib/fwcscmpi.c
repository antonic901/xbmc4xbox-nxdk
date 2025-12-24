/*
 * fstrcmp - fuzzy string compare library
 * Copyright (C) 2009 Peter Miller
 *
 *
 * Derived from gettext 0.17
 * Copyright (C) 1988-2006 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Derived from GNU diff 2.7, analyze.c et al.
 *
 * The basic idea is to consider two vectors as similar if, when
 * transforming the first vector into the second vector through a
 * sequence of edits (inserts and deletes of one element each), this
 * sequence is short - or equivalently, if the ordered list of elements
 * that are untouched by these edits is long.  For a good introduction
 * to the subject, read about the "Levenshtein distance" in Wikipedia.
 *
 * The basic algorithm is described in:
 * "An O(ND) Difference Algorithm and its Variations", Eugene Myers,
 * Algorithmica Vol. 1 No. 2, 1986, pp. 251-266;
 * see especially section 4.2, which describes the variation used below.
 *
 * The basic algorithm was independently discovered as described in:
 * "Algorithms for Approximate String Matching", E. Ukkonen, Information
 * and Control Vol. 64, 1985, pp. 100-118.
 *
 * Unless the 'find_minimal' flag is set, this code uses the
 * TOO_EXPENSIVE heuristic, by Paul Eggert, to limit the cost to
 * O(N**1.5 log N) at the price of producing suboptimal output for large
 * inputs with many differences.
 */

#include <lib/fstrcmp.h>

#include <lib/ac/string.h>
#include <lib/ac/stdio.h>
#include <lib/ac/stdlib.h>
#include <lib/ac/wchar.h>
#include <limits.h>

#include <lib/minmax.h>
#include <lib/nmalloc.h>

#ifndef uintptr_t
# define uintptr_t unsigned long
#endif


#define ELEMENT wchar_t
#define EQUAL(x,y) ((x) == (y))
#define OFFSET_T ssize_t
#define OFFSET OFFSET_T
#define EXTRA_CONTEXT_FIELDS \
  /* The number of elements inserted or deleted. */ \
  size_t xvec_edit_count; \
  size_t yvec_edit_count;
#define NOTE_DELETE(ctxt, xoff) ctxt->xvec_edit_count++
#define NOTE_INSERT(ctxt, yoff) ctxt->yvec_edit_count++
/*
 * We don't need USE_HEURISTIC, since it is unlikely in typical uses of
 * fstrcmp().
 */
#include <lib/diffseq.h>


/*
 * Because fstrcmp is typically called multiple times, attempt to
 * minimize the number of memory allocations performed.  Thus, let a
 * call reuse the memory already allocated by the previous call, if
 * it is sufficient.
 *
 * This isn't thread safe.
 */

static OFFSET_T *buffer;
static size_t bufmax;


int
fwcscmpi(const wchar_t *wcs1, const wchar_t *wcs2)
{
    struct context  ctxt;
    int             i;
    size_t          fdiag_len;
    size_t          size1;
    size_t          size2;
    size_t          numerator;
    size_t          denominator;

    /*
     * set the info for each string.
     */
    ctxt.xvec = wcs1;
    ctxt.yvec = wcs2;

    /*
     * short-circuit obvious comparisons
     */
    size1 = wcslen(wcs1);
    size2 = wcslen(wcs2);
    if (size1 == 0 && size2 == 0)
        return 1.0;
    if (size1 == 0 || size2 == 0)
        return 0.0;

    /*
     * Set TOO_EXPENSIVE to be approximate square root of input size,
     * bounded below by 256.
     */
    ctxt.too_expensive = 1;
    for (i = size1 + size2; i != 0; i >>= 2)
        ctxt.too_expensive <<= 1;
    if (ctxt.too_expensive < 256)
        ctxt.too_expensive = 256;

    /*
     * Allocate memory for fdiag and bdiag.
     */
    fdiag_len = size1 + size2 + 3;
    if (fdiag_len > bufmax)
    {
        /* Need more memory.  */
        bufmax = 2 * bufmax;
        if (fdiag_len > bufmax)
            bufmax = fdiag_len;
        /*
         * Calling realloc() would be a waste: buffer's contents do not
         * need to be preserved.
         */
        if (buffer != NULL)
            free(buffer);
        buffer = (OFFSET_T *)fstrcmp_nmalloc(bufmax, 2 * sizeof(OFFSET_T));
        if (!buffer)
            return FSTRCMP_ERROR;
    }
    ctxt.fdiag = buffer + size2 + 1;
    ctxt.bdiag = ctxt.fdiag + fdiag_len;

    /*
     * Now do the main comparison algorithm
     */
    ctxt.xvec_edit_count = 0;
    ctxt.yvec_edit_count = 0;
    compareseq(0, size1, 0, size2, 0, &ctxt);

    /*
     * The result is
     *
     *     ((number of chars in common) / (average length of the strings)).
     *
     * This is admittedly biased towards finding that the strings are
     * similar, however it does produce meaningful results.
     */
    numerator = size1 + size2 - ctxt.yvec_edit_count - ctxt.xvec_edit_count;
    denominator = size1 + size2;
    return ((FSTRCMPI_IDENTICAL * numerator + denominator / 2) / denominator);
}
