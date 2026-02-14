#include "Core/UserManager.h"
#include "Core/Logger.h"
#include <fstream>
#include <iostream>

namespace Core {

std::map<std::string, std::string> UserManager::s_users;

void UserManager::Initialize() {
    LoadUsers();
}

bool UserManager::Register(const std::string& username, const std::string& password) {
    if (UserExists(username)) {
        return false;
    }
    s_users[username] = password;
    SaveUsers();
    return true;
}

bool UserManager::Login(const std::string& username, const std::string& password) {
    if (!UserExists(username)) {
        return false;
    }
    return s_users[username] == password;
}

bool UserManager::UserExists(const std::string& username) {
    return s_users.find(username) != s_users.end();
}

void UserManager::LoadUsers() {
    std::ifstream file("userdata.txt");
    if (!file.is_open()) return;

    std::string user, pass;
    while (file >> user >> pass) {
        s_users[user] = pass;
    }
    Logger::Info("Loaded " + std::to_string(s_users.size()) + " users.");
}

void UserManager::SaveUsers() {
    std::ofstream file("userdata.txt");
    if (!file.is_open()) {
        Logger::Error("Failed to save userdata.txt");
        return;
    }

    for (const auto& [user, pass] : s_users) {
        file << user << " " << pass << std::endl;
    }
}

} // namespace Core
