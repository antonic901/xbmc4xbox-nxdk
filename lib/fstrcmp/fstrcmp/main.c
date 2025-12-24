/*
 * fstrcmp - fuzzy string compare library
 * Copyright (C) 2009, 2014 Peter Miller
 * Written by Peter Miller <pmiller@opensource.org.au>
 *
 * This program is free software; you can redistribute it and/or modify
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <lib/ac/fcntl.h>
#include <lib/ac/getopt.h> /* getopt_long */
#include <lib/ac/stdio.h>
#include <lib/ac/stdlib.h>
#include <lib/ac/string.h>
#include <lib/ac/sys/stat.h>
#include <lib/ac/sys/time.h>
#include <lib/ac/unistd.h> /* getopt */

#include <lib/fstrcmp.h>
#include <lib/program_name.h>
#include <lib/version_print.h>


static void
usage(void)
{
    const char *prog = fstrcmp_program_name_get();
    fprintf(stderr, "Usage: %s [ -p ] <string1> <string2>\n", prog);
    fprintf(stderr, "       %s -a <file1> <file2>\n", prog);
    fprintf(stderr, "       %s --version\n", prog);
    exit(1);
}


static const struct option options[] =
{
    { "case-pair", 0, 0, 'c' },
    { "case-pair-int", 0, 0, 'C' },
    { "files-as-bytes", 0, 0, 'a' },
    { "files-as-bytes-int", 0, 0, 'A' },
    { "pair", 0, 0, 'p' },
    { "pair-int", 0, 0, 'P' },
    { "select", 0, 0, 's' },
    { "select-int", 0, 0, 'S' },
    { "version", 0, 0, 'V' },
    { "wide-pair", 0, 0, 'w' },
    { "wide-pair-int", 0, 0, 'W' },
    { 0, 0, 0, 0 }
};


static void
fatal_malloc_error(void)
{
    perror("malloc");
    exit(1);
}


static void
pair(int argc, char **argv)
{
    double          result;

    if (argc != 2)
        usage();
    result = fstrcmp(argv[0], argv[1]);
    if (result < 0)
        fatal_malloc_error();
    printf("%6.4f\n", result);
}


static void
pair_int(int argc, char **argv)
{
    int             result;

    if (argc != 2)
        usage();
    result = fstrcmpi(argv[0], argv[1]);
    if (result < 0)
        fatal_malloc_error();
    printf("%d\n", result);
}


static void
wide_pair(int argc, char **argv)
{
    double          result;

    if (argc != 2)
        usage();
    result = fstrcoll(argv[0], argv[1]);
    if (result < 0)
        fatal_malloc_error();
    printf("%6.4f\n", result);
}


static void
wide_pair_int(int argc, char **argv)
{
    int             result;

    if (argc != 2)
        usage();
    result = fstrcolli(argv[0], argv[1]);
    if (result < 0)
        fatal_malloc_error();
    printf("%d\n", result);
}


static void
case_pair(int argc, char **argv)
{
    double          result;

    if (argc != 2)
        usage();
    result = fstrcasecmp(argv[0], argv[1]);
    if (result < 0)
        fatal_malloc_error();
    printf("%6.4f\n", result);
}


static void
case_pair_int(int argc, char **argv)
{
    int             result;

    if (argc != 2)
        usage();
    result = fstrcasecmpi(argv[0], argv[1]);
    if (result < 0)
        fatal_malloc_error();
    printf("%d\n", result);
}


static void
read_whole_file(const char *filename, void **data, size_t *size)
{
    int             fd;
    struct stat     st;
    ssize_t         nbytes;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror(filename);
        exit(1);
    }
    if (fstat(fd, &st) < 0)
    {
        perror(filename);
        exit(1);
    }
    if (!S_ISREG(st.st_mode))
    {
        fprintf(stderr, "%s: not a regular file\n", filename);
        exit(1);
    }
    *size = st.st_size;
    if (st.st_size == 0)
    {
        data = NULL;
        return;
    }
#ifdef HAVE_MMAP
    *data =
        mmap
        (
            NULL,        /* address hint */
            st.st_size,  /* length */
            PROT_READ,   /* prot */
            MAP_PRIVATE, /* flags */
            fd,
            0            /* offset */
        );
    if (addr)
    {
        close(fd);
        return;
    }
#endif
    *data = malloc(st.st_size);
    if (!*data)
    {
        perror("malloc");
        exit(1);
    }
    nbytes = read(fd, *data, st.st_size);
    if (nbytes < 0)
    {
        perror(filename);
        exit(1);
    }
    if (nbytes != st.st_size)
    {
        fprintf(stderr, "%s: changed size while being read\n", filename);
        exit(1);
    }
    close(fd);
}


static void
files_as_bytes(int argc, char **argv)
{
    void            *data1;
    size_t          size1;
    void            *data2;
    size_t          size2;
    double          result;

    if (argc != 2)
        usage();
    read_whole_file(argv[0], &data1, &size1);
    read_whole_file(argv[1], &data2, &size2);
    result = fmemcmp(data1, size1, data2, size2);
    if (result < 0)
        fatal_malloc_error();
    printf("%6.4f\n", result);
}


static void
files_as_bytes_int(int argc, char **argv)
{
    void            *data1;
    size_t          size1;
    void            *data2;
    size_t          size2;
    int             result;

    if (argc != 2)
        usage();
    read_whole_file(argv[0], &data1, &size1);
    read_whole_file(argv[1], &data2, &size2);
    result = fmemcmpi(data1, size1, data2, size2);
    if (result < 0)
        fatal_malloc_error();
    printf("%d\n", result);
}


static double
now(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
    {
        perror("gettimeofday");
        exit(1);
    }
    return (tv.tv_sec + 1.0e-6 * tv.tv_usec);
}


static void
selector(int argc, char **argv)
{
    const char      *needle;
    const char      *best_haystack;
    double          best_weight;
    int             j;
    double          begin_time;
    double          end_time;
    int             print_time;

    print_time = 0;
    if (argc > 0 && 0 == strcmp(argv[0], "-v"))
    {
        print_time = 1;
        --argc;
        ++argv;
    }
    if (argc < 1)
        usage();
    needle = argv[0];
    --argc;
    ++argv;

    if (argc == 2 && 0 == strcmp(argv[0], "-f"))
    {
        void *data = 0;
        size_t size = 0;
        size_t nlines;
        char *cp;
        char *end;
        char *bline;
        size_t n;

        read_whole_file(argv[1], &data, &size);

        nlines = 0;
        cp = data;
        end = cp + size;
        while (cp < end)
        {
            if (*cp++ == '\n')
                ++nlines;
        }
        argc = nlines;
        argv = malloc(nlines * sizeof(char *));
        if (!argv)
            fatal_malloc_error();
        n = 0;
        cp = data;
        bline = cp;
        while (cp < end)
        {
            if (*cp++ == '\n')
            {
                cp[-1] = '\0';
                argv[n++] = bline;
                bline = cp;
            }
        }
    }

    best_haystack = 0;
    best_weight = FSTRCMP_THRESHOLD;
    begin_time = now();
    for (j = 0; j < argc; ++j)
    {
        const char      *haystack;
        double          weight;

        haystack = argv[j];
        weight = fstrcmp(needle, haystack);
        if (weight < 0)
            fatal_malloc_error();
        if (weight > best_weight)
        {
            best_haystack = haystack;
            best_weight = weight;
        }
    }
    if (best_haystack)
        printf("%s\n", best_haystack);
    end_time = now();
    if (print_time && argc >= 1)
    {
        double          elapsed;

        elapsed = end_time - begin_time;
        printf("%0.9f = %5.3f sec / %d pair\n", elapsed / argc, elapsed, argc);
    }
}


static void
selector_int(int argc, char **argv)
{
    const char      *needle;
    const char      *best_haystack;
    int             best_weight;
    int             j;

    if (argc < 1)
        usage();
    needle = argv[0];
    best_haystack = 0;
    best_weight = FSTRCMPI_THRESHOLD;
    for (j = 1; j < argc; ++j)
    {
        const char      *haystack;
        int             weight;

        haystack = argv[j];
        weight = fstrcmpi(needle, haystack);
        if (weight < 0)
            fatal_malloc_error();
        if (weight > best_weight)
        {
            best_haystack = haystack;
            best_weight = weight;
        }
    }
    if (best_haystack)
        printf("%s\n", best_haystack);
}


int
main(int argc, char **argv)
{
    typedef void (*action_t)(int argc, char **argv);
    action_t action;

    action = pair;
    fstrcmp_program_name_set(argv[0]);
    for (;;)
    {
        int             c;

        c = getopt_long(argc, argv, "AaCcPpSsVWw", options, 0);
        if (c == -1)
            break;
        switch (c)
        {
        case 'A':
            action = files_as_bytes_int;
            break;

        case 'a':
            action = files_as_bytes;
            break;

        case 'C':
            action = case_pair_int;
            break;

        case 'c':
            action = case_pair;
            break;

        case 'P':
            action = pair_int;
            break;

        case 'p':
            action = pair;
            break;

        case 's':
            action = selector;
            break;

        case 'S':
            action = selector_int;
            break;

        case 'V':
            fstrcmp_version_print();
            return 0;

        case 'W':
            action = wide_pair_int;
            break;

        case 'w':
            action = wide_pair;
            break;

        default:
            usage();
        }
    }
    action(argc - optind, argv + optind);
    return 0;
}
