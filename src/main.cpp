#include <iostream>
#include <string>
#include "server/AuthServer.h"
#include "hwid/HWIDSpoofer.h"

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "       ALT SPOOFER - HWID Spoofer" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << std::endl;

    try {
        // Inicializar servidor de autenticação
        AuthServer authServer(8080);
        
        std::cout << "[*] Servidor iniciado na porta 8080" << std::endl;
        std::cout << "[*] Acesse: http://localhost:8080" << std::endl;
        std::cout << std::endl;

        // Executar servidor
        authServer.start();

    } catch (const std::exception& e) {
        std::cerr << "[ERRO] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}