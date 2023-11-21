#include <csignal>

#include <tgbot/tgbot.h>
#include "log.hpp"
#include "fagotron.hpp"

std::atomic<bool> sigintReceived(false);

void handleSignal(int s)
{
  LOG_INFO("Got SIGINT, exiting\n");
  sigintReceived = true;
}

int main()
{
  TgBot::Bot bot(getenv("FAGOTRON_TOKEN"));
  Log::get().configure(TraceType::file).set_level(TraceSeverity::debug);
  LOG_INFO("Bot {} started\n", bot.getApi().getMe()->username);  

  // Set commands
  std::vector<TgBot::BotCommand::Ptr> commands;  
  cmdArray->command = "play";
  cmdArray->description = U"Кто сегодня герой?";
  commands.push_back(cmdArray);
  bot.getApi().setMyCommands(commands); 

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