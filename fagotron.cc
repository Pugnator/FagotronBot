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
  Log::get().configure(TraceType::file).set_level(TraceSeverity::debug);
  LOG_INFO("Starting FAGOTRON\n");

#if defined(WAKABOT_TOKEN)
  LOG_DEBUG("WAKABOT_TOKEN is set: {}\n", WAKABOT_TOKEN);
  TgBot::Bot bot(WAKABOT_TOKEN);
#else
  LOG_DEBUG("WAKABOT_TOKEN is not set, trying to get it from environment\n");
  const char *token = getenv("WAKABOT_TOKEN");
  if (!token)
  {
    LOG_DEBUG("WAKABOT_TOKEN is not set, exiting\n");
    return 1;
  }
  TgBot::Bot bot(token);
#endif

  LOG_INFO("Bot {} started\n", bot.getApi().getMe()->username);
  std::vector<TgBot::BotCommand::Ptr> commands;
  TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
  cmdArray->command = "play";
  cmdArray->description = "Who's the boss of the Gym?";
  commands.push_back(cmdArray);
  cmdArray = std::make_shared<TgBot::BotCommand>();
  cmdArray->command = "register";
  cmdArray->description = "Register yourself as a Gym member.";
  commands.push_back(cmdArray);
  cmdArray = std::make_shared<TgBot::BotCommand>();
  commands.push_back(cmdArray);
  cmdArray->command = "unregister";
  cmdArray->description = "Leave the Gym.";
  commands.push_back(cmdArray);
  bot.getApi().setMyCommands(commands);

  bot.getEvents().onUnknownCommand([&bot](TgBot::Message::Ptr message)
                                   { Bot::processCommands(bot, message); });

  bot.getEvents().onNonCommandMessage([](TgBot::Message::Ptr message) {});

  bot.getEvents().onCallbackQuery([](TgBot::CallbackQuery::Ptr query) {});

  signal(SIGINT, handleSignal);
  try
  {
    bot.getApi().deleteWebhook();
    TgBot::TgLongPoll longPoll(bot, 20, 5);
    int lastUpdateId = 0;
    while (!sigintReceived)
    {
      longPoll.start();
    }
  }
  catch (TgBot::TgException &e)
  {
    LOG_EXCEPTION("Runtime error", e);
  }
}