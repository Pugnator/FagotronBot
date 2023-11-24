
#include "fagotron.hpp"
#include "log.hpp"

namespace Bot
{
  void sentTextMessage(TgBot::Bot &bot, int64_t chatID, const std::string &message)
  {
    bot.getApi().sendMessage(chatID, message, false, 0, std::make_shared<TgBot::GenericReply>(), "Markdown");
  }
  void sendSticker(TgBot::Bot &bot, int64_t chatID, const std::string &stickerID)
  {
    
  }
}