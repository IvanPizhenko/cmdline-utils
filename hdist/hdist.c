// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) Ivan Pizhenko, 2025. All rights reserved.
// See file LICENSE for more information.

// CRT
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// OS common
#include <fcntl.h>
#include <sys/stat.h>

// OS specific
#ifdef _MSC_VER
#include <io.h>
typedef int read_result_type;
#define O_RDONLY _O_RDONLY
#else
#include <unistd.h>
typedef ssize_t read_result_type;
#endif

#ifdef _MSC_VER
#define popcount(x) __popcnt(x)
#define ALWAYS_INLINE __forceinline
#define NORETURN __declspec(noreturn) void
#else
#define popcount(x) __builtin_popcount(x)
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#define NORETURN __attribute__((noreturn)) void
#endif

#ifndef static_assert
#define static_assert _Static_assert
#endif

#define HDIST_PROGRAM_NAME "hdist"
#define HDIST_VERSION_STR "0.1.0"

static ALWAYS_INLINE int hex_to_bin(char c)
{
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}

static unsigned long long hdist_hex_numbers(const char *hex1, const char *hex2)
{
  const size_t len = strlen(hex1);
  if (strlen(hex2) != len) {
    fputs("error: lengths of the hex strings are different\n", stderr);
    exit(EXIT_FAILURE);
  }

  unsigned long long d = 0;
  const char *p1 = hex1;
  const char * const hex1_end = hex1 + len;
  const char *p2 = hex2;
  while (p1 != hex1_end) {
    const int bin1 = hex_to_bin(*p1++);
    const int bin2 = hex_to_bin(*p2++);
    if (bin1 >= 0 && bin2 >=0) {
      d += popcount(bin1 ^ bin2);
    } else {
      const int arg_no = bin1 < 0 ? 1 : 2;
      fprintf(stderr, "error: non-hex character at position %td in the argument %d\n",
        (arg_no == 1 ? p1 - hex1 : p2 - hex2) + 1,
        arg_no);
      exit(EXIT_FAILURE);
    }
  }
  return d;
}

#define INPUT_BUFFER_SIZE 4096

static unsigned long long hdist_files(const char *f1, const char *f2)
{
  int fd2 = -1;
  bool err = false;

  const int fd1 = open(f1, O_RDONLY);
  if (fd1 < 0) {
    err = true;
    fprintf(stderr, "error: can't open file %s: %s\n", f1, strerror(errno));
    goto cleanup;
  }

  struct stat st1;
  if (fstat(fd1, &st1) < 0) {
    err = true;
    fprintf(stderr, "error: can't stat file %s: %s\n", f1, strerror(errno));
    goto cleanup;
  }

  fd2 = open(f2, O_RDONLY);
  if (fd2 < 0) {
    err = true;
    fprintf(stderr, "error: can't open file %s: %s\n", f2, strerror(errno));
    goto cleanup;
  }

  struct stat st2;
  if (fstat(fd2, &st2) < 0) {
    err = true;
    fprintf(stderr, "error: can't stat file %s: %s\n", f2, strerror(errno));
    goto cleanup;
  }

  if (st1.st_size != st2.st_size) {
    err = true;
    fputs("error: lengths of the files are different\n", stderr);
    goto cleanup;
  }

  unsigned long long d = 0;
  size_t r = st1.st_size;
  //printf("fsize=%zu\n", r);

  while (r > 0) {
    unsigned char buffer1[INPUT_BUFFER_SIZE];
    unsigned char buffer2[INPUT_BUFFER_SIZE];
    static_assert(sizeof(buffer1) == sizeof(buffer2), "Buffers are not of the same size");

    const size_t m = r < sizeof(buffer1) ? r : sizeof(buffer1);
    //printf("about to read %d bytes\n", (int)m);

    size_t l = m;
    while (l > 0) {
      const size_t rn = l < sizeof(buffer1) ? l : sizeof(buffer1);
      const read_result_type n = read(fd1, buffer1, rn);
      //printf("1: read %d bytes\n", (int)n);
      if (n < 0) {
        err = true;
        fprintf(stderr, "error: can't read from file %s: %s\n", f1, strerror(errno));
        goto cleanup;
      }
      l -= n;
    }

    l = m;
    while (l > 0) {
      const size_t rn = l < sizeof(buffer2) ? l : sizeof(buffer2);
      const read_result_type n = read(fd2, buffer2, rn);
      //printf("2: read %d bytes\n", (int)n);
      if (n < 0) {
        err = true;
        fprintf(stderr, "error: can't read from file %s: %s\n", f2, strerror(errno));
        goto cleanup;
      }
      l -= n;
    }

    const unsigned char *p1 = buffer1;
    const unsigned char *p2 = buffer2;
    const unsigned char * const p1_end = p1 + m;
    while (p1 != p1_end) {
      d += popcount((*p1++) ^ (*p2++));
    }

    r -= m;
  }

cleanup:
  if (fd2 >= 0) close(fd2);
  if (fd1 >= 0) close(fd1);
  if (err) exit(EXIT_FAILURE);

  return d;
}

static NORETURN incorrect_args(const char* argv0, int n)
{
  fprintf(stderr, "error: %s arguments\nTry '%s --help' for more information\n",
    n < 2 ? "missing required" : "too many",
    argv0);
  exit(EXIT_FAILURE);
}

static void usage(void)
{
  puts(
    "Usage: " HDIST_PROGRAM_NAME " [OPTIONS] [FILE1 FILE2 | HEX1 HEX2]\n"
    "Compute Hamming distance between contents of two files\n"
    "or two hexadecimal numbers of the arbitrary length.\n"
    "\n"
    "FILE1 and FILE2 must be of the same size.\n"
    "HEX1 and HEX2 must be of the same length.\n"
    "\n"
    "  -f, --file       Compare files (default mode)\n"
    "  -n, --number     Compare hexadecimal numbers\n"
    "  -h, --help       Print this help message and exit\n"
    "  -v, --version    Print version and exit\n"
  );
}

static bool is_file_mode = true;

int main(int argc, char **argv)
{
  if (argc < 2) incorrect_args(argv[0], 0);

  if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
    printf("%s %s\n", HDIST_PROGRAM_NAME, HDIST_VERSION_STR);
    return EXIT_SUCCESS;
  }

  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    usage();
    return EXIT_SUCCESS;
  }

  int first_arg_index = 1;

  if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--file") == 0) {
      is_file_mode = true;
      ++first_arg_index;
  } else if (strcmp(argv[1], "-n") == 0 || strcmp(argv[1], "--number") == 0) {
      is_file_mode = false;
      ++first_arg_index;
  }

  const int nargs = argc - first_arg_index;
  if (nargs != 2) incorrect_args(argv[0], nargs);

  const unsigned long long d = is_file_mode
    ? hdist_files(argv[first_arg_index], argv[first_arg_index + 1])
    : hdist_hex_numbers(argv[first_arg_index], argv[first_arg_index + 1]);
  printf("%llu\n", d);

  return EXIT_SUCCESS;
}
