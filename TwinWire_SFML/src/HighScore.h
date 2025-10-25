#pragma once
#include <string>
#include <vector>

struct ScoreEntry
{
    std::string initials; // "AJX"
    float timeSec;
    int deaths;
};

// Orden : menor tiempo, y si empatan, menos muertes
bool operator<(const ScoreEntry& a, const  ScoreEntry& b);


class HighScore
{
public:
    static bool load(const std::string& path, std::vector<ScoreEntry>& out);
    static bool save(const std::string& path, const std::vector<ScoreEntry>& data);

    static void insertAndSort(std::vector<ScoreEntry>& data, const ScoreEntry& e, size_t keepTopN=10);
};
