#include "fagotron.hpp"
#include "log.hpp"

namespace Bot
{
  void processCommands(TgBot::Bot &bot, TgBot::Message::Ptr message)
  {
    int64_t userID = message->from->id;
    int64_t groupID = message->chat->id;
    if (message->text == "/play@FagotronBot")
    {
      LOG_DEBUG("User {} wants to play a game of fag\n", userID);
      Bot::playGameOfFag(bot, groupID);
    }
    else if (message->text == "/register@FagotronBot")
    {

      Bot::UserManager userManager(bot);
      if (userManager.userExists(userID))
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
    else if (message->text == "/unregister@FagotronBot")
    {
      Bot::UserManager userManager(bot);
      if (userManager.removeUserEntry(userID))
      {
        bot.getApi().sendMessage(groupID, "You're removed from the Gym, fag.");
      }
      else
      {
        bot.getApi().sendMessage(groupID, "Failed to remove you, try again.");
      }
    }
    else
    {
      bot.getApi().sendMessage(groupID, "Unknown command");
      LOG_DEBUG("Unknown command {} from chat {}\n", message->text, userID);
    }
  }
}