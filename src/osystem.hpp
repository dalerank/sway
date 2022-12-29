#pragma once

#include <string>
#include <vector>

class OSystem
{
public:
  typedef enum { unknown, windows, win64, unix, linux, macos, bsd, haiku } Type;
  static void error( const std::string& title, const std::string& text );
  static void openUrl(const std::string& url , const std::string& prefix="");
  static void openDir(const std::string& path , const std::string &prefix="");
  static int  gmtOffsetMs();
  static bool is( Type type );

  static bool isLinux();
  static bool isUnix();
  static bool isMac();
  static bool isWindows();
};
