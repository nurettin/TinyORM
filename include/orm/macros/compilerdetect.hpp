#pragma once
#ifndef ORM_MACROS_COMPILERDETECT_HPP
#define ORM_MACROS_COMPILERDETECT_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

// Used compiler
// Must be before GNU, because clang claims to be GNU too
#if defined(__clang__)
// Apple clang has other version numbers
#  ifdef __apple_build_version__
#    define TINYORM_COMPILER_STRING "Clang " __clang_version__ " (Apple)"
#  else
// Clang-cl simulating MSVC
#    if defined(_MSC_VER)
#      define TINYORM_COMPILER_STRING "Clang-cl " __clang_version__

#      if _MSC_VER < 1910
#        define TINYORM_SIMULATED_STRING "MSVC 2015 (" TINYORM_STRINGIFY(_MSC_VER) ")"
#      elif _MSC_VER < 1917
#        define TINYORM_SIMULATED_STRING "MSVC 2017 (" TINYORM_STRINGIFY(_MSC_VER) ")"
#      elif _MSC_VER < 1930
#        define TINYORM_SIMULATED_STRING "MSVC 2019 (" TINYORM_STRINGIFY(_MSC_VER) ")"
#      elif _MSC_VER < 2000
#        define TINYORM_SIMULATED_STRING "MSVC 2022 (" TINYORM_STRINGIFY(_MSC_VER) ")"
#      else
#        define TINYORM_SIMULATED_STRING "MSVC _MSC_VER " TINYORM_STRINGIFY(_MSC_VER)
#      endif
// Normal Clang
#    else
#      define TINYORM_COMPILER_STRING "Clang " __clang_version__
#    endif
#  endif
#elif defined(__GNUC__)
#  define TINYORM_COMPILER_STRING "GCC " __VERSION__
#elif defined(_MSC_VER)
#  if _MSC_VER < 1910
#    define TINYORM_COMPILER_STRING "MSVC 2015 (" TINYORM_STRINGIFY(_MSC_VER) ")"
#  elif _MSC_VER < 1917
#    define TINYORM_COMPILER_STRING "MSVC 2017 (" TINYORM_STRINGIFY(_MSC_VER) ")"
#  elif _MSC_VER < 1930
#    define TINYORM_COMPILER_STRING "MSVC 2019 (" TINYORM_STRINGIFY(_MSC_VER) ")"
#  elif _MSC_VER < 2000
#    define TINYORM_COMPILER_STRING "MSVC 2022 (" TINYORM_STRINGIFY(_MSC_VER) ")"
#  else
#    define TINYORM_COMPILER_STRING "MSVC _MSC_VER " TINYORM_STRINGIFY(_MSC_VER)
#  endif
#else
#  define TINYORM_COMPILER_STRING "<unknown compiler>"
#endif

#endif // ORM_MACROS_COMPILERDETECT_HPP
