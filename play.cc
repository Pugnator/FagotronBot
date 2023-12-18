#include "fagotron.hpp"
#include "log.hpp"
#include <random>
#include <future>
#include <thread>
#include <vector>
#include "corpus.hpp"
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

    std::vector<std::string> names = {chat->username, chat->firstName, chat->lastName};
    std::erase_if(names, [](std::string const &s)
                  { return s.empty(); });
    std::uniform_int_distribution<> intName(0, names.size() - 1);
    int nameId = intName(gen);
    std::string username = names[nameId];

    std::string congratulations = Corpus::CorpusManager::get().getRandomEntry(Corpus::EntryType::Congratulations);
    sentTextMessage(bot, groupID, congratulations);
    std::string insult = Corpus::CorpusManager::get().getRandomEntry(Corpus::EntryType::Insults);
    std::string announce = Corpus::CorpusManager::get().getRandomEntry(Corpus::EntryType::Announces);
    std::string result = std::format("{}, [{}](tg://user?id={})", insult, username, fags[winner]);
    std::string announceText = std::vformat(announce, std::make_format_args(username));
    sentTextMessage(bot, groupID, announceText);
    sentTextMessage(bot, groupID, result);
    userManager.addWin(fags[winner], groupID);

    LOG_DEBUG("The winner is {}\n", winner);
  }

  std::vector<int64_t> getPossibleGroupUsers(TgBot::Bot &bot, int64_t groupID)
  {
    TgBot::Chat::Ptr chat = bot.getApi().getChat(userId);
  }
}