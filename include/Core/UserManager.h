#pragma once
#include <string>
#include <map>

namespace Core {

class UserManager {
public:
    static void Initialize();
    static bool Register(const std::string& username, const std::string& password);
    static bool Login(const std::string& username, const std::string& password);
    static bool UserExists(const std::string& username);
    static const std::map<std::string, std::string>& GetUsers() { return s_users; }

private:
    static std::map<std::string, std::string> s_users;
    static void LoadUsers();
    static void SaveUsers();
};

} // namespace Core
