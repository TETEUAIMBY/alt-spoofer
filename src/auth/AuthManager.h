#pragma once

#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include "User.h"
#include "../utils/CryptoUtils.h"

class AuthManager {
private:
    std::map<std::string, User> users;
    const std::string DATABASE_FILE = "users.db";

public:
    AuthManager() {
        loadUsersFromFile();
    }

    // Registrar novo usuário
    bool registerUser(const std::string& username, const std::string& email, const std::string& password) {
        // Verificar se usuário já existe
        if (users.find(username) != users.end()) {
            return false;
        }

        // Hash da senha
        std::string passwordHash = CryptoUtils::hashPassword(password);
        
        // Gerar API Key única
        std::string apiKey = CryptoUtils::generateAPIKey();

        // Criar usuário
        User newUser(username, email, passwordHash, apiKey);
        users[username] = newUser;

        // Salvar no arquivo
        saveUsersToFile();

        std::cout << "[+] Usuário '" << username << "' registrado com sucesso!" << std::endl;
        std::cout << "[+] API Key: " << apiKey << std::endl;

        return true;
    }

    // Login do usuário
    bool loginUser(const std::string& username, const std::string& password) {
        auto it = users.find(username);
        if (it == users.end()) {
            return false;
        }

        User& user = it->second;
        std::string passwordHash = CryptoUtils::hashPassword(password);

        if (user.passwordHash == passwordHash && user.isActive) {
            std::cout << "[+] Login bem-sucedido para: " << username << std::endl;
            return true;
        }

        return false;
    }

    // Validar API Key
    bool validateAPIKey(const std::string& apiKey) {
        for (auto& pair : users) {
            if (pair.second.apiKey == apiKey && pair.second.isActive) {
                return true;
            }
        }
        return false;
    }

    // Obter usuário pela API Key
    std::string getUserByAPIKey(const std::string& apiKey) {
        for (auto& pair : users) {
            if (pair.second.apiKey == apiKey) {
                return pair.first;
            }
        }
        return "";
    }

    // Salvar usuários em arquivo
    void saveUsersToFile() {
        std::ofstream file(DATABASE_FILE);
        for (auto& pair : users) {
            User& user = pair.second;
            file << user.username << "|" << user.email << "|" 
                 << user.passwordHash << "|" << user.apiKey << "|"
                 << (user.isActive ? "1" : "0") << "|" << user.createdAt << "\n";
        }
        file.close();
    }

    // Carregar usuários do arquivo
    void loadUsersFromFile() {
        std::ifstream file(DATABASE_FILE);
        if (!file.is_open()) {
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string username, email, hash, apiKey, active, createdAt;

            std::getline(iss, username, '|');
            std::getline(iss, email, '|');
            std::getline(iss, hash, '|');
            std::getline(iss, apiKey, '|');
            std::getline(iss, active, '|');
            std::getline(iss, createdAt, '|');

            User user(username, email, hash, apiKey);
            user.isActive = (active == "1");
            user.createdAt = std::stol(createdAt);

            users[username] = user;
        }

        file.close();
        std::cout << "[+] " << users.size() << " usuários carregados do banco de dados." << std::endl;
    }
};