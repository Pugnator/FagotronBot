#pragma once
#include <tgbot/tgbot.h>
#include <SQLiteCpp/SQLiteCpp.h>

namespace Bot
{
  void playGameOfFagAsync(TgBot::Bot &bot, int64_t userID);
  void playGameOfFag(TgBot::Bot &bot, int64_t userID);
  void processCommands(TgBot::Bot &bot, TgBot::Message::Ptr message);
  class UserManager
  {
  public:
    using Ptr = std::unique_ptr<UserManager>;

    UserManager(TgBot::Bot &bot, const std::string &dbFilePath = "users.db3");
    ~UserManager() = default;
    UserManager(const UserManager &) = delete;
    UserManager &operator=(const UserManager &) = delete;

    bool createUserEntry(int64_t username, int64_t group);
    bool removeUserEntry(int64_t username);
    bool userExists(int64_t username);
    std::vector<int64_t> getRegisteredGroupUsers(int64_t groupID);

  private:
    std::unique_ptr<SQLite::Database> db;
    TgBot::Bot &bot_;
  };
}