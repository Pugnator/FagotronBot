#include "corpus.hpp"
#include "log.hpp"
#define INCBIN_PREFIX corpus_
#include "incbin.h"
#include <random>

INCTXT(BasicCorpus, "corpus.yml");

namespace Corpus
{
  CorpusManager::CorpusManager()
  {
    const std::string corpusFile = "corpus.yml";
    try
    {
      corpus_ = YAML::LoadFile(corpusFile);
    }
    catch (const YAML::Exception &e)
    {
      LOG_DEBUG("Error loading corpus.yml: {}, recreating\n", e.what());
      auto defaultConfig = YAML::Load(corpus_BasicCorpusData);
      std::ofstream fout(corpusFile);
      if (!fout.is_open())
      {
        LOG_DEBUG("Failed to open settings file for writing.\n");
        throw std::runtime_error("Failed to open the insult corpus");
      }
      fout << defaultConfig << std::flush;
      fout.close();
      corpus_ = defaultConfig;
    }
  }

  std::string CorpusManager::entryTypeToString(EntryType type)
  {
    static const std::unordered_map<EntryType, std::string> typeToString = {
        {EntryType::Congratulations, "Congratulations"},
        {EntryType::Insults, "Insults"},
        {EntryType::Announces, "Announces"},
        {EntryType::Gifs, "Gifs"},
        {EntryType::Wisdom, "Wisdom"},
    };

    auto it = typeToString.find(type);
    if (it != typeToString.end())
    {
      return it->second;
    }

    return std::string();
  }

  std::vector<std::string> CorpusManager::getEntries(EntryType type)
  {
    auto typeName = entryTypeToString(type);
    if (typeName.empty())
    {
      return std::vector<std::string>();
    }
    std::shared_lock<std::shared_mutex> lock(corpusMutex_);
    std::vector<std::string> entries;
    const YAML::Node &node = corpus_[typeName];
    for (const auto &it : node["rus"])
    {
      entries.push_back(it.as<std::string>());
    }
    return entries;
  }

  std::string CorpusManager::getRandomEntry(EntryType type)
  {
    auto typeName = entryTypeToString(type);
    if (typeName.empty())
    {
      return std::string();
    }
    auto sen = getEntries(type);
    if (sen.empty())
    {
      return std::string();
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> intDist(0, sen.size() - 1);
    return sen[intDist(gen)];
  }

} //