//===--- config.h - Manages  Command Line Configuration ----------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __config_h
#define __config_h

#include "gl.h"

#include <string.h>
#include <assert.h>

// This is a really really simple class to make it easy for me to pass config options into
// the program from the command line. Maybe I'll expand it later. It's not very fast and that's OK.
//
// If a config option is going to be used in a tight loop, it should be cached.
struct Config {
  static int argc;
  static char **argv;

  static void init(int argc, char **argv) {
    Config::argc = argc;
    Config::argv = argv;
  }

  static bool boolArg(const char *name) {
    for (int i=0; i<argc; i++) {
      if (argv[i][0] == '-' && strcmp(argv[i] + 1, name) == 0) {
        return true;
      }
    }

    return false;
  }

  static char *strArg(const char *name) {
    for (int i=0; i<argc; i++) {
      if (argv[i][0] == '-' && strcmp(argv[i] + 1, name) == 0) {
        assert(i + 1 < argc);
        return argv[i + 1];
      }
    }

    return NULL;
  }
};


#endif // defined(__config_h)
