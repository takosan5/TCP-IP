/**
 * Lesson 01: Echo Server - サーバー側プログラム
 *
 * このプログラムは、クライアントから受信したメッセージを
 * そのまま送り返す「エコーサーバー」です。
 *
 * 学習ポイント:
 * - socket(): ソケットの作成
 * - bind(): アドレスとポートの紐付け
 * - listen(): 接続待ち状態への移行
 * - accept(): クライアント接続の受け入れ
 * - recv(): データの受信
 * - send(): データの送信
 * - close(): ソケットの終了
 */

#include <iostream>     // 標準入出力（std::cout, std::cerr）
#include <cstring>      // memset(), strlen()
#include <unistd.h>     // close()
#include <sys/socket.h> // socket(), bind(), listen(), accept(), send(), recv()
#include <netinet/in.h> // sockaddr_in, INADDR_ANY
#include <arpa/inet.h>  // inet_ntoa()

// 使用するポート番号（1024以上でroot権限不要）
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
    // - 0: プロトコルを自動選択（TCPの場合はTCPIPROTO_TCP）
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        std::cerr << "エラー: ソケットの作成に失敗しました" << std::endl;
        std::cerr << "原因: システムリソースが不足している可能性があります" << std::endl;
        std::cerr << "対処法: 他のプログラムを終了してリソースを解放してください" << std::endl;
        return 1;
    }

    // ソケットオプション: アドレスの再利用を許可
    // これにより、サーバー再起動時に「Address already in use」を回避
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "エラー: ソケットオプションの設定に失敗しました" << std::endl;
        close(server_fd);
        return 1;
    }

    // ========================================
    // ステップ2: アドレス構造体の設定
    // ========================================
    // sockaddr_in: IPv4アドレスを格納する構造体
    sockaddr_in address;
    memset(&address, 0, sizeof(address));  // 構造体をゼロで初期化

    address.sin_family = AF_INET;          // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // すべてのネットワークインターフェースで待ち受け
    address.sin_port = htons(PORT);        // ポート番号（ネットワークバイトオーダーに変換）

    // ========================================
    // ステップ3: ソケットにアドレスをバインド
    // ========================================
    // bind(ソケット, アドレス構造体, 構造体のサイズ)
    // ソケットに特定のIPアドレスとポート番号を紐付ける
    if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        std::cerr << "エラー: ポートへのバインドに失敗しました" << std::endl;
        std::cerr << "原因: ポート " << PORT << " は既に使用中の可能性があります" << std::endl;
        std::cerr << "対処法: 'lsof -i :" << PORT << "' で使用中のプロセスを確認し、" << std::endl;
        std::cerr << "        終了するか、別のポート番号を使用してください" << std::endl;
        close(server_fd);
        return 1;
    }

    // ========================================
    // ステップ4: 接続待ち状態に移行
    // ========================================
    // listen(ソケット, バックログ)
    // - バックログ: 保留中の接続要求の最大数
    if (listen(server_fd, 3) < 0) {
        std::cerr << "エラー: listen()に失敗しました" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "サーバーを起動しました。ポート " << PORT << " で接続を待っています..." << std::endl;

    // メインループ: 接続を待ち受け続ける
    while (true) {
        // ========================================
        // ステップ5: クライアント接続の受け入れ
        // ========================================
        // accept(ソケット, クライアントアドレス, アドレス長)
        // 新しい接続を受け入れ、通信用の新しいソケットを返す
        sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);

        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_address), &client_len);

        if (client_fd < 0) {
            std::cerr << "エラー: クライアント接続の受け入れに失敗しました" << std::endl;
            continue;  // 次の接続を待つ
        }

        // 接続したクライアントのIPアドレスを表示
        std::cout << "クライアントが接続しました: " << inet_ntoa(client_address.sin_addr) << std::endl;

        // ========================================
        // ステップ6: データの送受信ループ
        // ========================================
        char buffer[BUFFER_SIZE];

        while (true) {
            // バッファをクリア
            memset(buffer, 0, BUFFER_SIZE);

            // recv(ソケット, バッファ, バッファサイズ, フラグ)
            // クライアントからデータを受信
            ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

            if (bytes_received <= 0) {
                // 0: クライアントが接続を閉じた
                // 負: エラー発生
                std::cout << "クライアントが切断しました" << std::endl;
                break;
            }

            std::cout << "受信: " << buffer;

            // send(ソケット, データ, データ長, フラグ)
            // 受信したデータをそのまま送り返す（エコー）
            send(client_fd, buffer, strlen(buffer), 0);
        }

        // ========================================
        // ステップ7: クライアントソケットを閉じる
        // ========================================
        close(client_fd);
        std::cout << "次の接続を待っています..." << std::endl;
    }

    // サーバーソケットを閉じる（通常はここに到達しない）
    close(server_fd);
    return 0;
}
