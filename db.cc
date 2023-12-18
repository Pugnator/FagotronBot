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
    LOG_DEBUG("Initing user manager\n");
    try
    {
      db = std::make_unique<SQLite::Database>(
          dbFilePath,
          SQLite::OPEN_READWRITE |
              SQLite::OPEN_CREATE |
              SQLite::OPEN_FULLMUTEX);
      db->exec(SQL_OPTIONS);
      db->exec("CREATE TABLE IF NOT EXISTS User (ID INTEGER PRIMARY KEY AUTOINCREMENT, UserID INTEGER, GroupID INTEGER, WinnerCount INTEGER DEFAULT 0);");
      //db->exec("CREATE TABLE IF NOT EXISTS Group (ID INTEGER PRIMARY KEY AUTOINCREMENT, GroupID INTEGER, LastRun INTEGER, AutoRun INTEGER);");
    }
    catch (const SQLite::Exception &e)
    {
      LOG_EXCEPTION("SQlite exception", e);
    }
  }

  bool UserManager::createUserEntry(int64_t username, int64_t group)
  {
    LOG_DEBUG("Creating user {} entry in group {}\n", username, group);
    try
    {
      SQLite::Statement query(*db, "INSERT INTO User (UserID, GroupID) VALUES (?,?);");
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

  bool UserManager::removeUserEntry(int64_t username, int64_t group)
  {
    LOG_DEBUG("Removing user {} from group {}\n", username, group);
    try
    {
      SQLite::Statement query(*db, "DELETE FROM User WHERE UserID = ? AND GroupID = ?;");
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

  bool UserManager::userExists(int64_t username, int64_t group)
  {
    LOG_DEBUG("Checking if user {} exists in group {}\n", username, group);
    try
    {
      SQLite::Statement query(*db, "SELECT ID FROM User WHERE UserID = ? AND GroupID = ?");
      query.bind(1, username);
      query.bind(2, group);
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
    LOG_DEBUG("Getting registered users for group {}\n", groupID);
    try
    {
      SQLite::Statement query(*db, "SELECT UserID FROM User WHERE GroupID = ?");
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

  bool UserManager::addWin(int64_t username, int64_t group)
  {
    LOG_DEBUG("Adding a win for {} in group {}\n", username, group);
    try
    {
      SQLite::Statement query(*db, "UPDATE User SET WinnerCount = WinnerCount + 1 WHERE UserID = ? AND GroupID = ?;");
      query.bind(1, username);
      query.bind(2, group);
      LOG_DEBUG("Query: {}\n", query.getExpandedSQL());
      query.exec();
      return true;
    }
    catch (const SQLite::Exception &e)
    {
      LOG_EXCEPTION("SQlite exception", e);
    }
    return false;
  }

  bool UserManager::getWinCount(int64_t username, int64_t group, int64_t &count)
  {
    LOG_DEBUG("Getting a win count for {} in group {}\n", username, group);
    try
    {
      SQLite::Statement query(*db, "SELECT WinnerCount FROM User WHERE UserID = ? AND GroupID = ?");
      query.bind(1, username);
      query.bind(2, group);
      if (query.executeStep())
      {
        count = query.getColumn(0).getInt64();
        return true;
      }
    }
    catch (const SQLite::Exception &e)
    {
      LOG_EXCEPTION("SQlite exception", e);
    }
    return false;
  }

  std::unordered_map<int64_t, int64_t> UserManager::getWinners(int64_t group, int32_t maxUsers)
  {
    LOG_DEBUG("Getting {} winners for group {}\n", maxUsers, group);
    try
    {
      SQLite::Statement query(*db, "SELECT UserID, WinnerCount FROM User WHERE GroupID=? ORDER BY WinnerCount DESC LIMIT ?;");
      query.bind(1, group);
      query.bind(2, maxUsers);
      std::unordered_map<int64_t, int64_t> winners;
      while (query.executeStep())
      {
        winners[query.getColumn(0).getInt64()] = query.getColumn(1).getInt64();
      }
      return winners;
    }
    catch (const SQLite::Exception &e)
    {
      LOG_EXCEPTION("SQlite exception", e);
    }
    return {};
  }
}