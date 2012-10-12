#ifndef LOG_WRITER_H
#define LOG_WRITER_H

#include <string>

class LogWriter {
public:
    virtual void writeMessage(const std::string &msg) = 0;
    virtual ~LogWriter() {}
private:

protected:
};

#endif
