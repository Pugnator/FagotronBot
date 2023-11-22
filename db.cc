#include "fagotron.hpp"
#include "log.hpp"

inline constexpr const char *const SQL_OPTIONS =
    "PRAGMA main.page_size = 4096;"
    "PRAGMA main.cache_size=-4096;"
    "PRAGMA main.synchronous=OFF;"
    "PRAGMA foreign_keys = ON;"
    "PRAGMA main.journal_mode=WAL;"
    "PRAGMA main.temp_store=MEMORY;";

namespace Bot
{
  UserManager::UserManager(TgBot::Bot &bot, const std::string &dbFilePath)
      : bot_(bot)
  {
    try
    {
      db = std::make_unique<SQLite::Database>(
          dbFilePath,
          SQLite::OPEN_READWRITE |
              SQLite::OPEN_CREATE |
              SQLite::OPEN_FULLMUTEX);
      db->exec(SQL_OPTIONS);
      db->exec("CREATE TABLE IF NOT EXISTS User (ID INTEGER PRIMARY KEY, GroupID INTEGER, WinnerCount INTEGER);");
    }
    catch (const SQLite::Exception &e)
    {
      LOG_EXCEPTION("SQlite exception", e);
    }
  }

  bool UserManager::createUserEntry(int64_t username, int64_t group)
  {
    try
    {
      SQLite::Statement query(*db, "INSERT INTO User (ID, GroupID, WinnerCount) VALUES (?,?,0)");
      query.bind(1, username);
      query.bind(2, group);
      query.exec();
      return true;
    }
    catch (const SQLite::Exception &e)
    {
      LOG_EXCEPTION("SQlite exception", e);
    }
    return false;
  }

  bool UserManager::removeUserEntry(int64_t username)
  {
    try
    {
      SQLite::Statement query(*db, "DELETE FROM User WHERE ID = ?");
      query.bind(1, username);
      query.exec();
      return true;
    }
    catch (const SQLite::Exception &e)
    {
      LOG_EXCEPTION("SQlite exception", e);
    }
    return false;
  }

  bool UserManager::userExists(int64_t username)
  {
    try
    {
      SQLite::Statement query(*db, "SELECT ID FROM User WHERE ID = ?");
      query.bind(1, username);
      return query.executeStep();
    }
    catch (const SQLite::Exception &e)
    {
      LOG_EXCEPTION("SQlite exception", e);
    }
    return false;
  }

  std::vector<int64_t> UserManager::getRegisteredGroupUsers(int64_t groupID)
  {
    try
    {
      SQLite::Statement query(*db, "SELECT ID FROM User WHERE GroupID = ?");
      query.bind(1, groupID);
      std::vector<int64_t> users;
      while (query.executeStep())
      {
        users.push_back(query.getColumn(0).getInt64());
      }
      return users;
    }
    catch (const SQLite::Exception &e)
    {
      LOG_EXCEPTION("SQlite exception", e);
    }
    return {};
  }
}