/**
 * Lesson 02: 双方向チャット - クライアント側プログラム
 *
 * このプログラムは、select()を使用して双方向通信を実現する
 * チャットクライアントです。サーバーからのメッセージを受信しながら、
 * 同時にキーボード入力も受け付けます。
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
#include <sys/socket.h> // socket(), connect(), send(), recv()
#include <sys/select.h> // select(), fd_set, FD_ZERO, FD_SET, FD_ISSET
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_pton()

// 接続先のサーバー情報
constexpr const char* SERVER_IP = "127.0.0.1";  // ローカルホスト
constexpr int PORT = 8080;

// バッファサイズ（受信データの最大長）
constexpr int BUFFER_SIZE = 1024;

int main() {
    // ========================================
    // ステップ1: ソケットの作成
    // ========================================
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd < 0) {
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
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // IPアドレスを文字列からバイナリ形式に変換
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        std::cerr << "エラー: 無効なIPアドレスです: " << SERVER_IP << std::endl;
        std::cerr << "対処法: 正しいIPv4アドレスを指定してください" << std::endl;
        close(sock_fd);
        return 1;
    }

    // ========================================
    // ステップ3: サーバーに接続
    // ========================================
    if (connect(sock_fd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
        std::cerr << "エラー: サーバーへの接続に失敗しました" << std::endl;
        std::cerr << "原因: 以下のいずれかの可能性があります" << std::endl;
        std::cerr << "  - サーバーが起動していない" << std::endl;
        std::cerr << "  - IPアドレスまたはポート番号が間違っている" << std::endl;
        std::cerr << "  - ファイアウォールがブロックしている" << std::endl;
        std::cerr << "対処法: サーバーを先に起動してから再度接続してください" << std::endl;
        close(sock_fd);
        return 1;
    }

    std::cout << "サーバーに接続しました (" << SERVER_IP << ":" << PORT << ")" << std::endl;
    std::cout << "メッセージを入力してください（終了するには 'quit' と入力）:" << std::endl;

    // ========================================
    // ステップ4: select()による双方向通信ループ
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
        // sock_fd: サーバーソケット
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(sock_fd, &read_fds);

        // 監視するfdの最大値+1（select()の第1引数に必要）
        int max_fd = (sock_fd > STDIN_FILENO) ? sock_fd : STDIN_FILENO;

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

            // サーバーにメッセージを送信
            input += "\n";  // 改行を追加
            send(sock_fd, input.c_str(), input.length(), 0);
        }

        // ----------------------------------------
        // サーバーからのメッセージをチェック
        // ----------------------------------------
        if (FD_ISSET(sock_fd, &read_fds)) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t bytes_received = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);

            if (bytes_received <= 0) {
                // サーバーが切断した
                std::cout << "サーバーとの接続が切断されました。" << std::endl;
                break;
            }

            // 受信したメッセージを表示
            std::cout << "サーバー: " << buffer;
        }
    }

    // ========================================
    // ステップ5: ソケットを閉じる
    // ========================================
    close(sock_fd);

    std::cout << "クライアントを終了しました。" << std::endl;
    return 0;
}
