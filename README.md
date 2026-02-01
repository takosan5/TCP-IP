# TCP/IP Learning Kit

TCP/IP通信をプログラムを動かしながら理解するための教育プロジェクトです。

## 概要

このプロジェクトは、TCP/IPに触れたことがない人がリポジトリをクローンして手元で動作確認しながら学べることを目的としています。

C++とPOSIXソケットAPIを使用し、TCP/IPの低レベルな動作を直接理解できます。

## 前提条件

- Linux環境（Ubuntu 20.04以上推奨）
- g++（C++17対応）
- make

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install g++ build-essential make
```

## クイックスタート

```bash
# リポジトリをクローン
git clone https://github.com/<your-username>/tcpip-learning-kit.git
cd tcpip-learning-kit

# Lesson 01 を試す
cd lessons/01-echo-server
make

# ターミナル1: サーバー起動
./server

# ターミナル2: クライアント起動
./client
```

## レッスン構成

| レッスン | 内容 | 学習するソケットAPI |
|----------|------|---------------------|
| [01-echo-server](lessons/01-echo-server/) | エコーサーバー | socket, bind, listen, accept, send, recv, close |
| [02-chat](lessons/02-chat/) | 双方向チャット | select, FD_ZERO, FD_SET, FD_ISSET |
| [03-wireshark-analysis](lessons/03-wireshark-analysis/) | パケット解析 | tcpdump, Wireshark |

## 学べること

### Lesson 01: Echo Server

クライアントから送られたメッセージをそのまま返すサーバーを実装します。

```
クライアント          サーバー
    |                    |
    |--- "Hello" ------->|
    |<-- "Hello" --------|
```

学習するソケットAPI：
- `socket()` - ソケットの作成
- `bind()` - アドレスとポートの紐付け
- `listen()` - 接続待ち状態への移行
- `accept()` - クライアント接続の受け入れ
- `connect()` - サーバーへの接続
- `send()` / `recv()` - データの送受信
- `close()` - ソケットの終了

### Lesson 02: Bidirectional Chat

`select()`を使用して、サーバーとクライアント間で双方向にメッセージをやり取りできるチャットを実装します。

```
クライアント          サーバー
    |                    |
    |--- "こんにちは" -->|
    |<-- "やあ！" -------|
    |--- "元気？" ------>|
    |<-- "元気だよ" -----|
```

学習するソケットAPI：
- `select()` - 複数のファイルディスクリプタを同時に監視
- `FD_ZERO()` - fd_setを初期化
- `FD_SET()` - fd_setにfdを追加
- `FD_ISSET()` - fdがセットされているか確認

### Lesson 03: Wireshark Packet Analysis

tcpdumpとWiresharkを使用して、TCP/IP通信のパケットを解析します。

```
3ウェイハンドシェイク:
    [SYN]     クライアント → サーバー
    [SYN-ACK] サーバー → クライアント
    [ACK]     クライアント → サーバー

データ転送:
    [PSH,ACK] データ送信
    [ACK]     受信確認

4ウェイハンドシェイク:
    [FIN,ACK] 終了要求
    [ACK]     確認
```

学習する内容：
- `tcpdump` - パケットキャプチャ（pcapファイル生成）
- `Wireshark` - パケット解析（3ウェイハンドシェイク、データ転送、4ウェイハンドシェイク）

## ドキュメント

- [環境セットアップ](docs/SETUP.md)
- [トラブルシューティング](docs/TROUBLESHOOTING.md)

## ライセンス

教育目的で自由に使用できます。
