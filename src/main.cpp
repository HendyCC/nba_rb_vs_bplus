#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include "httplib.h"
#include "json.hpp"
#include "benchmark.cpp"
 
using json = nlohmann::json;
 
// ---------------------------------------------------------------------------
// Loading game.csv. Every game becomes two records:
//   key = gameId * 2      -> the home record (home = 0)
//   key = gameId * 2 + 1  -> the away record (away = 1)
// ---------------------------------------------------------------------------
 
static std::vector<std::string> split_line(std::string line, char delimiter) {
    std::vector<std::string> fields;
    std::string current;
    bool in_quotes = false;
    for (char c : line) {
        if (c == '"') {
            in_quotes = !in_quotes;
        }
        else if (c == delimiter && !in_quotes) {
            fields.push_back(current);
            current = "";
        }
        else if (c != '\r') {
            current += c;
        }
    }
    fields.push_back(current);
    return fields;
}
 
static std::vector<GameRecord> load_games(std::string path) {
    std::vector<GameRecord> records;
    std::ifstream file(path);
    std::string line;
    if (!file || !std::getline(file, line)) {
        return records;
    }
 
    char delimiter = ',';
    if (line.find('\t') != std::string::npos) {
        delimiter = '\t';
    }
 
    std::map<std::string, int> column;
    std::vector<std::string> header = split_line(line, delimiter);
    for (int i = 0; i < (int)header.size(); i++) {
        column[header[i]] = i;
    }
    const char* needed[] = {"game_id", "game_date", "team_id_home", "team_id_away", "pts_home", "pts_away"};
    for (const char* name : needed) {
        if (column.count(name) == 0) {
            std::cout << "game.csv is missing the column " << name << "\n";
            return records;
        }
    }
 
    while (std::getline(file, line)) {
        std::vector<std::string> f = split_line(line, delimiter);
        if (f.size() < header.size()) {
            continue;
        }
        std::string pts_home = f[column["pts_home"]];
        std::string pts_away = f[column["pts_away"]];
        if (pts_home.empty() || pts_away.empty()) {
            continue;   // skip games with no score
        }
 
        try {
            unsigned long long game_id = std::stoull(f[column["game_id"]]);
            std::string date = f[column["game_date"]].substr(0, 10);
 
            GameRecord home;
            home.key = game_id * 2;
            home.gameId = game_id;
            home.date = date;
            home.teamId = f[column["team_id_home"]];
            home.side = "home";
            home.points = (int)std::stod(pts_home);
            records.push_back(home);
 
            GameRecord away;
            away.key = game_id * 2 + 1;
            away.gameId = game_id;
            away.date = date;
            away.teamId = f[column["team_id_away"]];
            away.side = "away";
            away.points = (int)std::stod(pts_away);
            records.push_back(away);
        }
        catch (...) {
            continue;
        }
    }
    return records;
}
 

template <typename F>
static BenchmarkResult best_of(int runs, F run) {
    BenchmarkResult best = run();
    for (int i = 1; i < runs; i++) {
        BenchmarkResult r = run();
        if (r.elapsedMicroseconds < best.elapsedMicroseconds) {
            best.elapsedMicroseconds = r.elapsedMicroseconds;
        }
    }
    return best;
}
 
static double two_decimals(double x) {
    return std::round(x * 100.0) / 100.0;
}
 
static std::string find_folder(const char* name, const char* file) {
    const char* prefixes[] = {"./", "../", "../../"};
    for (const char* prefix : prefixes) {
        std::string path = std::string(prefix) + name;
        if (std::filesystem::exists(path + "/" + file)) {
            return path;
        }
    }
    return "";
}
 
int main(int argc, char** argv) {
    // load the data
    std::string csv = argc > 1 ? argv[1] : find_folder("data", "game.csv") + "/game.csv";
    std::vector<GameRecord> records = load_games(csv);
    if (records.empty()) {
        std::cout << "Could not load " << csv << ". Put game.csv in the data folder.\n";
        return 1;
    }
 
    // build both trees from the same records
    RedBlackTree rb;
    BPlusTree bp;
    for (GameRecord& r : records) {
        rb.insert(r);
    }
    for (GameRecord& r : records) {
        bp.insert(r.key, r);
    }
    std::cout << "Loaded " << records.size() << " records. Red-black tree: " << rb.size()
              << " records, height " << rb.height() << ". B+ tree: " << bp.size()
              << " records, height " << bp.height() << ".\n";
 
    httplib::Server server;
    std::mutex tree_lock;
 
    std::string website = find_folder("website", "index.html");
    if (website.empty()) {
        std::cout << "Could not find website/index.html\n";
        return 1;
    }
    server.set_mount_point("/", website);
 
    server.Post("/query", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(tree_lock);
        try {
            json input = json::parse(req.body);
            std::string type = input["type"];
            unsigned long long startID = input["startID"];
            unsigned long long endID = input["endID"];
 
            BenchmarkResult rbResult;
            BenchmarkResult bpResult;
            unsigned long long rbReturned = 0;
            unsigned long long bpReturned = 0;
 
            if (type == "range") {
                // from the home record of the first game to the away record of the last game
                unsigned long long lo = startID * 2;
                unsigned long long hi = endID * 2 + 1;
                rbReturned = rb.range_search(lo, hi).size();
                bpReturned = bp.range(lo, hi).size();
                rbResult = best_of(30, [&]() { return TreeBenchmark::benchmarkRBRange(rb, lo, hi); });
                bpResult = best_of(30, [&]() { return TreeBenchmark::benchmarkBPlusRange(bp, lo, hi); });
            }
            else {
                // exact lookup of the home record of that game
                unsigned long long key = startID * 2;
                rbReturned = rb.search(key) != nullptr ? 1 : 0;
                bpReturned = bp.find(key) != nullptr ? 1 : 0;
                rbResult = best_of(500, [&]() { return TreeBenchmark::benchmarkRBFind(rb, key); });
                bpResult = best_of(500, [&]() { return TreeBenchmark::benchmarkBPlusFind(bp, key); });
            }
 
            json output;
            output["rbTime"] = two_decimals(rbResult.elapsedMicroseconds);
            output["bpTime"] = two_decimals(bpResult.elapsedMicroseconds);
            output["rbNodes"] = rbResult.node_count;
            output["bpNodes"] = bpResult.node_count;
            output["rbHeight"] = rbResult.height;
            output["bpHeight"] = bpResult.height;
            output["rbRecords"] = rbReturned;
            output["bpRecords"] = bpReturned;
            res.set_content(output.dump(), "application/json");
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });
 
    std::cout << "Website running at http://localhost:8080\n";
    if (!server.listen("localhost", 8080)) {
        std::cout << "Could not start the server on port 8080. Is another program using it?\n";
    }
    return 0;
}

