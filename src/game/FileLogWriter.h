#ifndef FILE_LOG_WRITER_H
#define FILE_LOG_WRITER_H

#include "LogWriter.h"
#include "stdio.h"

class FileLogWriter : public LogWriter {
public:
    FileLogWriter();
    ~FileLogWriter();
    virtual void writeMessage(const std::string &msg);
private:
    FILE *file;
protected:
};

#endif
