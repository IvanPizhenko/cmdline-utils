// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) Ivan Pizhenko, 2024-2025. All rights reserved.
// See file LICENSE for more information.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARGS_PROGRAM_NAME "args"
#define ARGS_VERSION_STR "0.0.1"

int main(int argc, char **argv)
{
  if (argc == 1) return 0;

  if (argc == 2) {
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
      printf("%s %s\n", ARGS_PROGRAM_NAME, ARGS_VERSION_STR);
      exit(EXIT_SUCCESS);
    }
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
      printf("Usage: %s [OPTION] [ARGS]...\n", ARGS_PROGRAM_NAME);
      puts(
        "Print ARGS one per line with argument number.\n"
        "\n"
        "  -h, --help       display this help and exit\n"
        "  -v, --version    output version information and exit\n"
      );
      exit(EXIT_SUCCESS);
    }
  }

  char buffer[32];
  const int n = snprintf(buffer, sizeof(buffer), "%d", argc);
  snprintf(buffer, sizeof(buffer), "[%%.0%dd] %%s\n", n);
  for (int i = 1; i < argc; ++i) {
    printf(buffer, i, argv[i]);
  }

  return 0;
}
