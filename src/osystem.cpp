#include "osystem.hpp"
#include "platform.hpp"
#include "logger.hpp"
#include <fstream>
#include <ctime>

#if defined(PLATFORM_UNIX) || defined(PLATFORM_HAIKU)
  #include <limits.h>
  #include <unistd.h>
  #include <sys/stat.h>
#endif

#ifdef PLATFORM_LINUX
#include <cstdlib>
#include <string.h>
const char* getDialogCommand()
{
  if (::system(NULL))
  {
    if(::system("which gdialog") == 0)
      return "gdialog";

    else if (::system("which kdialog") == 0)
      return "kdialog";
  }
  return NULL;
}
#elif defined(PLATFORM_MACOSX)
  #include <cstdlib>
#elif defined(PLATFORM_WIN)
  #include <windows.h>
#endif

void OSystem::error(const std::string& title, const std::string& text)
{
#if defined(PLATFORM_LINUX)
  const char * dialogCommand = getDialogCommand();
  if (dialogCommand)
  {
    std::string command = dialogCommand;
    command += " --title \"" + title + "\" --msgbox \"" + text + "\"";
    int syserror = ::system(command.c_str());
    if( syserror )
    {
      Logger::error( "WARNING: Cant execute command " + command );
    }
  }

  // fail-safe method here, using stdio perhaps, depends on your application
#elif defined(PLATFORM_WIN)
  MessageBox(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
#endif
}

void OSystem::openUrl(const std::string& url, const std::string& prefix)
{
#ifdef PLATFORM_LINUX
  std::string command = prefix + "xdg-open '" + url + "'";
  Logger::info( command );
  auto result = ::system( command.c_str() );
  result;
#elif defined(GAME_PLATFORM_WIN)
  ShellExecuteA(0, "Open", url.c_str(), 0, 0 , SW_SHOW );
#elif defined(GAME_PLATFORM_MACOSX)
  std::string command = "open \"" + url + "\" &";
  auto result = ::system(command.c_str());
  result;
#endif
}

void OSystem::openDir(const std::string& path, const std::string& prefix)
{
  std::string command;
#ifdef PLATFORM_LINUX
  command = prefix + "nautilus '" + path + "' &";
  auto result = ::system(command.c_str() );
  result;
#elif defined(PLATFORM_WIN)
  ShellExecute(GetDesktopWindow(), "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(PLATFORM_MACOSX)
  command = "open \"" + path + "\" &";
  auto result = ::system(command.c_str());
  result;
#endif
}

int OSystem::gmtOffsetMs()
{
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOSX)
  std::time_t current_time;
  std::time(&current_time);
  struct std::tm *timeinfo = std::localtime(&current_time);
  return timeinfo->tm_gmtoff;
#elif defined(PLATFORM_WIN)
  time_t now = time(NULL);
  struct tm lcl = *localtime(&now);
  struct tm gmt = *gmtime(&now);
  return (lcl.tm_hour - gmt.tm_hour);
#endif
}

bool OSystem::is(OSystem::Type type)
{
  switch( type )
  {
  case windows:
#ifdef PLATFORM_WIN
    return true;
#endif
  case win64:
#ifdef PLATFORM_WIN64
    return true;
#endif
  case linux:
#ifdef PLATFORM_LINUX
    return true;
#endif
  case unix:
#ifdef PLATFORM_UNIX
    return true;
#endif
  break;
  break;
  case macos:
#ifdef PLATFORM_MACOSX
    return true;
#endif
  break;
  case bsd:
#ifdef PLATFORM_XBSD
    return true;
#endif
  break;
  case haiku:
#ifdef PLATFORM_HAIKU
    return true;
#endif
  break;
  }

  return false;
}

bool OSystem::isLinux() { return is( linux ); }
bool OSystem::isUnix() { return is( unix ); }
bool OSystem::isMac() { return is( macos ); }
bool OSystem::isWindows() { return is( windows ); }
