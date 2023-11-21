#include <csignal>

#include <tgbot/tgbot.h>
#include "log.hpp"
#include "botcommander.hpp"

std::atomic<bool> sigintReceived(false);

void handleSignal(int s)
{
  LOG_INFO("Got SIGINT, exiting\n");
  sigintReceived = true;
}

int main()
{

  TgBot::Bot bot(getenv("TOKEN"));
  Log::get().configure(TraceType::file).set_level(TraceSeverity::debug);
  LOG_INFO("Bot {} started\n", bot.getApi().getMe()->username);

  Bot::BotCommander::Ptr commander = std::make_unique<Bot::BotCommander>(bot);

  // Set commands
  std::vector<TgBot::BotCommand::Ptr> commands;
  TgBot::BotCommand::Ptr cmdArray = std::make_shared<TgBot::BotCommand>();
  cmdArray->command = "search";
  cmdArray->description = "Search for a word, kanji or example in the dictionary.";
  commands.push_back(cmdArray);
  cmdArray = std::make_shared<TgBot::BotCommand>();
  cmdArray->command = "quiz";
  cmdArray->description = "Start a quiz. You can choose between diffeeent types of questions.";
  commands.push_back(cmdArray);
  cmdArray = std::make_shared<TgBot::BotCommand>();
  cmdArray->command = "difficulty";
  cmdArray->description = "Set the difficulty level for the quiz.";
  commands.push_back(cmdArray);
  // cmdArray = std::make_shared<TgBot::BotCommand>();
  // cmdArray->command = "explain";
  // cmdArray->description = "Explain a word, kanji or example.";
  // commands.push_back(cmdArray);
  bot.getApi().setMyCommands(commands);

  // Set event handlers
  bot.getEvents().onUnknownCommand([&commander](TgBot::Message::Ptr message)
                                   { commander->parseCommand(message); });

  bot.getEvents().onNonCommandMessage([&commander](TgBot::Message::Ptr message)
                                      { commander->parseUserInputAsync(message); });

  bot.getEvents().onCallbackQuery([&commander](TgBot::CallbackQuery::Ptr query)
                                  { commander->parseCallback(query); });

  signal(SIGINT, handleSignal);

  bot.getApi().deleteWebhook();
  TgBot::TgLongPoll longPoll(bot, 20, 5);
  int lastUpdateId = 0;
  while (!sigintReceived)
  {
    try
    {
      longPoll.start();
    }
    catch (TgBot::TgException &e)
    {
      LOG_EXCEPTION("Runtime error", e);
    }
  }
}