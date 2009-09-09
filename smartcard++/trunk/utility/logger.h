/*!
	\file		logger.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-04-17 16:19:22 +0300 (Fri, 17 Apr 2009) $
*/
// Revision $Revision: 255 $

#ifndef __LOGGER_H__
#define __LOGGER_H__

enum logTarget {
	log_to_CONSOLE,
	log_to_FILE,
	log_to_SYSTEMLOG,
	log_to_WINDOW,
};
enum logPrio {
      log_DEBUG,
      log_WARNING,
      log_CRITICAL
};

class log_target {
public:
  log_target(const std::string &) {}
  virtual void writeLine(const std::string &line,logPrio prio = log_DEBUG) = 0;
};

class  log_streambuffer : public std::basic_streambuf<char, std::char_traits<char> > {
    std::vector<char> m_inputbuffer;
    typedef std::char_traits<char> _Tr;
protected:
     virtual int overflow(int = _Tr::eof());
public:
    std::vector<log_target *> targets;
    log_streambuffer();
    ~log_streambuffer();
};

class logger : public std::ostream {
	const std::string m_name;
	log_streambuffer m_streambuffer;
public:
	logger(const std::string name = "cardlib_utility");
	void addTarget(logTarget target);
	logger(const std::string name,logTarget target);
  };

class ScopedLog {
	const ScopedLog &operator=(const ScopedLog &);
    logger &m_log;
    const char *fn;
public:
    ScopedLog(const char *func_,logger & log) : m_log(log),fn(func_) {
      m_log << "-->function:" << fn << std::endl;
      }
    ~ScopedLog() {
      m_log << "<--function:" << fn << std::endl;
  }
};
#endif
