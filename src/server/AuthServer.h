#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include "../auth/AuthManager.h"
#include "../hwid/HWIDSpoofer.h"
#include "HTTPServer.h"

class AuthServer {
private:
    int port;
    AuthManager authManager;
    HWIDSpoofer hwidSpoofer;
    HTTPServer httpServer;

public:
    AuthServer(int p) : port(p), httpServer(p) {
        setupRoutes();
    }

    void setupRoutes() {
        // Rota: GET / - Servir página de login
        httpServer.onGET("/", [this](const std::string& path, const std::map<std::string, std::string>& params) {
            return getLoginPage();
        });

        // Rota: POST /register - Registrar novo usuário
        httpServer.onPOST("/api/register", [this](const std::string& body, const std::map<std::string, std::string>& headers) {
            return handleRegister(body);
        });

        // Rota: POST /login - Login do usuário
        httpServer.onPOST("/api/login", [this](const std::string& body, const std::map<std::string, std::string>& headers) {
            return handleLogin(body);
        });

        // Rota: POST /spoof - Falsificar HWID (requer autenticação)
        httpServer.onPOST("/api/spoof", [this](const std::string& body, const std::map<std::string, std::string>& headers) {
            return handleSpoof(body, headers);
        });

        // Rota: GET /api/hwid - Obter HWID atual (requer autenticação)
        httpServer.onGET("/api/hwid", [this](const std::string& path, const std::map<std::string, std::string>& params) {
            return handleGetHWID(params);
        });

        // Rota: POST /api/restore - Restaurar HWID original (requer autenticação)
        httpServer.onPOST("/api/restore", [this](const std::string& body, const std::map<std::string, std::string>& headers) {
            return handleRestore(headers);
        });
    }

    std::string handleRegister(const std::string& body) {
        std::string username = extractJSONValue(body, "username");
        std::string email = extractJSONValue(body, "email");
        std::string password = extractJSONValue(body, "password");

        if (username.empty() || email.empty() || password.empty()) {
            return "{\"success\":false,\"message\":\"Dados inválidos\"}";
        }

        if (authManager.registerUser(username, email, password)) {
            return "{\"success\":true,\"message\":\"Usuário registrado com sucesso\"}";
        }

        return "{\"success\":false,\"message\":\"Usuário já existe\"}";
    }

    std::string handleLogin(const std::string& body) {
        std::string username = extractJSONValue(body, "username");
        std::string password = extractJSONValue(body, "password");

        if (username.empty() || password.empty()) {
            return "{\"success\":false,\"message\":\"Dados inválidos\"}";
        }

        if (authManager.loginUser(username, password)) {
            return "{\"success\":true,\"message\":\"Login bem-sucedido\"}";
        }

        return "{\"success\":false,\"message\":\"Usuário ou senha incorretos\"}";
    }

    std::string handleSpoof(const std::string& body, const std::map<std::string, std::string>& headers) {
        std::string apiKey = extractAPIKey(headers);
        
        if (!authManager.validateAPIKey(apiKey)) {
            return "{\"success\":false,\"message\":\"API Key inválida\"}";
        }

        if (hwidSpoofer.spoofdHWID()) {
            return "{\"success\":true,\"message\":\"HWID falsificado com sucesso\",\"hwid\":\"" + hwidSpoofer.getCurrentHWID() + "\"}";
        }

        return "{\"success\":false,\"message\":\"Erro ao falsificar HWID\"}";
    }

    std::string handleGetHWID(const std::map<std::string, std::string>& params) {
        auto it = params.find("key");
        if (it == params.end() || !authManager.validateAPIKey(it->second)) {
            return "{\"success\":false,\"message\":\"API Key inválida\"}";
        }

        std::string currentHWID = hwidSpoofer.getCurrentHWID();
        return "{\"success\":true,\"hwid\":\"" + currentHWID + "\"}";
    }

    std::string handleRestore(const std::map<std::string, std::string>& headers) {
        std::string apiKey = extractAPIKey(headers);
        
        if (!authManager.validateAPIKey(apiKey)) {
            return "{\"success\":false,\"message\":\"API Key inválida\"}";
        }

        if (hwidSpoofer.restoreOriginalHWID()) {
            return "{\"success\":true,\"message\":\"HWID restaurado. Reinicie o PC.\"}";
        }

        return "{\"success\":false,\"message\":\"Erro ao restaurar HWID\"}";
    }

    std::string getLoginPage() {
        return R"(<!DOCTYPE html><html lang="pt-BR"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Stan Spoofer - Alt Spoofer</title><style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:'Segoe UI',Tahoma,Geneva,Verdana,sans-serif;background:linear-gradient(135deg,#1a0033 0%,#330066 100%);display:flex;justify-content:center;align-items:center;min-height:100vh;color:#fff}.container{display:flex;width:90%;max-width:1000px;box-shadow:0 10px 40px rgba(0,0,0,0.3);border-radius:15px;overflow:hidden}.left-section{background:linear-gradient(135deg,#6b2fbf 0%,#9d4edd 100%);padding:60px 40px;display:flex;flex-direction:column;justify-content:center;align-items:center;width:50%}.logo{font-size:32px;font-weight:bold;margin-bottom:10px}.logo span{color:#bb86fc}.tagline{font-size:14px;opacity:0.8;margin-bottom:30px}.image-placeholder{width:200px;height:250px;background:rgba(0,0,0,0.3);border-radius:10px;display:flex;align-items:center;justify-content:center}.right-section{background:#1a1a2e;padding:60px 40px;width:50%;display:flex;flex-direction:column;justify-content:center}.form-container{display:none}.form-container.active{display:block}h2{margin-bottom:30px;font-size:24px}.form-group{margin-bottom:20px}label{display:block;margin-bottom:8px;font-size:14px;opacity:0.8}input{width:100%;padding:12px;border:1px solid #333;border-radius:5px;background:#2a2a3e;color:#fff;font-size:14px;transition:border-color 0.3s}input:focus{outline:none;border-color:#bb86fc;box-shadow:0 0 10px rgba(187,134,252,0.2)}.remember-me{display:flex;align-items:center;font-size:14px;margin-bottom:20px}input[type=checkbox]{width:18px;height:18px;margin-right:10px;accent-color:#bb86fc;cursor:pointer}.form-buttons{display:flex;gap:10px;margin-bottom:20px}button{flex:1;padding:12px;border:none;border-radius:5px;font-size:16px;font-weight:bold;cursor:pointer;transition:all 0.3s}.btn-primary{background:linear-gradient(135deg,#bb86fc 0%,#9d4edd 100%);color:#fff}.btn-primary:hover{transform:translateY(-2px);box-shadow:0 5px 20px rgba(187,134,252,0.4)}.btn-secondary{background:#2a2a3e;color:#bb86fc;border:1px solid #bb86fc}.btn-secondary:hover{background:#3a3a4e}.toggle-form{text-align:center;font-size:14px;opacity:0.7}.toggle-form a{color:#bb86fc;cursor:pointer;text-decoration:underline}.toggle-form a:hover{opacity:0.8}.message{padding:12px;border-radius:5px;margin-bottom:20px;display:none;font-size:14px}.message.success{background:#1b5e20;color:#4caf50;display:block}.message.error{background:#b71c1c;color:#ff5252;display:block}@media (max-width:768px){.container{flex-direction:column}.left-section,.right-section{width:100%}.left-section{padding:40px 20px}}</style></head><body><div class="container"><div class="left-section"><div class="logo">Stan<span>Spoofer</span></div><div class="tagline">HWID Spoofer - Alt Generator</div><div class="image-placeholder"><img src="data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 200 250'%3E%3Crect fill='%23bb86fc' width='200' height='250'/%3E%3Ctext x='50%' y='50%' font-size='48' fill='%23fff' text-anchor='middle' dy='.3em'%3E%F0%9F%94%90%3C/text%3E%3C/svg%3E" alt="Alt Spoofer" style="width:100%;height:100%;object-fit:cover;"></div></div><div class="right-section"><div id="loginForm" class="form-container active"><h2>Login</h2><div id="loginMessage" class="message"></div><div class="form-group"><label for="loginUsername">Usuário</label><input type="text" id="loginUsername" placeholder="Digite seu usuário" required></div><div class="form-group"><label for="loginPassword">Senha</label><input type="password" id="loginPassword" placeholder="Digite sua senha" required></div><div class="remember-me"><input type="checkbox" id="rememberMe"><label for="rememberMe" style="margin:0;">Lembrar-me</label></div><div class="form-buttons"><button class="btn-primary" onclick="handleLogin()">Login</button></div><div class="toggle-form">Não tem conta? <a onclick="toggleForm()">Registre-se aqui</a></div></div><div id="registerForm" class="form-container"><h2>Registrar</h2><div id="registerMessage" class="message"></div><div class="form-group"><label for="regUsername">Usuário</label><input type="text" id="regUsername" placeholder="Escolha um usuário" required></div><div class="form-group"><label for="regEmail">Email</label><input type="email" id="regEmail" placeholder="seu@email.com" required></div><div class="form-group"><label for="regPassword">Senha</label><input type="password" id="regPassword" placeholder="Escolha uma senha forte" required></div><div class="form-buttons"><button class="btn-primary" onclick="handleRegister()">Registrar</button><button class="btn-secondary" onclick="toggleForm()">Voltar</button></div><div class="toggle-form">Já tem conta? <a onclick="toggleForm()">Faça login aqui</a></div></div></div></div><script>function toggleForm(){document.getElementById('loginForm').classList.toggle('active');document.getElementById('registerForm').classList.toggle('active');document.getElementById('loginMessage').className='message';document.getElementById('registerMessage').className='message'}async function handleLogin(){const username=document.getElementById('loginUsername').value;const password=document.getElementById('loginPassword').value;const messageDiv=document.getElementById('loginMessage');if(!username||!password){messageDiv.className='message error';messageDiv.textContent='Preenchha todos os campos!';return}try{const response=await fetch('/api/login',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({username,password})});const data=await response.json();if(data.success){messageDiv.className='message success';messageDiv.textContent='Login bem-sucedido! Redirecionando...';setTimeout(()=>{window.location.href='/dashboard'},2000)}else{messageDiv.className='message error';messageDiv.textContent=data.message||'Erro ao fazer login'}}catch(error){messageDiv.className='message error';messageDiv.textContent='Erro de conexão: '+error}}async function handleRegister(){const username=document.getElementById('regUsername').value;const email=document.getElementById('regEmail').value;const password=document.getElementById('regPassword').value;const messageDiv=document.getElementById('registerMessage');if(!username||!email||!password){messageDiv.className='message error';messageDiv.textContent='Preenchha todos os campos!';return}try{const response=await fetch('/api/register',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({username,email,password})});const data=await response.json();if(data.success){messageDiv.className='message success';messageDiv.textContent='Registro bem-sucedido! Faça login para continuar.';setTimeout(()=>{toggleForm()},2000)}else{messageDiv.className='message error';messageDiv.textContent=data.message||'Erro ao registrar'}}catch(error){messageDiv.className='message error';messageDiv.textContent='Erro de conexão: '+error}}</script></body></html>)";
    }

    void start() {
        httpServer.start();
    }

private:
    std::string extractJSONValue(const std::string& json, const std::string& key) {
        std::string searchString = "\"" + key + "\":\"";
        size_t startPos = json.find(searchString);
        
        if (startPos == std::string::npos) return "";
        
        startPos += searchString.length();
        size_t endPos = json.find("\"", startPos);
        
        if (endPos == std::string::npos) return "";
        
        return json.substr(startPos, endPos - startPos);
    }

    std::string extractAPIKey(const std::map<std::string, std::string>& headers) {
        auto it = headers.find("Authorization");
        if (it != headers.end()) {
            std::string auth = it->second;
            if (auth.find("Bearer ") == 0) {
                return auth.substr(7);
            }
        }
        return "";
    }
};