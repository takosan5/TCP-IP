/**
 * Lesson 01: Echo Server - クライアント側プログラム
 *
 * このプログラムは、サーバーに接続してメッセージを送信し、
 * サーバーからの返答（エコー）を受信して表示します。
 *
 * 学習ポイント:
 * - socket(): ソケットの作成
 * - connect(): サーバーへの接続
 * - send(): データの送信
 * - recv(): データの受信
 * - close(): ソケットの終了
 */

#include <iostream>     // 標準入出力（std::cout, std::cerr, std::cin）
#include <string>       // std::string, std::getline()
#include <cstring>      // memset(), strlen()
#include <unistd.h>     // close()
#include <sys/socket.h> // socket(), connect(), send(), recv()
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_pton()

// 接続先のサーバー設定
constexpr const char* SERVER_IP = "127.0.0.1";  // localhost
constexpr int PORT = 8080;

// バッファサイズ（受信データの最大長）
constexpr int BUFFER_SIZE = 1024;

int main() {
    // ========================================
    // ステップ1: ソケットの作成
    // ========================================
    // socket(ドメイン, タイプ, プロトコル)
    // - AF_INET: IPv4を使用
    // - SOCK_STREAM: TCP（信頼性のあるストリーム通信）
    // - 0: プロトコルを自動選択
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        std::cerr << "エラー: ソケットの作成に失敗しました" << std::endl;
        std::cerr << "原因: システムリソースが不足している可能性があります" << std::endl;
        std::cerr << "対処法: 他のプログラムを終了してリソースを解放してください" << std::endl;
        return 1;
    }

    // ========================================
    // ステップ2: サーバーアドレスの設定
    // ========================================
    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;      // IPv4
    server_address.sin_port = htons(PORT);    // ポート番号（ネットワークバイトオーダー）

    // IPアドレスを文字列からバイナリ形式に変換
    // inet_pton: "presentation to network" の略
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        std::cerr << "エラー: 無効なIPアドレスです: " << SERVER_IP << std::endl;
        std::cerr << "対処法: 正しいIPアドレス形式（例: 127.0.0.1）を使用してください" << std::endl;
        close(sock);
        return 1;
    }

    // ========================================
    // ステップ3: サーバーに接続
    // ========================================
    // connect(ソケット, サーバーアドレス, アドレス長)
    // TCPの3ウェイハンドシェイクがここで行われる
    if (connect(sock, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
        std::cerr << "エラー: サーバーへの接続に失敗しました" << std::endl;
        std::cerr << "原因: サーバーが起動していないか、ポート番号が間違っています" << std::endl;
        std::cerr << "対処法: " << std::endl;
        std::cerr << "  1. サーバー（./server）が起動しているか確認してください" << std::endl;
        std::cerr << "  2. ポート番号 " << PORT << " が正しいか確認してください" << std::endl;
        std::cerr << "  3. ファイアウォールがブロックしていないか確認してください" << std::endl;
        close(sock);
        return 1;
    }

    std::cout << "サーバーに接続しました。" << std::endl;
    std::cout << "メッセージを入力してください（終了するには 'quit' と入力）:" << std::endl;

    // ========================================
    // ステップ4: メッセージ送受信ループ
    // ========================================
    std::string message;
    char buffer[BUFFER_SIZE];

    while (true) {
        // ユーザーからの入力を取得
        std::cout << "> ";
        std::getline(std::cin, message);

        // 終了コマンドのチェック
        if (message == "quit") {
            std::cout << "接続を終了します。" << std::endl;
            break;
        }

        // 空のメッセージはスキップ
        if (message.empty()) {
            continue;
        }

        // 改行を追加（サーバー側で表示しやすくするため）
        message += "\n";

        // send(ソケット, データ, データ長, フラグ)
        // サーバーにメッセージを送信
        ssize_t bytes_sent = send(sock, message.c_str(), message.length(), 0);

        if (bytes_sent < 0) {
            std::cerr << "エラー: メッセージの送信に失敗しました" << std::endl;
            break;
        }

        // バッファをクリア
        memset(buffer, 0, BUFFER_SIZE);

        // recv(ソケット, バッファ, バッファサイズ, フラグ)
        // サーバーからの返答を受信
        ssize_t bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0) {
            std::cerr << "エラー: サーバーとの接続が切断されました" << std::endl;
            std::cerr << "原因: サーバーが予期せず終了した可能性があります" << std::endl;
            std::cerr << "対処法: サーバーを再起動してください" << std::endl;
            break;
        }

        // サーバーからの返答を表示
        std::cout << "サーバーからの返答: " << buffer;
    }

    // ========================================
    // ステップ5: ソケットを閉じる
    // ========================================
    // TCPの4ウェイハンドシェイクがここで行われる
    close(sock);

    return 0;
}
