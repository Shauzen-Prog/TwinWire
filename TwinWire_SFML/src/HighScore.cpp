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
    if (!in.is_open()) return false;

    auto parseLine = [](const std::string& ln, ScoreEntry& e) -> bool {
        if (ln.empty()) return false;
        std::istringstream ss(ln);
        std::string init, tstr, dstr;
        if (!std::getline(ss, init, ';')) return false;
        if (!std::getline(ss, tstr, ';')) return false;
        if (!std::getline(ss, dstr)) return false;
        try {
            e.initials = init;
            e.timeSec  = std::stof(tstr);
            e.deaths   = std::stoi(dstr);
            return !e.initials.empty();
        } catch (...) { return false; }
    };

    std::string line;
   
    if (std::getline(in, line)) {
        if (line.rfind("initials;", 0) != 0) {
            ScoreEntry first{};
            if (parseLine(line, first)) out.push_back(first); // primera fila era dato, no header
        }
    }
    // resto de filas
    while (std::getline(in, line)) {
        ScoreEntry e{};
        if (parseLine(line, e)) out.push_back(e);
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
