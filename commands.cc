#include "fagotron.hpp"
#include "log.hpp"

namespace
{
  void playCommand(TgBot::Bot &bot, TgBot::Message::Ptr message)
  {
    int64_t userID = message->from->id;
    int64_t groupID = message->chat->id;
    LOG_DEBUG("User {} wants to play a game of fag\n", userID);
    Bot::playGameOfFag(bot, groupID);
  }

  void registerCommand(TgBot::Bot &bot, TgBot::Message::Ptr message)
  {
    int64_t userID = message->from->id;
    int64_t groupID = message->chat->id;
    Bot::UserManager userManager(bot);
    if (userManager.userExists(userID, groupID))
    {
      LOG_DEBUG("User {} is already a well-known fag.\n", userID);
      bot.getApi().sendMessage(groupID, "You are already a well-known fag.");
    }
    else
    {
      if (userManager.createUserEntry(userID, groupID))
      {
        LOG_DEBUG("Created the user entry for {}\n", userID);
        bot.getApi().sendMessage(groupID, "You are now the official fag.");
      }
      else
      {
        LOG_DEBUG("Failed to create the user {} entry\n", userID);
        bot.getApi().sendMessage(groupID, "Failed to register you, probably you're an imposter.");
      }
    }
  }

  void unregisterCommand(TgBot::Bot &bot, TgBot::Message::Ptr message)
  {
    int64_t userID = message->from->id;
    int64_t groupID = message->chat->id;
    Bot::UserManager userManager(bot);
    if (userManager.removeUserEntry(userID, groupID))
    {
      bot.getApi().sendMessage(groupID, "You're removed from the Gym, fag.");
    }
    else
    {
      bot.getApi().sendMessage(groupID, "Failed to remove you, try again.");
    }
  }

  void unknownCommand(TgBot::Bot &bot, TgBot::Message::Ptr message)
  {
    int64_t userID = message->from->id;
    int64_t groupID = message->chat->id;
    bot.getApi().sendMessage(groupID, "Unknown command");
    LOG_DEBUG("Unknown command {} from chat {}\n", message->text, userID);
  }

  using ptrProcessCommand = std::function<void(TgBot::Bot &, TgBot::Message::Ptr)>;
  
  std::unordered_map<std::string, ptrProcessCommand> mapCommands = 
  {
    {"/play", playCommand},
    {"/play@FagotronBot", playCommand},
    {"/register", registerCommand},
    {"/register@FagotronBot", registerCommand},
    {"/unregister", unregisterCommand},
    {"/unregister@FagotronBot", unregisterCommand},
  };

  ptrProcessCommand getCommand(std::string& cmd)
  {
    if (mapCommands.find(cmd) != mapCommands.end())
    {
      return mapCommands[cmd];
    }
    return unknownCommand;
  }
}

namespace Bot
{
  void processCommands(TgBot::Bot &bot, TgBot::Message::Ptr message)
  {
    getCommand(message->text)(bot, message);
  }
}
