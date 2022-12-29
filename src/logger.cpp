#include "logger.hpp"

#include <map>
#include <iostream>
#include <filesystem>
#include "format.hpp"

const char* LogWriter::severity(LogWriter::Severity s)
{
  switch (s) {
  case LogWriter::debug: return "[DEBUG]";
  case LogWriter::info:  return "[INFO]";
  case LogWriter::warn:  return "[WARN]";
  case LogWriter::error: return "[ERROR]";
  case LogWriter::fatal: return "[FATAL]";
  }
  return "[UNKNOWN]";
}

class FileLogWriter : public LogWriter
{
private:
  FILE* _logFile;

public:
  FileLogWriter(const std::string& path)
  {
    time_t now = time(0);
    tm *gmtm = gmtime(&now);

    _logFile = fopen(path.c_str(), "w");

    if( _logFile )
    {
      fputs("Sway logfile created: ", _logFile);
      fputs( fmt::format("%02d:%02d:%02d", gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec).c_str(), _logFile);
      fputs("\n", _logFile);
    }
  }

  ~FileLogWriter()
  {
    time_t now = time(0);
    tm *gmtm = gmtime(&now);

    if( _logFile )
    {
      fputs("Sway logfile closed: ", _logFile);
      fputs( fmt::format("%02d:%02d:%02d", gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec).c_str(), _logFile);
      fputs("\n", _logFile);

      fflush(_logFile);
    }
  }

  virtual bool isActive() const { return _logFile != 0; }

  virtual void write( const std::string& str, bool )
  {
    // Don't write progress stuff into the logfile
    // Make sure only one thread is writing to the file at a time
    static int count = 0;
    if( _logFile )
    {
      fputs(str.c_str(), _logFile);
      fputs("\n", _logFile);

      count++;
      if( count % 10 == 0 )
      {
        fflush(_logFile);
      }
    }
  }
};

class ConsoleLogWriter : public LogWriter
{
public:
  virtual void write( const std::string& str, bool newline )
  {
    std::cout << str;
    if( newline ) std::cout << std::endl;
    else std::cout << std::flush;
  }

  virtual bool isActive() const { return true; }
};

static Logger _logger;

void Logger::_write(const std::string& message, bool newline)   {
    for (auto& filter : _filters)       {
        if (message.compare(0, filter.length(), filter) == 0)
            return;
    }

    for (auto& item : _writers)       {
        if (!!item.second)           {
            item.second->write(message, newline);
        }
    }
}

void Logger::warningIf(bool warn, const std::string& text){  if (warn) warning( text ); }
void Logger::update(const std::string& text, bool newline){  _logger._write( text, newline ); }
void Logger::_print(LogWriter::Severity s,const std::string& str ) {  _logger._write(s,str); }

void Logger::addFilter(const std::string& text)
{
  if (hasFilter(text))
    return;

  _logger._filters.push_back(text);
}

void Logger::addFilter(LogWriter::Severity s)
{
    _logger._filters.push_back(LogWriter::severity(s));
}

bool Logger::hasFilter(const std::string& text)
{
  for( auto& filter : _logger._filters)
  {
    if (filter == text) return true;
  }
  return false;
}

void Logger::removeFilter(const std::string& text)
{
    auto it = std::find(_logger._filters.begin(), _logger._filters.end(), text);
    if (it != _logger._filters.end()) _logger._filters.erase(it);
}

void Logger::registerWriter(Logger::Type type, const std::string& param )
{
  switch( type )
  {
  case consolelog:
  {
    auto wr = std::make_shared<ConsoleLogWriter>();
    registerWriter( "__console", wr );
  }
  break;

  case filelog:
  {
    std::filesystem::path fn = std::filesystem::current_path() / param / "stdout.txt";
    auto wr = std::make_shared<FileLogWriter>( fn.string() );
    registerWriter( "__log", wr );
  }
  break;

  case count: break;
  }
}

void Logger::registerWriter(const std::string& name, LogWriterPtr writer)
{
  if( !!writer && writer->isActive() )
  {
    _logger._writers[ name ] = writer;
  }
}

void SimpleLogger::write(const std::string &message, bool newline) {
  Logger::update( message, newline );
}

SimpleLogger::SimpleLogger( const std::string& category)
  : _category(category)
{}

void SimpleLogger::llog(LogWriter::Severity s, const std::string &text)
{
  write(fmt::format("{} {}: {}", LogWriter::severity(s), _category, text));
}

bool SimpleLogger::isDebugEnabled() const {
#ifdef DEBUG
  return true;
#else
  return false;
#endif
}
