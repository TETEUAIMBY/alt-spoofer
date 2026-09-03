#pragma once

#include <string>
#include <iostream>
#include <map>
#include "../utils/CryptoUtils.h"

#ifdef _WIN32
    #include <windows.h>
    #include <wbemidl.h>
    #include <wmistr.h>
    #pragma comment(lib, "wbemuuid.lib")
    #pragma comment(lib, "oleaut32.lib")
    #pragma comment(lib, "ole32.lib")
#endif

class HWIDSpoofer {
private:
    std::string originalHWID;
    std::string currentSpoofdHWID;
    bool isSpoofd;

public:
    HWIDSpoofer() : isSpoofd(false) {
        getOriginalHWID();
    }

    // Obter HWID original do sistema
    std::string getOriginalHWID() {
#ifdef _WIN32
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
            "SOFTWARE\\Microsoft\\Cryptography", 
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            
            char buffer[256];
            DWORD bufferSize = sizeof(buffer);
            
            if (RegQueryValueExA(hKey, "MachineGuid", NULL, NULL, 
                (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
                originalHWID = std::string(buffer);
                RegCloseKey(hKey);
                std::cout << "[+] HWID Original detectado: " << originalHWID << std::endl;
                return originalHWID;
            }
            RegCloseKey(hKey);
        }
#endif
        return "";
    }

    // Falsificar HWID (gerar novo aleatório)
    bool spoofdHWID() {
        if (isSpoofd) {
            std::cout << "[!] HWID já foi falsificado!" << std::endl;
            return false;
        }

        currentSpoofdHWID = CryptoUtils::generateRandomHWID();

#ifdef _WIN32
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
            "SOFTWARE\\Microsoft\\Cryptography", 
            0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            
            if (RegSetValueExA(hKey, "MachineGuid", 0, REG_SZ, 
                (const BYTE*)currentSpoofdHWID.c_str(), 
                (DWORD)currentSpoofdHWID.length() + 1) == ERROR_SUCCESS) {
                
                isSpoofd = true;
                RegCloseKey(hKey);
                std::cout << "[+] HWID Falsificado com sucesso!" << std::endl;
                std::cout << "[+] Novo HWID: " << currentSpoofdHWID << std::endl;
                std::cout << "[!] Reinicie o PC para voltar ao HWID original." << std::endl;
                return true;
            }
            RegCloseKey(hKey);
        }
#endif
        return false;
    }

    // Restaurar HWID original
    bool restoreOriginalHWID() {
        if (!isSpoofd || originalHWID.empty()) {
            std::cout << "[!] HWID não foi falsificado ou original não encontrado!" << std::endl;
            return false;
        }

#ifdef _WIN32
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
            "SOFTWARE\\Microsoft\\Cryptography", 
            0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            
            if (RegSetValueExA(hKey, "MachineGuid", 0, REG_SZ, 
                (const BYTE*)originalHWID.c_str(), 
                (DWORD)originalHWID.length() + 1) == ERROR_SUCCESS) {
                
                isSpoofd = false;
                RegCloseKey(hKey);
                std::cout << "[+] HWID Restaurado para original!" << std::endl;
                return true;
            }
            RegCloseKey(hKey);
        }
#endif
        return false;
    }

    // Obter HWID atual
    std::string getCurrentHWID() {
        return isSpoofd ? currentSpoofdHWID : originalHWID;
    }

    // Verificar se está falsificado
    bool isSpoofdActive() const {
        return isSpoofd;
    }
};