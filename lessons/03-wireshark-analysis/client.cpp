/**
 * Lesson 03: Wiresharkによるパケット解析 - クライアント側プログラム
 *
 * このプログラムはWiresharkでのパケット解析を学ぶためのシンプルなクライアントです。
 * 各ステップでWiresharkで観察すべきポイントをコメントで説明しています。
 *
 * Wiresharkで観察できるもの:
 * - 3ウェイハンドシェイク（SYN → SYN-ACK → ACK）
 * - データ転送（PSHフラグ付きパケット）
 * - 4ウェイハンドシェイク（FIN → ACK → FIN → ACK）
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

constexpr const char* SERVER_IP = "127.0.0.1";
constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

int main() {
    std::cout << "=== Wireshark パケット解析クライアント ===" << std::endl;
    std::cout << std::endl;

    // ソケットの作成
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        std::cerr << "エラー: ソケットの作成に失敗しました" << std::endl;
        return 1;
    }

    // サーバーアドレスの設定
    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    // ========================================
    // サーバーに接続
    // 【Wireshark】ここで3ウェイハンドシェイクが発生
    //   - SYN: クライアント → サーバー（接続要求）
    //   - SYN-ACK: サーバー → クライアント（要求受諾）
    //   - ACK: クライアント → サーバー（接続確立）
    // ========================================
    std::cout << "サーバーに接続中... (" << SERVER_IP << ":" << PORT << ")" << std::endl;

    if (connect(sock_fd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
        std::cerr << "エラー: サーバーへの接続に失敗しました" << std::endl;
        std::cerr << "対処法: サーバーが起動しているか確認してください" << std::endl;
        close(sock_fd);
        return 1;
    }

    std::cout << "✓ 接続成功！" << std::endl;
    std::cout << "【Wireshark確認】3ウェイハンドシェイク（SYN → SYN-ACK → ACK）を確認してください" << std::endl;
    std::cout << std::endl;

    // 少し待機（Wiresharkで確認しやすくするため）
    sleep(1);

    // ========================================
    // データの送信
    // 【Wireshark】PSHフラグ付きのデータパケット
    //   - クライアント → サーバー: "Hello, TCP!" + PSH + ACK
    //   - サーバー → クライアント: ACK（確認応答）
    // シーケンス番号とACK番号の変化に注目
    // ========================================
    const char* message = "Hello, TCP!\n";
    std::cout << "データを送信中: " << message;

    send(sock_fd, message, strlen(message), 0);

    std::cout << "✓ 送信完了" << std::endl;
    std::cout << "【Wireshark確認】PSHフラグ付きパケットとACK応答を確認してください" << std::endl;
    std::cout << std::endl;

    // 少し待機
    sleep(1);

    // ========================================
    // サーバーからの応答を受信
    // 【Wireshark】サーバーからのデータパケット
    //   - サーバー → クライアント: 応答データ + PSH + ACK
    //   - クライアント → サーバー: ACK（確認応答）
    // ========================================
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    std::cout << "サーバーからの応答を待機中..." << std::endl;
    ssize_t bytes_received = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received > 0) {
        std::cout << "✓ 受信データ: " << buffer;
        std::cout << std::endl;
    }

    // 少し待機
    sleep(1);

    // ========================================
    // 接続の終了
    // 【Wireshark】4ウェイハンドシェイク
    //   - FIN + ACK: クライアント → サーバー（終了要求）
    //   - ACK: サーバー → クライアント（要求受諾）
    //   - FIN + ACK: サーバー → クライアント（サーバー側終了）
    //   - ACK: クライアント → サーバー（接続終了完了）
    // ========================================
    std::cout << "接続を終了します..." << std::endl;
    close(sock_fd);

    std::cout << "✓ 接続を終了しました" << std::endl;
    std::cout << "【Wireshark確認】4ウェイハンドシェイク（FIN → ACK → FIN → ACK）を確認してください" << std::endl;
    std::cout << std::endl;
    std::cout << "=== パケット解析完了 ===" << std::endl;
    std::cout << std::endl;
    std::cout << "【次のステップ】" << std::endl;
    std::cout << "Wiresharkでキャプチャを停止し、パケットを詳しく確認してください。" << std::endl;
    std::cout << "詳細な操作方法は wireshark_guide.md を参照してください。" << std::endl;

    return 0;
}
