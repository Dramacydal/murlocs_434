#include "ChatLogger.h"
#include "Policies/Singleton.h"
#include "Config/Config.h"
#include <ace/Log_Msg.h>
#include <ace/Guard_T.h>

INSTANTIATE_SINGLETON_1( ChatLogger );


ChatLogger::ChatLogger() : executor(), mutex() {
    formatter = NULL;
    writer  = NULL;
    init();
}

ChatLogger::~ChatLogger() {
    executor.deactivate();
    delete formatter;
    delete writer;
    formatter = NULL;
    writer = NULL;
}

void ChatLogger::init() {
    // ToDo: Init with the data from Config file.
    bool enabled = sConfig.GetBoolDefault("ChatLogger.Enable", false);
    if (!enabled) {
        return;
    }
    int32 writerType = sConfig.GetBoolDefault("ChatLogger.WriterType", 0);
    switch (writerType) {
        case FILE_LOG_WRITER:
            writer = new FileLogWriter();
            break;
        default:
            break;
    }
    int32 formatterType = sConfig.GetBoolDefault("ChatLogger.FormatterType", 0);
    switch (formatterType) {
        case SIMPLE_FORMATTER:
            formatter = new SimpleMessageFormatter();
            break;
        default:
        break;
    }   
    executor.activate(1, NULL, NULL);
}

void ChatLogger::logSayMessage(Player const *who, std::string &msg) {
    if (formatter == NULL || writer == NULL) {
        return;
    }
    std::string send = formatter->formatSayMessage(who, msg);
    writeMessage(send);
}

void ChatLogger::logYellMessage(Player const *who, std::string &msg) {
    if (formatter == NULL || writer == NULL) {
        return;
    }
    std::string send = formatter->formatYellMessage(who, msg);
    writeMessage(send);
}

void ChatLogger::logEmoteMessage(Player const *who, std::string &msg) {
    if (formatter == NULL || writer == NULL) {
        return;
    }
    std::string send = formatter->formatEmoteMessage(who, msg);
    writeMessage(send);
}

void ChatLogger::logWhisperMessage(Player const *who, Player const *to, std::string &msg) {
    if (formatter == NULL || writer == NULL) {
        return;
    }
    std::string send = formatter->formatWhisperMessage(who, to, msg);
    writeMessage(send);
}

void ChatLogger::logGuildMessage(Player const *who, Guild const *guild, std::string &msg) {
    if (formatter == NULL || writer == NULL) {
        return;
    }
    std::string send = formatter->formatGuildMessage(who, guild, msg);
    writeMessage(send);
}

void ChatLogger::logOfficerMessage(Player const *who, Guild const *guild, std::string &msg) {
    if (formatter == NULL || writer == NULL) {
        return;
    }
    std::string send = formatter->formatOfficerMessage(who, guild, msg);
    writeMessage(send);
}

void ChatLogger::logChannelMessage(Player const *who, Channel const *channel, std::string &msg) {
    if (formatter == NULL || writer == NULL) {
        return;
    }
    std::string send = formatter->formatChannelMessage(who, channel, msg);
    writeMessage(send);
}

void ChatLogger::logSetGuildMOTD(Player const *who, Guild const *guild, std::string &msg) {
    if (formatter == NULL || writer == NULL) {
        return;
    }
    std::string send = formatter->formatSetGuildMOTD(who, guild, msg);
    writeMessage(send);
}

int ChatLogger::writeMessage(std::string &msg) {

    ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, mutex, -1);

    if (executor.execute(new ChatLoggerRequest(writer, msg)) == -1) {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) \n"), ACE_TEXT("Failed to write chat log message.")));
        return -1;
    }
    return 0;
}


ChatLoggerRequest::ChatLoggerRequest(LogWriter *logWriter, std::string &msg) {
    this->logWriter = logWriter;
    this->msg = msg;
}

int ChatLoggerRequest::call() {
    if (logWriter != NULL) {
        logWriter->writeMessage(msg);
    }
    return 0;
}