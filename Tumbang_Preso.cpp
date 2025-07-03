#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
#include <algorithm>
#include <random> 

using namespace std;

mutex mtx;
int currentTaya = -1;
bool tayaChanged = false;
int newTaya = -1;

struct Player {
    string name;
    int score = 0;  // Number of times the player became Taya
};

vector<string> moves(3); 

void playerAction(int pid, vector<Player>& players) {
    if (pid == currentTaya) return;

    string move;
    {
        lock_guard<mutex> lock(mtx);
        cout << players[pid].name << ", choose your move (run/hide): ";
        cin >> move;
    }

    if (move != "run" && move != "hide") {
        lock_guard<mutex> lock(mtx);
        cout << "Invalid input. Defaulting to hide.\n";
        move = "hide";
    }

    {
        lock_guard<mutex> lock(mtx);
        moves[pid] = move;
    }
}

void tayaAction(vector<Player>& players) {
    vector<int> candidates;
    for (int i = 0; i < players.size(); ++i) {
        if (i != currentTaya) candidates.push_back(i);
    }

     // Shuffle candidates for randomness
    random_device rd;
    mt19937 g(rd());
    shuffle(candidates.begin(), candidates.end(), g);

    for (int pid : candidates) {
        int chance = rand() % 2; // 50% chance to tag
        if (chance == 1) {
            lock_guard<mutex> lock(mtx);
            cout << players[pid].name << " was caught by " << players[currentTaya].name
                 << " while trying to " << moves[pid] << "!\n";
            newTaya = pid;
            tayaChanged = true;
            return;
        }
    }

    lock_guard<mutex> lock(mtx);
    cout << players[currentTaya].name << " failed to tag anyone this round.\n";
}

void playGame(vector<Player>& players) {
    srand(time(0));
    currentTaya = rand() % players.size(); // Initial Taya
    players[currentTaya].score++; // Add score to initial Taya

    for (int round = 1; round <= 3; ++round) {
        cout << "\n--- Round " << round << " ---\n";
        cout << players[currentTaya].name << " is the Taya this round.\n";

        tayaChanged = false;
        newTaya = -1;
        moves = vector<string>(players.size(), "");

        vector<thread> threads;
        for (int i = 0; i < players.size(); ++i) {
            threads.emplace_back(playerAction, i, ref(players));
        }

        for (auto& t : threads)
            t.join();

        tayaAction(players);

        if (tayaChanged && newTaya != -1) {
            currentTaya = newTaya;
            players[currentTaya].score++;
        }

        cout << "End of Round " << round << ".\n";
        
        if (round < 3) {
            cout << "Taya for next round will be: " << players[currentTaya].name << "\n";
        }
    }

    cout << "\n=== Game Over! ===\n\n";
}

void displayScoresAndWinners(const vector<Player>& players) {
    cout << "=== Final Scores ===\n";
    int minScore = INT_MAX;
    for (const auto& p : players) {
        cout << p.name << " - Taya Count: " << p.score << "\n";
        minScore = min(minScore, p.score);
    }

    cout << "\n=== Winner(s): ===\n";
    for (const auto& p : players) {
        if (p.score == minScore) {
            cout << p.name << "\n";
        }
    }
}

void loginPlayers(vector<Player>& players) {
    cout << "------------------------------\n";
    cout << " Welcome to Tumbang Preso 2.0\n";
    cout << "------------------------------\n";
    for (int i = 0; i < 3; ++i) {
        Player p;
        cout << "Enter Player " << i + 1 << " Name: ";
        cin >> p.name;
        players.push_back(p);
    }
}

int main() {
    vector<Player> players;
    loginPlayers(players);
    playGame(players);
    displayScoresAndWinners(players);
    return 0;
}
