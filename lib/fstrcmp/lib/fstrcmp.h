/*
 * fstrcmp - fuzzy string compare library
 * Copyright (C) 2009 Peter Miller
 * Written by Peter Miller <pmiller@opensource.org.au>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FSTRCMP_H
#define FSTRCMP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  * The FSTRCMP_IDENTICAL constant may be used where the upper bound of
  * the fstrcmp result is required.
  */
#define FSTRCMP_IDENTICAL 1.0

/**
  * The FSTRCMP_THRESHOLD constant may be used to decide "usefully
  * similar" questions.  Pairs of strings that return and fstrcmp result
  * larger than this threshold are what humans would consider "similar".
  */
#define FSTRCMP_THRESHOLD (FSTRCMP_IDENTICAL * 3 / 5)

/**
  * The FSTRCMP_ERROR constant msy be used to test whether or not the
  * fstrcmp function has returned an error.  (Actually, any value < 0
  * indicates and error, and < 0 can be a faster test.)
  */
#define FSTRCMP_ERROR -1

/**
  * The FSTRCMPI_IDENTICAL constant may be used where the upper bound of
  * the fstrcmpi result is required.
  *
  * Don't assume the value will always be the one you see here.  Always
  * use the defined symbols.  You can assume it will always be positive.
  */
#define FSTRCMPI_IDENTICAL 10000

/**
  * The FSTRCMPI_THRESHOLD constant may be used to decide "usefully
  * similar" questions.  Pairs of strings that return and fstrcmpi result
  * larger than this threshold are what humans would consider "similar".
  */
#define FSTRCMPI_THRESHOLD (FSTRCMPI_IDENTICAL * 3 / 5)

/**
  * The FSTRCMPI_ERROR constant msy be used to test whether or not the
  * fstrcmpi function has returned an error.  (Actually, any value < 0
  * indicates and error, and < 0 can be a faster test.)
  */
#define FSTRCMPI_ERROR -1

/**
  * The fstrcmp function may be used to make a fuzzy comparison between
  * two strings.  Note that this function's units of operation are
  * bytes, not characters.
  *
  * This function is very useful in reducing "cascade" or "secondary"
  * errors in compilers or other situations where symbol tables occur.
  *
  * @param string1
  *    The first string to be compared.
  * @param string2
  *    The first string to be compared.
  * @returns
  *    a number between 0.0 and FSTRCMP_IDENTICAL.  The value 0.0 means
  *    the strings are utterly un-alike, and FSTRCMP_IDENTICAL means the
  *    strings are identical.  A value of FSTRCMP_ERROR indicates an
  *    error (malloc failure).
  */
double fstrcmp(const char *string1, const char *string2);

/**
  * The fstrcmpi function may be used to make a fuzzy comparison between
  * two strings.  Note that this function's units of operation are
  * bytes, not characters.
  *
  * This function is very useful in reducing "cascade" or "secondary"
  * errors in compilers or other situations where symbol tables occur.
  * Return values in excess of FSTRFCMPI_THRESHOLD are "similar".
  *
  * @param string1
  *    The first string to be compared.
  * @param string2
  *    The first string to be compared.
  * @returns
  *    a number between 0 and FSTRFCMPI_IDENTICAL.  The value 0 means
  *    the strings are utterly un-alike, and FSTRFCMPI_IDENTICAL means
  *    the strings are identical.  A value of FSTRFCMPI_ERROR indicates
  *    an error (malloc failure).
  */
int fstrcmpi(const char *string1, const char *string2);

/**
  * The fstrcasecmp function may be used to make a fuzzy comparison
  * between two strings, while ignoring the case of the characters.
  * Note that this function's units of operation are bytes, not
  * characters.
  *
  * @param s1
  *    The first string to be compared.
  * @param s2
  *    The second string to be compared.
  * @returns
  *    a number between 0.0 and 1.0.  The value 0.0 means the strings
  *    are utterly un-alike, and 1.0 means the strings are identical.  A
  *    value of -1 indicates and error (malloc failure).
  */
double fstrcasecmp(const char *s1, const char *s2);

/**
  * The fstrcasecmpi function may be used to make a fuzzy comparison
  * between two strings, while ignoring the case of the characters.
  * Note that this function's units of operation are bytes, not
  * characters.
  *
  * @param s1
  *    The first string to be compared.
  * @param s2
  *    The second string to be compared.
  * @returns
  *    a number between 0.0 and FSTRCMPI_IDENTICAL.  The value 0 means
  *    the strings are utterly un-alike, and FSTRCMPI_IDENTICAL means
  *    the strings are identical.  A value of FSTRCMPI_ERROR indicates
  *    and error (malloc failure).
  */
int fstrcasecmpi(const char *s1, const char *s2);

/**
  * The fmemcmp function may be used to make a fuzzy comparison between
  * two arrays of bytes.
  *
  * @param data1
  *    The first array to be compared.
  * @param size1
  *    The size in bytes of the first array to be compared.
  * @param data2
  *    The first array to be compared.
  * @param size2
  *    The size in bytes of the second array to be compared.
  * @returns
  *    a number between 0.0 and 1.0.  The value 0.0 means the arrays
  *    are utterly un-alike, and 1.0 means the strings are identical.
  *    A value of -1 indicates and error (malloc failure).
  */
double fmemcmp(const void *data1, size_t size1, const void *data2,
    size_t size2);

/**
  * The fmemcmpi function may be used to make a fuzzy comparison between
  * two arrays of bytes.
  *
  * @param data1
  *    The first array to be compared.
  * @param size1
  *    The size in bytes of the first array to be compared.
  * @param data2
  *    The first array to be compared.
  * @param size2
  *    The size in bytes of the second array to be compared.
  * @returns
  *    a number between 0 and FSTRFCMPI_IDENTICAL.  The value 0 means
  *    the strings are utterly un-alike, and FSTRFCMPI_IDENTICAL means
  *    the strings are identical.  A value of FSTRFCMPI_ERROR indicates
  *    an error (malloc failure).
  */
int fmemcmpi(const void *data1, size_t size1, const void *data2,
    size_t size2);

/**
  * The fwcscmp function may be used to make a fuzzy comparison between
  * two wide-character strings.
  *
  * @param string1
  *    The first string to be compared.
  * @param string2
  *    The first string to be compared.
  * @returns
  *    a number between 0.0 and FSTRCMP_IDENTICAL.  The value 0.0 means
  *    the strings are utterly un-alike, and FSTRCMP_IDENTICAL means the
  *    strings are identical.  A value of FSTRCMP_ERROR indicates an
  *    error (malloc failure).
  */
double fwcscmp(const wchar_t *string1, const wchar_t *string2);

/**
  * The fwcscmpi function may be used to make a fuzzy comparison between
  * two wide-character strings.
  *
  * @param string1
  *    The first string to be compared.
  * @param string2
  *    The first string to be compared.
  * @returns
  *    a number between 0 and FSTRCMPI_IDENTICAL.  The value 0 means the
  *    strings are utterly un-alike, and FSTRCMPI_IDENTICAL means the
  *    strings are identical.  A value of FSTRCMPI_ERROR indicates an
  *    error (malloc failure).
  */
int fwcscmpi(const wchar_t *string1, const wchar_t *string2);

/**
  * The fstrcoll function may be used to make a fuzzy comparison
  * between two multi-byte-character strings.  Note that this function's
  * units of operation are characters, possibly multi-byte characters.
  *
  * The behavior of fstrcoll() depends on the LC_CTYPE category of the
  * current locale.
  *
  * @param string1
  *    The first string to be compared.
  * @param string2
  *    The first string to be compared.
  * @returns
  *    a number between 0.0 and FSTRCMP_IDENTICAL.  The value 0.0 means
  *    the strings are utterly un-alike, and FSTRCMP_IDENTICAL means the
  *    strings are identical.  A value of FSTRCMP_ERROR indicates an
  *    error, either a malloc() failure or a mbstowcs() failure.
  */
double fstrcoll(const char *string1, const char *string2);

/**
  * The fstrcolli function may be used to make a fuzzy comparison
  * between two multi-byte-character strings.  Note that this function's
  * units of operation are characters, possibly multi-byte characters.
  *
  * The behavior of fstrcolli() depends on the LC_CTYPE category of the
  * current locale.
  *
  * @param string1
  *    The first string to be compared.
  * @param string2
  *    The first string to be compared.
  * @returns
  *    a number between 0 and FSTRCMPI_IDENTICAL.  The value 0 means
  *    the strings are utterly un-alike, and FSTRCMPI_IDENTICAL means the
  *    strings are identical.  A value of FSTRCMPI_ERROR indicates an
  *    error, either a malloc() failure or a mbstowcs() failure.
  */
int fstrcolli(const char *string1, const char *string2);

#ifdef __cplusplus
}
#endif

#endif /* FSTRCMP_H */
