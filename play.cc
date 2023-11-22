#include "fagotron.hpp"
#include "log.hpp"
#include <random>
#include <future>
#include <thread>

namespace Bot
{
  void playGameOfFag(TgBot::Bot &bot, int64_t groupID)
  {
    std::future<void> future = std::async(std::launch::async, playGameOfFagAsync, std::ref(bot), groupID);
  }

  void playGameOfFagAsync(TgBot::Bot &bot, int64_t groupID)
  {
    LOG_DEBUG("Group {} wants to play a game of fag\n", groupID);
    Bot::UserManager userManager(bot);
    auto fags = userManager.getRegisteredGroupUsers(groupID);
    if (fags.empty())
    {
      bot.getApi().sendMessage(groupID, "There is no fags here. Find one first.");
      return;
    }
    bot.getApi().sendDice(groupID, false, 0, std::make_shared<TgBot::GenericReply>(), "🎲", "Markdown");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> intDist(0, fags.size() - 1);
    int winner = intDist(gen);
    TgBot::Chat::Ptr chat = bot.getApi().getChat(fags[winner]);
    auto result = std::format("And the winner is ... [{}](tg://user?id={})", chat->username, fags[winner]);

    bot.getApi().sendMessage(groupID, result, false, 0, std::make_shared<TgBot::GenericReply>(), "Markdown");

    LOG_DEBUG("The winner is {}\n", winner);
  }
}