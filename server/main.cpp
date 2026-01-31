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
        return 1;
    }

    Server server(&db);
    if (!server.start(8081)) {
        return 1;
    }

    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}