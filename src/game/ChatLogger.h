#ifndef CHAT_LOGGER_H
#define CHAT_LOGGER_H

#include "Common.h"
#include "LogWriter.h"
#include "MessageFormatter.h"

#include "FileLogWriter.h"
#include "SimpleMessageFormatter.h"

#include "Policies/Singleton.h"
#include "DelayExecutor.h"

#include "Player.h"
#include "Guild.h"
#include "Channel.h"

#include <ace/Method_Request.h>
#include <ace/Thread_Mutex.h>

enum Writers {
    FILE_LOG_WRITER = 0,
};

enum Formatters {
    SIMPLE_FORMATTER = 0,
};


class ChatLoggerRequest : public ACE_Method_Request {
    
public:
    ChatLoggerRequest(LogWriter *logWriter, std::string &msg);
    virtual int call();
private:
    LogWriter *logWriter;
    std::string msg;
};


class ChatLogger : public MaNGOS::Singleton< ChatLogger, MaNGOS::ClassLevelLockable<ChatLogger, ACE_Thread_Mutex> > {
    friend class MaNGOS::OperatorNew<ChatLogger>;
public:

    void init();

    void logSayMessage(Player const *who, std::string &msg);
    void logYellMessage(Player const *who, std::string &msg);
    void logEmoteMessage(Player const *who, std::string &msg);
    void logWhisperMessage(Player const *who, Player const *to, std::string &msg);
    void logGuildMessage(Player const *who, Guild const *guild, std::string &msg);
    void logOfficerMessage(Player const *who, Guild const *guild, std::string &msg);
    void logChannelMessage(Player const *who, Channel const *channel, std::string &msg);
    void logSetGuildMOTD(Player const *who, Guild const *guild, std::string &msg);

private:
    ChatLogger();
    ~ChatLogger();
    LogWriter *writer;
    MessageFormatter *formatter;
    DelayExecutor executor;
    ACE_Thread_Mutex mutex;

    int writeMessage(std::string &msg);

protected:
};

#define sChatLogger MaNGOS::Singleton<ChatLogger>::Instance()

#endif
