#include <iostream>
#include <winsock2.h>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // 初始化Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock.\n";
        return 1;
    }

    // 创建UDP Socket
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.\n";
        WSACleanup();
        return 1;
    }

    // 设置服务器地址
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器的IP地址，替换为你的服务器IP地址
    serverAddr.sin_port = htons(11255);  // 选择一个固定端口，例如12345

    // 获取起始时间
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {

        // 设置通信频率
        if (std::chrono::steady_clock::now() - startTime >= std::chrono::milliseconds(50)) {
            // 获取当前时间（ns单位）
            auto now = std::chrono::system_clock::now().time_since_epoch().count();

            // 发送当前时间
            if (sendto(clientSocket, std::to_string(now).c_str(), std::to_string(now).size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
                std::cerr << "sendto failed with error.\n";
                closesocket(clientSocket);
                WSACleanup();
                return 1;
            }

            std::cout << "Sent time to server: " << now << std::endl;
            // 重置起始时间
            startTime = std::chrono::steady_clock::now();
        }
    }

    // 关闭Socket和清理Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
