# TCP/IP Learning Kit

TCP/IP通信をプログラムを動かしながら理解するための教育プロジェクトです。

## 概要

このプロジェクトは、TCP/IPに触れたことがない人がリポジトリをクローンして手元で動作確認しながら学べることを目的としています。C++とPOSIXソケットAPIを使用し、TCP/IPの低レベルな動作を直接理解できます。

## 前提条件

- Linux環境（Ubuntu 20.04以上推奨）
- g++（C++17対応）
- make
- Wireshark（Lesson 03用、オプション）

## レッスン構成

| レッスン | 内容 | 学習目標 |
|----------|------|----------|
| [01-echo-server](../lessons/01-echo-server/) | エコーサーバー | socket(), bind(), listen(), accept(), send(), recv(), close() |
| [02-chat](../lessons/02-chat/) | 双方向チャット | select()による非同期I/O |
| [03-wireshark-analysis](../lessons/03-wireshark-analysis/) | パケット解析 | 3ウェイハンドシェイク、データ転送、4ウェイハンドシェイク |

## クイックスタート

```bash
# Lesson 01 を試す
cd lessons/01-echo-server
make

# ターミナル1: サーバー起動
./server

# ターミナル2: クライアント起動
./client
```

## ドキュメント

- [SETUP.md](./SETUP.md) - 環境セットアップガイド
- [TROUBLESHOOTING.md](./TROUBLESHOOTING.md) - よくある問題と解決策

## ライセンス

教育目的で自由に使用できます。
