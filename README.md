# Alt Spoofer - HWID Spoofer com Autenticação

Um aplicativo em **C++** para Visual Studio 2026 que falsifica o HWID (Hardware ID) do PC com autenticação por chave API.

## 🎯 Recursos

- ✅ **Falsificação de HWID**: Gera um HWID aleatório e o substitui no registro do Windows
- ✅ **Restauração Automática**: Volta ao HWID original após reiniciar o PC
- ✅ **Sistema de Autenticação**: Login e registro de usuários
- ✅ **API Key**: Cada usuário recebe uma chave API única
- ✅ **Interface Web**: Interface moderna com login/registro
- ✅ **API REST**: Endpoints para controlar o spoofer

## 📋 Requisitos

- **Visual Studio 2026** (ou versão recente com C++17)
- **Windows 10/11** (por causa da manipulação de registros)
- **Permissões de administrador** (necessário para modificar registro)

## 🚀 Instalação e Uso

### 1. Clonar o repositório

```bash
git clone https://github.com/TETEUAIMBY/alt-spoofer.git
cd alt-spoofer
```

### 2. Compilar com CMake

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 3. Executar como Administrador

```bash
.\AltSpoofer.exe
```

A aplicação iniciará um servidor HTTP na porta **8080**.

### 4. Acessar a Interface

Abra o navegador e acesse: **http://localhost:8080**

## 🔐 Endpoints da API

### Registrar Novo Usuário
```bash
POST /api/register
Content-Type: application/json

{
  "username": "seu_usuario",
  "email": "seu@email.com",
  "password": "sua_senha"
}
```

### Login
```bash
POST /api/login
Content-Type: application/json

{
  "username": "seu_usuario",
  "password": "sua_senha"
}
```

### Falsificar HWID
```bash
POST /api/spoof
Authorization: Bearer sk_seu_api_key_aqui
```

### Obter HWID Atual
```bash
GET /api/hwid?key=sk_seu_api_key_aqui
```

### Restaurar HWID Original
```bash
POST /api/restore
Authorization: Bearer sk_seu_api_key_aqui
```

## 📁 Estrutura do Projeto

```
alt-spoofer/
├── src/
│   ├── main.cpp
│   ├── auth/
│   │   ├── User.h
│   │   └── AuthManager.h
│   ├── hwid/
│   │   └── HWIDSpoofer.h
│   ├── server/
│   │   ├── AuthServer.h
│   │   └── HTTPServer.h
│   └── utils/
│       └── CryptoUtils.h
├── CMakeLists.txt
└── README.md
```

## ⚠️ Notas Importantes

1. **Permissões de Admin**: Necessário executar como administrador
2. **Reinicialização**: HWID falsificado volta ao original após reiniciar o PC
3. **Backup**: Faça backup do seu HWID original
4. **Antivírus**: Alguns antivírus podem marcar como suspeito

## 🔒 Segurança

- Senhas são hasheadas antes do armazenamento
- Chaves API únicas para cada usuário
- Banco de dados local em arquivo (`users.db`)

---

**Desenvolvido por**: TETEUAIMBY  
**Data**: Setembro 2026