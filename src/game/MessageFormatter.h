#ifndef MESSAGE_FORMATTER_H
#define MESSAGE_FORMATTER_H

#include <string>
#include "Player.h"
#include "Guild.h"
#include "Channel.h"

class MessageFormatter
{
public:
    virtual std::string formatSayMessage(Player const *who, std::string &msg) = 0;
    virtual std::string formatYellMessage(Player const *who, std::string &msg) = 0;
    virtual std::string formatEmoteMessage(Player const *who, std::string &msg) = 0;
    virtual std::string formatWhisperMessage(Player const *who, Player const *to, std::string &msg) = 0;
    virtual std::string formatGuildMessage(Player const *who, Guild const *guild, std::string &msg) = 0;
    virtual std::string formatOfficerMessage(Player const *who, Guild const *guild, std::string &msg) = 0;
    virtual std::string formatChannelMessage(Player const *who, Channel const *channel, std::string &msg) = 0;
    virtual std::string formatSetGuildMOTD(Player const *who, Guild const *guild, std::string &msg) = 0;

    virtual ~MessageFormatter() {}
private:
protected:
};

#endif
