#include "Database.hpp"
#include "Server.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main() {
    cout << "Starting C.CODE Server..." << endl;

    Database db;
    if (!db.connect()) {
        cerr << "Failed to connect to database." << endl;
        // Continue for now to test socket
    }

    Server server;
    if (!server.start(8080)) {
        return 1;
    }

    // Keep main thread alive
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}