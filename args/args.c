// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) Ivan Pizhenko, 2024-2025. All rights reserved.
// See file LICENSE for more information.

#include <stdio.h>

int main(int argc, char **argv)
{
  if (argc > 1) { 
    char buffer[32];
    const int n = snprintf(buffer, sizeof(buffer), "%d", argc);
    snprintf(buffer, sizeof(buffer), "[%%.0%dd] %%s\n", n);
    for (int i = 1; i < argc; ++i) {
      printf(buffer, i, argv[i]);
    }
  }
  return 0;
}
