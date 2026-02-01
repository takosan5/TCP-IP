/**
 * Lesson 02: 双方向チャット - サーバー側プログラム
 *
 * このプログラムは、select()を使用して双方向通信を実現する
 * チャットサーバーです。サーバー側からもメッセージを送信できます。
 *
 * 学習ポイント:
 * - select(): 複数のファイルディスクリプタを同時に監視
 * - FD_ZERO(), FD_SET(), FD_ISSET(): fd_setの操作
 * - 標準入力とソケットの同時監視
 */

#include <iostream>     // 標準入出力（std::cout, std::cerr, std::cin）
#include <string>       // std::string, std::getline()
#include <cstring>      // memset(), strlen()
#include <unistd.h>     // close(), STDIN_FILENO
#include <sys/socket.h> // socket(), bind(), listen(), accept(), send(), recv()
#include <sys/select.h> // select(), fd_set, FD_ZERO, FD_SET, FD_ISSET
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
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        std::cerr << "エラー: ソケットの作成に失敗しました" << std::endl;
        std::cerr << "原因: システムリソースが不足している可能性があります" << std::endl;
        std::cerr << "対処法: 他のプログラムを終了してリソースを解放してください" << std::endl;
        return 1;
    }

    // ソケットオプション: アドレスの再利用を許可
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "エラー: ソケットオプションの設定に失敗しました" << std::endl;
        close(server_fd);
        return 1;
    }

    // ========================================
    // ステップ2: アドレス構造体の設定とバインド
    // ========================================
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        std::cerr << "エラー: ポートへのバインドに失敗しました" << std::endl;
        std::cerr << "原因: ポート " << PORT << " は既に使用中の可能性があります" << std::endl;
        std::cerr << "対処法: 'lsof -i :" << PORT << "' で使用中のプロセスを確認し、" << std::endl;
        std::cerr << "        終了するか、別のポート番号を使用してください" << std::endl;
        close(server_fd);
        return 1;
    }

    // ========================================
    // ステップ3: 接続待ち状態に移行
    // ========================================
    if (listen(server_fd, 3) < 0) {
        std::cerr << "エラー: listen()に失敗しました" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "サーバーを起動しました。ポート " << PORT << " で接続を待っています..." << std::endl;

    // ========================================
    // ステップ4: クライアント接続の受け入れ
    // ========================================
    sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_address), &client_len);

    if (client_fd < 0) {
        std::cerr << "エラー: クライアント接続の受け入れに失敗しました" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "クライアントが接続しました: " << inet_ntoa(client_address.sin_addr) << std::endl;
    std::cout << "メッセージを入力してください（終了するには 'quit' と入力）:" << std::endl;

    // ========================================
    // ステップ5: select()による双方向通信ループ
    // ========================================
    char buffer[BUFFER_SIZE];
    std::string input;
    bool running = true;

    while (running) {
        // ----------------------------------------
        // fd_setの初期化と設定
        // ----------------------------------------
        // fd_set: 監視するファイルディスクリプタの集合
        fd_set read_fds;

        // FD_ZERO(): fd_setを空に初期化
        // 毎回のループで初期化が必要（select()がfd_setを変更するため）
        FD_ZERO(&read_fds);

        // FD_SET(): 監視対象のfdを追加
        // STDIN_FILENO (0): 標準入力（キーボード）
        // client_fd: クライアントソケット
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(client_fd, &read_fds);

        // 監視するfdの最大値+1（select()の第1引数に必要）
        int max_fd = (client_fd > STDIN_FILENO) ? client_fd : STDIN_FILENO;

        // ----------------------------------------
        // select()で入力を待つ
        // ----------------------------------------
        // select(最大fd+1, 読み込み監視, 書き込み監視, 例外監視, タイムアウト)
        // - 読み込み可能なfdがあるまでブロック
        // - タイムアウトがNULLの場合、無限に待つ
        int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);

        if (activity < 0) {
            std::cerr << "エラー: select()に失敗しました" << std::endl;
            break;
        }

        // ----------------------------------------
        // 標準入力からの入力をチェック
        // ----------------------------------------
        // FD_ISSET(): 指定したfdが読み込み可能かどうかを確認
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            // キーボードからの入力を取得
            if (!std::getline(std::cin, input)) {
                // EOF（Ctrl+D）が入力された場合
                std::cout << "入力が終了しました。接続を閉じます。" << std::endl;
                break;
            }

            // "quit"が入力されたら終了
            if (input == "quit") {
                std::cout << "チャットを終了します。" << std::endl;
                break;
            }

            // クライアントにメッセージを送信
            input += "\n";  // 改行を追加
            send(client_fd, input.c_str(), input.length(), 0);
        }

        // ----------------------------------------
        // クライアントからのメッセージをチェック
        // ----------------------------------------
        if (FD_ISSET(client_fd, &read_fds)) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

            if (bytes_received <= 0) {
                // クライアントが切断した
                std::cout << "クライアントが切断しました。" << std::endl;
                break;
            }

            // 受信したメッセージを表示
            std::cout << "クライアント: " << buffer;
        }
    }

    // ========================================
    // ステップ6: ソケットを閉じる
    // ========================================
    close(client_fd);
    close(server_fd);

    std::cout << "サーバーを終了しました。" << std::endl;
    return 0;
}
