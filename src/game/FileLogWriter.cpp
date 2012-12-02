#include "FileLogWriter.h"
#include "Log.h"
#include "Config/Config.h"

FileLogWriter::FileLogWriter() {
    std::string fileName = sConfig.GetStringDefault("ChatLogger.FileLogWriter.FileName", "chatlog.log");
    file = fopen(fileName.c_str(), "a+");
    if (file == NULL) {
        ERROR_LOG( "FileLogWriter can't open log file %s.", fileName.c_str() );
    }
}

void FileLogWriter::writeMessage(const std::string &msg) {
    if (file == NULL) {
        return;
    }
    fprintf(file, "%s\n", msg.c_str());
    fflush(file);
}

FileLogWriter::~FileLogWriter() {
    if (file != NULL) {
        fclose(file);
    }
    file = NULL;    
}
