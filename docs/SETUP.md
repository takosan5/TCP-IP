# 環境セットアップガイド

## 必要なツール

### 1. C++コンパイラ（必須）

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install g++ build-essential

# バージョン確認（C++17対応が必要）
g++ --version
```

### 2. Make（必須）

```bash
# Ubuntu/Debian
sudo apt install make

# バージョン確認
make --version
```

### 3. Wireshark（Lesson 03用、オプション）

```bash
# Ubuntu/Debian
sudo apt install wireshark

# 一般ユーザーでキャプチャを許可（推奨）
sudo usermod -aG wireshark $USER
# 注意: グループ変更を反映するには再ログインが必要

# バージョン確認
wireshark --version
```

## 動作確認

### コンパイラのテスト

```bash
# テスト用ファイル作成
echo '#include <iostream>
int main() { std::cout << "Hello, TCP/IP!" << std::endl; return 0; }' > test.cpp

# コンパイル
g++ -std=c++17 -o test test.cpp

# 実行
./test
# 出力: Hello, TCP/IP!

# クリーンアップ
rm test.cpp test
```

### ポート8080の確認

```bash
# ポート8080が使用されていないか確認
lsof -i :8080

# 何も表示されなければOK
# プロセスが表示された場合は、そのプロセスを終了するか別のポートを使用
```

## トラブルシューティング

問題が発生した場合は [TROUBLESHOOTING.md](./TROUBLESHOOTING.md) を参照してください。
