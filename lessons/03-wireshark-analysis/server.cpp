/**
 * Lesson 03: Wiresharkによるパケット解析 - サーバー側プログラム
 *
 * このプログラムはWiresharkでのパケット解析を学ぶためのシンプルなサーバーです。
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

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

int main() {
    // ソケットの作成
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "エラー: ソケットの作成に失敗しました" << std::endl;
        return 1;
    }

    // アドレスの再利用を許可
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // アドレス構造体の設定
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // バインド
    if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        std::cerr << "エラー: ポート " << PORT << " へのバインドに失敗しました" << std::endl;
        std::cerr << "対処法: 'lsof -i :" << PORT << "' で使用中のプロセスを確認してください" << std::endl;
        close(server_fd);
        return 1;
    }

    // リッスン開始
    listen(server_fd, 1);
    std::cout << "=== Wireshark パケット解析サーバー ===" << std::endl;
    std::cout << "ポート " << PORT << " で接続を待っています..." << std::endl;
    std::cout << std::endl;
    std::cout << "【Wiresharkの準備】" << std::endl;
    std::cout << "1. Wiresharkを起動し、ループバック(lo)インターフェースを選択" << std::endl;
    std::cout << "2. キャプチャフィルタに 'port 8080' を設定" << std::endl;
    std::cout << "3. キャプチャを開始" << std::endl;
    std::cout << std::endl;

    // ========================================
    // クライアント接続の受け入れ
    // 【Wireshark】ここで3ウェイハンドシェイクが発生
    //   - SYN: クライアント → サーバー
    //   - SYN-ACK: サーバー → クライアント
    //   - ACK: クライアント → サーバー
    // ========================================
    sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    std::cout << "クライアントの接続を待機中..." << std::endl;
    int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_address), &client_len);

    if (client_fd < 0) {
        std::cerr << "エラー: 接続の受け入れに失敗しました" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << std::endl;
    std::cout << "✓ クライアントが接続しました: " << inet_ntoa(client_address.sin_addr) << std::endl;
    std::cout << "【Wireshark確認】3ウェイハンドシェイク（SYN → SYN-ACK → ACK）を確認してください" << std::endl;
    std::cout << std::endl;

    // ========================================
    // データの受信
    // 【Wireshark】PSHフラグ付きのデータパケットが見える
    //   - クライアント → サーバー: データ + PSH + ACK
    //   - サーバー → クライアント: ACK（確認応答）
    // ========================================
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    std::cout << "データ受信を待機中..." << std::endl;
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received > 0) {
        std::cout << "✓ 受信データ: " << buffer;
        std::cout << "【Wireshark確認】PSHフラグ付きパケットとACKを確認してください" << std::endl;
        std::cout << std::endl;

        // ========================================
        // データの送信（応答）
        // 【Wireshark】サーバーからの応答パケット
        //   - サーバー → クライアント: データ + PSH + ACK
        //   - クライアント → サーバー: ACK（確認応答）
        // ========================================
        const char* response = "サーバーからの応答です\n";
        send(client_fd, response, strlen(response), 0);
        std::cout << "✓ 応答を送信しました" << std::endl;
        std::cout << std::endl;
    }

    // ========================================
    // 接続の終了
    // 【Wireshark】4ウェイハンドシェイクが発生
    //   - FIN + ACK: サーバー → クライアント
    //   - ACK: クライアント → サーバー
    //   - FIN + ACK: クライアント → サーバー
    //   - ACK: サーバー → クライアント
    // ========================================
    std::cout << "接続を終了します..." << std::endl;
    close(client_fd);
    close(server_fd);

    std::cout << "✓ 接続を終了しました" << std::endl;
    std::cout << "【Wireshark確認】4ウェイハンドシェイク（FIN → ACK → FIN → ACK）を確認してください" << std::endl;
    std::cout << std::endl;
    std::cout << "=== パケット解析完了 ===" << std::endl;

    return 0;
}
