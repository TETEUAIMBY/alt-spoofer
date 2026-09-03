#pragma once

#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <thread>
#include <memory>
#include <sstream>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

class HTTPServer {
private:
    int port;
    std::map<std::string, std::function<std::string(const std::string&, const std::map<std::string, std::string>&)>> getHandlers;
    std::map<std::string, std::function<std::string(const std::string&, const std::map<std::string, std::string>&)>> postHandlers;

public:
    HTTPServer(int p) : port(p) {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    }

    ~HTTPServer() {
#ifdef _WIN32
        WSACleanup();
#endif
    }

    void onGET(const std::string& path, std::function<std::string(const std::string&, const std::map<std::string, std::string>&)> handler) {
        getHandlers[path] = handler;
    }

    void onPOST(const std::string& path, std::function<std::string(const std::string&, const std::map<std::string, std::string>&)> handler) {
        postHandlers[path] = handler;
    }

    void start() {
#ifdef _WIN32
        SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "[ERRO] Falha ao criar socket" << std::endl;
            return;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "[ERRO] Falha ao fazer bind do socket" << std::endl;
            closesocket(serverSocket);
            return;
        }

        listen(serverSocket, SOMAXCONN);

        std::cout << "[*] Servidor HTTP aguardando conexões na porta " << port << std::endl;

        while (true) {
            sockaddr_in clientAddr;
            int clientAddrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);

            if (clientSocket == INVALID_SOCKET) {
                continue;
            }

            std::thread(&HTTPServer::handleClient, this, clientSocket).detach();
        }

        closesocket(serverSocket);
#endif
    }

private:
    void handleClient(SOCKET clientSocket) {
        char buffer[4096] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            return;
        }

        std::string request(buffer, bytesReceived);
        std::cout << "[*] Requisição recebida:\n" << request.substr(0, 200) << "...\n" << std::endl;

        std::string response = handleRequest(request);
        send(clientSocket, response.c_str(), (int)response.length(), 0);

        closesocket(clientSocket);
    }

    std::string handleRequest(const std::string& request) {
        std::istringstream iss(request);
        std::string method, path, protocol;
        iss >> method >> path >> protocol;

        // Parse headers e body
        std::map<std::string, std::string> headers;
        std::string line;
        std::string body;
        bool headersDone = false;

        while (std::getline(iss, line)) {
            if (line == "\r" || line.empty()) {
                headersDone = true;
                break;
            }
            
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 2);
                if (!value.empty() && value.back() == '\r') value.pop_back();
                headers[key] = value;
            }
        }

        if (headersDone) {
            std::getline(iss, body);
        }

        // Extrair path sem query string
        size_t queryPos = path.find('?');
        std::map<std::string, std::string> params;
        if (queryPos != std::string::npos) {
            std::string queryString = path.substr(queryPos + 1);
            path = path.substr(0, queryPos);
            parseQueryString(queryString, params);
        }

        std::string responseBody;
        std::string contentType = "application/json";

        if (method == "GET") {
            auto it = getHandlers.find(path);
            if (it != getHandlers.end()) {
                responseBody = it->second(path, params);
                if (path == "/") {
                    contentType = "text/html; charset=utf-8";
                }
            } else {
                responseBody = "{\"error\":\"Rota não encontrada\"}";
            }
        } else if (method == "POST") {
            auto it = postHandlers.find(path);
            if (it != postHandlers.end()) {
                responseBody = it->second(body, headers);
            } else {
                responseBody = "{\"error\":\"Rota não encontrada\"}";
            }
        }

        return buildHTTPResponse(responseBody, contentType);
    }

    std::string buildHTTPResponse(const std::string& body, const std::string& contentType) {
        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n";
        oss << "Content-Type: " << contentType << "\r\n";
        oss << "Content-Length: " << body.length() << "\r\n";
        oss << "Connection: close\r\n";
        oss << "Access-Control-Allow-Origin: *\r\n";
        oss << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
        oss << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
        oss << "\r\n";
        oss << body;

        return oss.str();
    }

    void parseQueryString(const std::string& queryString, std::map<std::string, std::string>& params) {
        std::istringstream iss(queryString);
        std::string pair;

        while (std::getline(iss, pair, '&')) {
            size_t eqPos = pair.find('=');
            if (eqPos != std::string::npos) {
                std::string key = pair.substr(0, eqPos);
                std::string value = pair.substr(eqPos + 1);
                params[key] = value;
            }
        }
    }
};