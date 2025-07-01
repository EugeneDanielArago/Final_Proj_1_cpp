#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
#include <algorithm>

using namespace std;

mutex mtx;
int currentTaya = -1;
bool tayaChanged = false;

struct Player {
    string name;
    int score = 0;  // Score counts number of times the player became the Taya
};

void playerAction(int pid, vector<Player>& players) {
    if (pid == currentTaya) return;

    string move;

    mtx.lock();
    cout << players[pid].name << ", choose your move (run/hide): ";
    cin >> move;
    mtx.unlock();

    lock_guard<mutex> lock(mtx);
    if (move == "run") {
        if (rand() % 2 == 0) {
            cout << players[pid].name << " was caught running! They become the new Taya.\n";
            currentTaya = pid;
            tayaChanged = true;
        } else {
            cout << players[pid].name << " ran safely!\n";
        }
    } else if (move == "hide") {
        cout << players[pid].name << " hid successfully.\n";
    } else {
        cout << "Invalid input. Defaulting to hide.\n";
        cout << players[pid].name << " hid successfully.\n";
    }
}

void playGame(vector<Player>& players) {
    srand(time(0));
    currentTaya = rand() % players.size(); // Initial Taya
    players[currentTaya].score++; // Add score to initial Taya

    for (int round = 1; round <= 3; ++round) {
        cout << "\n--- Round " << round << " ---\n";
        cout << players[currentTaya].name << " is the Taya this round.\n";

        tayaChanged = false;

        vector<thread> threads;
        for (int i = 0; i < players.size(); ++i) {
            threads.emplace_back(playerAction, i, ref(players));
        }

        for (auto& t : threads)
            t.join();

        if (tayaChanged) {
            players[currentTaya].score++; // New Taya gains a point
        }

        cout << "End of Round " << round << ".\n";
        
        if (round < 3){
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
    cout << "Welcome to Tumbang Preso Digital Edition\n";
    for (int i = 0; i < 3; ++i) {
        Player p;
        cout << "Player " << i + 1 << ", enter your name: ";
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
