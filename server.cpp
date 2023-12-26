#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <chrono>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // 初始化Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock.\n";
        return 1;
    }

    // 创建UDP Socket
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.\n";
        WSACleanup();
        return 1;
    }

    // 设置服务器地址
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 服务器的IP地址，替换为你的服务器IP地址
    // serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(11255);  // 选择一个固定端口，例如12345

    // 绑定Socket到端口
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "UDP Server is listening on port 11255...\n";

    // 创建文件流
    std::ofstream outputFile("transmission_delay.txt");
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open output file.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 接收数据
    char buffer[1024];
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    while (true) {
        int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "recvfrom failed with error.\n";
            break;
        }

        buffer[bytesReceived] = '\0';
        std::cout << "Received message from " << inet_ntoa(clientAddr.sin_addr) << ": " << buffer << std::endl;

        // 解析接收到的时间信息
        long long sentTime;
        std::sscanf(buffer, "%lld", &sentTime);

        // 计算传输时延
        auto receiveTime = std::chrono::system_clock::now();
        auto timeDifference = receiveTime.time_since_epoch().count() - sentTime;
        // std::cout << "Transmission delay: " << timeDifference << " ms\n";


        // 将时延转换为毫秒
        auto timeDifferenceInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(timeDifference));
        // 输出时延（毫秒）
        std::cout << "Transmission delay: " << timeDifferenceInMilliseconds.count() << " ms\n";


        // 将传输时延写入txt文件
        outputFile << timeDifferenceInMilliseconds.count() << std::endl;
        // outputFile << timeDifferenceInMilliseconds.count() << std::endl;
    }

    // 关闭Socket和清理Winsock
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
