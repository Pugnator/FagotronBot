#include <random>

#include "fagotron.hpp"
#include "log.hpp"

namespace
{
  void playCommand(TgBot::Bot &bot, TgBot::Message::Ptr message)
  {
    int64_t userID = message->from->id;
    int64_t groupID = message->chat->id;
    LOG_DEBUG("User {} wants to play a game of fag\n", userID);
    Bot::UserManager userManager(bot);
    if (!userManager.isUserAdded(userID, groupID))
    {
      bot.getApi().sendMessage(groupID, "Only real fag can run a game. You're not the one.");
      return;
    }
    Bot::playGameOfFag(bot, groupID);
  }

  void registerCommand(TgBot::Bot &bot, TgBot::Message::Ptr message)
  {
    int64_t userID = message->from->id;
    int64_t groupID = message->chat->id;
    Bot::UserManager userManager(bot);
    if (userManager.isUserAdded(userID, groupID))
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

  void statisticsCommand(TgBot::Bot &bot, TgBot::Message::Ptr message)
  {
    int64_t userID = message->from->id;
    int64_t groupID = message->chat->id;
    Bot::UserManager userManager(bot);
    unsigned maxUsers = 10;
    std::unordered_map<int64_t, int64_t> luckyGuys = userManager.getWinners(groupID, maxUsers);
    std::string winners;
    std::random_device rd;
    std::mt19937 gen(rd());
    LOG_DEBUG("Found {} winners\n", luckyGuys.size());
    for (const auto &pair : luckyGuys)
    {
      int64_t userId = pair.first;
      int64_t winCount = pair.second;
      TgBot::Chat::Ptr chat = bot.getApi().getChat(userId);
      std::vector<std::string> names = {chat->username, chat->firstName, chat->lastName};
      std::erase_if(names, [](std::string const &s)
                    { return s.empty(); });
      std::uniform_int_distribution<> intName(0, names.size() - 1);
      int nameId = intName(gen);
      std::string username = names[nameId];
      LOG_DEBUG("Winner {} has {} wins\n", username, winCount);
      winners += std::format("{}: {}\n", username, winCount);
    }
    if(winners.empty())
    {
      winners = "No winners yet";
    }
    bot.getApi().sendMessage(groupID, winners);
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
          {"/stats@FagotronBot", statisticsCommand},
  };

  ptrProcessCommand getCommand(std::string &cmd)
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
