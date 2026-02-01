# Lesson 03: Wiresharkによるパケット解析

## 学習目標

このレッスンでは、**Wireshark**を使用してTCP/IP通信の内部動作を視覚的に理解します。

実際のパケットをキャプチャして、以下を確認します：
- **3ウェイハンドシェイク**: TCP接続の確立（SYN → SYN-ACK → ACK）
- **データ転送**: アプリケーションデータの送受信
- **4ウェイハンドシェイク**: TCP接続の終了（FIN → ACK → FIN → ACK）

## 前提知識

- Lesson 01（エコーサーバー）を完了していること
- Wiresharkがインストールされていること

## Wiresharkのインストール

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install wireshark

# インストール中に「非特権ユーザーがパケットをキャプチャできるようにしますか？」
# と聞かれたら「はい」を選択

# 現在のユーザーをwiresharkグループに追加
sudo usermod -aG wireshark $USER

# 変更を反映（ログアウト→ログインが必要な場合あり）
newgrp wireshark
```

## ファイル構成

```
lessons/03-wireshark-analysis/
├── README.md              # このファイル
├── Makefile               # ビルド設定
├── server.cpp             # シンプルなサーバー（パケット解析用）
├── client.cpp             # シンプルなクライアント（パケット解析用）
├── wireshark_guide.md     # Wireshark操作ガイド
└── capture_filters.txt    # キャプチャフィルタの例
```

## ビルドと実行

### 1. ビルド

```bash
cd lessons/03-wireshark-analysis
make
```

---

## 方法A: tcpdumpでキャプチャ → Wiresharkで解析（推奨）

サーバー環境やGUIがない環境では、tcpdumpでpcapファイルを保存し、後でWiresharkで開く方法が便利です。

### 1. tcpdumpでキャプチャ開始（ターミナル1）

```bash
# ポート8080のTCP通信をcapture.pcapに保存
sudo tcpdump -i lo port 8080 -w capture.pcap
```

### 2. サーバーを起動（ターミナル2）

```bash
./server
```

### 3. クライアントを実行（ターミナル3）

```bash
./client
```

### 4. tcpdumpを停止

ターミナル1で `Ctrl+C` を押してキャプチャを停止します。

```
^C
5 packets captured
5 packets received by filter
0 packets dropped by kernel
```

### 5. Wiresharkでpcapファイルを開く

```bash
# GUIがある環境で
wireshark capture.pcap

# または、ファイルをダウンロードしてローカルのWiresharkで開く
```

### tcpdumpの便利なオプション

```bash
# 詳細表示（リアルタイムで内容確認）
sudo tcpdump -i lo port 8080 -X

# パケット数を制限
sudo tcpdump -i lo port 8080 -c 20 -w capture.pcap

# タイムスタンプ付き
sudo tcpdump -i lo port 8080 -tttt -w capture.pcap
```

---

## 方法B: Wiresharkで直接キャプチャ

GUIが使える環境では、Wiresharkで直接キャプチャできます。

### 1. Wiresharkを起動

```bash
wireshark &
```

### 2. キャプチャの設定

1. **ループバックインターフェースを選択**: `lo` または `Loopback: lo`
2. **キャプチャフィルタを設定**: `port 8080`
3. **キャプチャ開始ボタン**（青いサメのヒレ）をクリック

### 3. サーバーを起動（ターミナル1）

```bash
./server
```

### 4. クライアントを実行（ターミナル2）

```bash
./client
```

### 5. Wiresharkでパケットを確認

キャプチャを停止し、パケットを分析します。

## 観察ポイント

### 3ウェイハンドシェイク（接続確立）

```
クライアント                    サーバー
    |                              |
    |------ SYN (seq=0) --------->|  ① 接続要求
    |<-- SYN-ACK (seq=0,ack=1) ---|  ② 接続要求の確認 + 接続要求
    |------ ACK (ack=1) --------->|  ③ 接続確立完了
    |                              |
```

Wiresharkで確認できること：
- **SYN**: Flags: 0x002 (SYN)
- **SYN-ACK**: Flags: 0x012 (SYN, ACK)
- **ACK**: Flags: 0x010 (ACK)

### データ転送

```
クライアント                    サーバー
    |                              |
    |------ "Hello" + PSH ------->|  データ送信
    |<-------- ACK ---------------|  受信確認
    |                              |
```

Wiresharkで確認できること：
- **PSH (Push)フラグ**: データをすぐに上位層に渡す
- **シーケンス番号**: データの順序を管理
- **確認応答番号**: 受信したデータの確認

### 4ウェイハンドシェイク（接続終了）

```
クライアント                    サーバー
    |                              |
    |------ FIN + ACK ----------->|  ① 終了要求
    |<-------- ACK ---------------|  ② 終了要求の確認
    |<------ FIN + ACK -----------|  ③ サーバーからの終了要求
    |------ ACK ----------------->|  ④ 接続終了完了
    |                              |
```

## 演習問題

1. **シーケンス番号の追跡**:
   データ送信時にシーケンス番号がどのように増加するか確認してください。

2. **RTT（往復時間）の測定**:
   Wiresharkの統計機能を使って、パケットの往復時間を確認してください。

3. **大きなデータの転送**:
   client.cppを修正して大きなデータ（例：10KB）を送信し、
   TCPのセグメント分割を観察してください。

## 次のステップ

このレッスンを完了すると、TCP/IP通信の基本的な仕組みを視覚的に理解できます。

さらに学習を進めるには：
- UDPプロトコルとの比較
- HTTPなど上位プロトコルの解析
- ネットワークトラブルシューティング
