#pragma once

#if defined(WIN64) || defined(_WIN64)
  #define PLATFORM_WIN
  #define PLATFORM_WIN64
  #define PLATFORM_NAME "win64"
#elif defined(__APPLE__)
  #define PLATFORM_UNIX
  #define PLATFORM_MACOSX
  #define PLATFORM_NAME "macosx"
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
  #define PLATFORM_UNIX
  #define PLATFORM_XBSD
  #define PLATFORM_NAME "freebsd"
#elif defined(__HAIKU__) || defined(HAIKU)
  #define PLATFORM_BEOS
  #define PLATFORM_HAIKU
  #define PLATFORM_NAME "haiku"
#else
  #define PLATFORM_UNIX
  #define PLATFORM_LINUX
  #define PLATFORM_NAME "linux"
#endif