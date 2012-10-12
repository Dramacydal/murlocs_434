#include "SimpleMessageFormatter.h"

/*

Simple ChatLog message formatter. Common format is:
[TYPE][DATA1]...[DATAN]Message

*/
SimpleMessageFormatter::SimpleMessageFormatter() {
}

// [SAY][PlayerName]Message
std::string SimpleMessageFormatter::formatSayMessage(Player const *who, std::string &msg) {
    std::ostringstream result;
    result << "[SAY]";
    result << "[" << who->GetName() << "]";
    result << msg;
    return result.str();
}

// [YELL][PlayerName]Message
std::string SimpleMessageFormatter::formatYellMessage(Player const *who, std::string &msg) {
    std::ostringstream result;
    result << "[YELL]";
    result << "[" << who->GetName() << "]";
    result << msg;
    return result.str();
}

// [EMOTE][PlayerName]Message
std::string SimpleMessageFormatter::formatEmoteMessage(Player const *who, std::string &msg) {
    std::ostringstream result;
    result << "[EMOTE]";
    result << "[" << who->GetName() << "]";
    result << msg;
    return result.str();
}

// [WHISPER][PlayerNameWho][PlayerNameTo]Message
std::string SimpleMessageFormatter::formatWhisperMessage(Player const *who, Player const *to, std::string &msg) {
    std::ostringstream result;
    result << "[WHISPER]";
    result << "[" << who->GetName() << "]";
    result << "[" << to->GetName() << "]";
    result << msg;
    return result.str();
}

// [GUILD][PlayerName][GuildName]Message
std::string SimpleMessageFormatter::formatGuildMessage(Player const *who, Guild const *guild, std::string &msg) {
    std::ostringstream result;
    result << "[GUILD]";
    result << "[" << who->GetName() << "]";
    result << "[" << guild->GetName() << "]";
    result << msg;
    return result.str();
}

// [OFFICER][PlayerName][GuildName]Message
std::string SimpleMessageFormatter::formatOfficerMessage(Player const *who, Guild const *guild, std::string &msg) {
    std::ostringstream result;
    result << "[OFFICER]";
    result << "[" << who->GetName() << "]";
    result << "[" << guild->GetName() << "]";
    result << msg;
    return result.str();
}

// [CHANNEL][PlayerName][ChannelName]Message
std::string SimpleMessageFormatter::formatChannelMessage(Player const *who, Channel const *channel, std::string &msg) {
    std::ostringstream result;
    result << "[CHANNEL]";
    result << "[" << who->GetName() << "]";
    result << "[" << channel->GetName() << "]";
    result << msg;
    return result.str();
}

// [MOTD][PlayerName][GuildName]Message
std::string SimpleMessageFormatter::formatSetGuildMOTD(Player const *who, Guild const *guild, std::string &msg) {
    std::ostringstream result;
    result << "[MOTD]";
    result << "[" << who->GetName() << "]";
    result << "[" << guild->GetName() << "]";
    result << msg;
    return result.str();
}
