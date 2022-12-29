#pragma once

#include <string>
#include <vector>
#include "format.hpp"

class LogWriter
{
public:
  enum Severity {
    debug=0,
    info,
    warn,
    error,
    fatal
  };

  static const char* severity(LogWriter::Severity s);
  virtual void write(const std::string&, bool newLine) = 0;
  virtual bool isActive() const = 0;
};

typedef std::shared_ptr<LogWriter> LogWriterPtr;

class SimpleLogger
{
public:
  SimpleLogger(const std::string& category);

  bool isDebugEnabled() const;

#define DECL_LOGFUNC(name,severity) template<typename... Args> \
  void name(const std::string& f, const Args & ... args) { llog( severity, fmt::format( f,args... )); } \
  void name(const std::string& text) { llog( severity, text ); }

  DECL_LOGFUNC(warn,  LogWriter::warn)
  DECL_LOGFUNC(info,  LogWriter::info)
  DECL_LOGFUNC(debug, LogWriter::debug)
  DECL_LOGFUNC(error, LogWriter::error)
  DECL_LOGFUNC(fatal, LogWriter::fatal)
#undef DECL_LOGFUNC

  template <typename... Args>
  void log(LogWriter::Severity severity, const std::string& format, const Args & ... args) { llog(severity, fmt::format(format, args...)); }

private:
  SimpleLogger() {}
  void write(const std::string &message, bool newline = true);

  std::string _category;

  void llog(LogWriter::Severity, const std::string &text);
};

class Logger
{
public:
  typedef enum {consolelog=0, filelog, count} Type;

#define DECL_LOGFUNC(severity,name) template<typename... Args> \
  static void name(const std::string& f, const Args & ... args) { _print(severity, fmt::format( f,args... )); } \
  static void name(const std::string& text) { _print( severity, text ); }

  DECL_LOGFUNC(LogWriter::info,  info)
  DECL_LOGFUNC(LogWriter::warn,  warning)
  DECL_LOGFUNC(LogWriter::debug, debug)
  DECL_LOGFUNC(LogWriter::error, error)
  DECL_LOGFUNC(LogWriter::fatal, fatal)
#undef DECL_LOGFUNC

  static void warningIf(bool warn, const std::string& text);
  static void update(const std::string& text, bool newline = false);

  static void addFilter(const std::string& text);
  static void addFilter(LogWriter::Severity s);
  static bool hasFilter(const std::string& text);
  static void removeFilter(const std::string& text);

  static void registerWriter(Type type, const std::string &param);
  static void registerWriter(const std::string& name, LogWriterPtr writer);

private:
  static void _print(LogWriter::Severity s,const std::string& text);
  void _write(LogWriter::Severity s, const std::string& message, bool newline = true) { _write(LogWriter::severity(s) + message, newline); }
  void _write(const std::string& message, bool newline = true);


  typedef std::map<std::string,LogWriterPtr> Writers;
  typedef std::vector<std::string> Filters;

  Filters _filters;
  Writers _writers;
};
