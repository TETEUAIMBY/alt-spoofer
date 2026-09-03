#pragma once

#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstring>

class CryptoUtils {
public:
    // Gerar hash simples da senha (em produção, usar bcrypt ou argon2)
    static std::string hashPassword(const std::string& password) {
        unsigned long hash = 5381;
        for (char c : password) {
            hash = ((hash << 5) + hash) + c;
        }
        
        std::stringstream ss;
        ss << std::hex << hash;
        return ss.str();
    }

    // Gerar API Key única
    static std::string generateAPIKey() {
        const char* charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 61);

        std::string apiKey = "sk_";
        for (int i = 0; i < 32; ++i) {
            apiKey += charset[dis(gen)];
        }

        return apiKey;
    }

    // Gerar HWID aleatório
    static std::string generateRandomHWID() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);

        std::stringstream ss;
        for (int i = 0; i < 16; ++i) {
            ss << std::hex << dis(gen);
        }

        return ss.str();
    }

    // Encodar Base64
    static std::string base64Encode(const std::string& input) {
        static const char* base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        std::string ret;
        int val = 0;
        int valb = 0;

        for (unsigned char c : input) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 6) {
                valb -= 6;
                ret.push_back(base64_chars[(val >> valb) & 0x3F]);
            }
        }

        if (valb > 0) ret.push_back(base64_chars[(val << (6 - valb)) & 0x3F]);
        while (ret.size() % 4) ret.push_back('=');

        return ret;
    }
};