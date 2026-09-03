#pragma once

#include <string>
#include <ctime>

class User {
public:
    std::string username;
    std::string email;
    std::string passwordHash;
    std::string apiKey;
    bool isActive;
    std::time_t createdAt;

    User() : isActive(true), createdAt(std::time(nullptr)) {}

    User(const std::string& user, const std::string& mail, const std::string& hash, const std::string& key)
        : username(user), email(mail), passwordHash(hash), apiKey(key), isActive(true), createdAt(std::time(nullptr)) {}
};