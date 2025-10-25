#include "HighScore.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

bool operator<(const ScoreEntry& a, const ScoreEntry& b)
{
    if (a.timeSec != b.timeSec) return a.timeSec < b.timeSec;
    return a.deaths < b.deaths;
}

bool HighScore::load(const std::string& path, std::vector<ScoreEntry>& out)
{
    out.clear();
    std::ifstream in(path);
    if(!in.is_open()) return false;
    std::string line;
    if (std::getline(in, line)) { // chequeo encabezado
        if (line.rfind("initials;",0) != 0) { in.clear(); in.seekg(0); }
    }
    while(std::getline(in, line)){
        if(line.empty()) continue;
        std::istringstream ss(line);
        std::string init; char sep;
        float t; int d;
        if( (ss >> init >> sep >> t >> sep >> d) && !init.empty() ){
            out.push_back({init, t, d});
        }
    }
    return true;
}

bool HighScore::save(const std::string& path, const std::vector<ScoreEntry>& data)
{
    namespace fs = std::filesystem;
    fs::create_directories(fs::path(path).parent_path());
    std::ofstream out(path, std::ios::trunc);
    if(!out.is_open()) return false;
    out << "initials;time;deaths\n";
    for(const auto& e: data)
        out << e.initials << ";" << e.timeSec << ";" << e.deaths << "\n";
    return true;
}

void HighScore::insertAndSort(std::vector<ScoreEntry>& data, const ScoreEntry& e, size_t keepTopN)
{
    data.push_back(e);
    std::sort(data.begin(), data.end()); // usa operator<
    if (data.size() > keepTopN) data.resize(keepTopN);
}
