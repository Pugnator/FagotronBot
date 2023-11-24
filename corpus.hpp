#pragma once

#include "yaml-cpp/yaml.h"
#include <string>
#include <vector>
#include <shared_mutex>

namespace Corpus
{
  enum class EntryType
  {
    Congratulations,
    Insults,
    Stickers,
    Announces,
    Gifs,
    Wisdom,
  };

  class CorpusManager
  {
  public:
    static CorpusManager &get()
    {
      static CorpusManager instance;
      return instance;
    }

    std::string entryTypeToString(EntryType type);
    std::vector<std::string> getEntries(EntryType type);
    std::string getRandomEntry(EntryType type);

  private:
    CorpusManager();

    CorpusManager(const CorpusManager &) = delete;
    CorpusManager &operator=(const CorpusManager &) = delete;
    CorpusManager(CorpusManager &&) = delete;
    CorpusManager &operator=(CorpusManager &&) = delete;

    YAML::Node corpus_;
    std::shared_mutex corpusMutex_;
  };
} // namespace Corpus