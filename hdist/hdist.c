// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) Ivan Pizhenko, 2025. All rights reserved.
// See file LICENSE for more information.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION_STR "0.0.1"

#ifdef _MSC_VER
#define popcnt(x) __popcnt(x)
#define ALWAYS_INLINE __forceinline
#else
#define popcnt(x) __builtin_popcount(x)
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#endif

static ALWAYS_INLINE int hex_to_bin(char c)
{
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}

int main(int argc, char** argv)
{
  if (argc == 2) {
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
      printf("%s\n", VERSION_STR);
      return EXIT_SUCCESS;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
      printf("usage: %s [OPTION] [HEX1 HEX2]\n", argv[0]);
      puts(
          "\n"
          "Compute Hamming distance between two hexadecimal numbers of the arbitrary precision.\n"
          "\n"
          "HEX1 and HEX2 must be of the same length\n"
          "\n"
          "Options:\n"
          "  -h, --help       Print this help message and exit\n"
          "  -v, --version    Print version and exit\n"
      );
      return EXIT_SUCCESS;
    }
  }

  if (argc != 3) {
    fprintf(stderr, "error: %s arguments\nTry '%s --help' for more information\n", 
      argc < 3 ? "missing required" : "too many",
      argv[0]);
    exit(EXIT_FAILURE);
  }

  const size_t len = strlen(argv[1]);
  if (strlen(argv[2]) != len) {
    fputs("error: lengths of the arguments are different.\n", stderr);
    exit(EXIT_FAILURE);
  }

  unsigned long long d = 0;

  const char * const p1_end = argv[1] + len;
  for (const char *p1 = argv[1], *p2 = argv[2]; p1 != p1_end; ++p1, ++p2) {
    const int bin1 = hex_to_bin(*p1);
    const int bin2 = hex_to_bin(*p2);
    if (bin1 >= 0 && bin2 >=0) {
      d += popcnt(bin1 ^ bin2);
    } else {
      const int arg_no = bin1 < 0 ? 1 : 2;
      fprintf(stderr, "error: non-hex character at position %td in the argument %d\n", 
        (arg_no == 1 ? p1 - argv[1] : p2 - argv[2]) + 1,
        arg_no);
      exit(EXIT_FAILURE);
    }
  }

  printf("%llu\n", d);
  return EXIT_SUCCESS;
}
