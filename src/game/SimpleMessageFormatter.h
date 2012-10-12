#ifndef SIMPLE_MESSAGE_FORMATTER_H
#define SIMPLE_MESSAGE_FORMATTER_H

#include <string>
#include <sstream>

#include "MessageFormatter.h"

#include "Player.h"
#include "Guild.h"
#include "Channel.h"

class SimpleMessageFormatter : public MessageFormatter {
public:
    SimpleMessageFormatter();

    virtual std::string formatSayMessage(Player const *who, std::string &msg);
    virtual std::string formatYellMessage(Player const *who, std::string &msg);
    virtual std::string formatEmoteMessage(Player const *who, std::string &msg);
    virtual std::string formatWhisperMessage(Player const *who, Player const *to, std::string &msg);
    virtual std::string formatGuildMessage(Player const *who, Guild const *guild, std::string &msg);
    virtual std::string formatOfficerMessage(Player const *who, Guild const *guild, std::string &msg);
    virtual std::string formatChannelMessage(Player const *who, Channel const *channel, std::string &msg);
    virtual std::string formatSetGuildMOTD(Player const *who, Guild const *guild, std::string &msg);

private:
protected:
};

#endif