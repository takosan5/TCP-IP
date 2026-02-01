# Wireshark操作ガイド

このガイドでは、TCP/IP通信をWiresharkで解析する方法を説明します。

## 1. キャプチャの開始

### 1.1 インターフェースの選択

ローカルホスト（127.0.0.1）での通信をキャプチャするには、**ループバックインターフェース**を選択します。

- Linux: `lo` または `Loopback: lo`
- macOS: `lo0` または `Loopback: lo0`
- Windows: `Npcap Loopback Adapter` または `Adapter for loopback traffic capture`

### 1.2 キャプチャフィルタの設定

キャプチャ開始前にフィルタを設定すると、必要なパケットのみをキャプチャできます。

```
port 8080
```

キャプチャフィルタはキャプチャバーの左側のテキストボックスに入力します。

### 1.3 キャプチャの開始

青いサメのヒレのアイコン（▶）をクリックしてキャプチャを開始します。

## 2. パケットの解析

### 2.1 表示フィルタ

キャプチャ後にパケットを絞り込むには、**表示フィルタ**を使用します。

| フィルタ | 説明 |
|----------|------|
| `tcp` | TCPパケットのみ表示 |
| `tcp.port == 8080` | ポート8080のTCPパケット |
| `tcp.flags.syn == 1` | SYNフラグが立っているパケット |
| `tcp.flags.fin == 1` | FINフラグが立っているパケット |
| `tcp.flags.push == 1` | PSH（Push）フラグが立っているパケット |
| `ip.addr == 127.0.0.1` | 127.0.0.1との通信 |

### 2.2 パケット詳細の見方

パケットをクリックすると、中央ペインに詳細が表示されます。

```
▼ Frame: 物理層の情報（パケットサイズなど）
▼ Ethernet II: データリンク層（MACアドレス）
▼ Internet Protocol Version 4: ネットワーク層（IPアドレス）
▼ Transmission Control Protocol: トランスポート層（TCP）
    Source Port: 送信元ポート
    Destination Port: 宛先ポート
    Sequence Number: シーケンス番号
    Acknowledgment Number: 確認応答番号
    Flags: TCPフラグ（SYN, ACK, FIN, PSHなど）
▼ Data: アプリケーションデータ
```

## 3. 3ウェイハンドシェイクの確認

### パケットの流れ

| No. | 方向 | フラグ | 説明 |
|-----|------|--------|------|
| 1 | Client → Server | SYN | 接続要求 |
| 2 | Server → Client | SYN, ACK | 接続受諾 + 確認 |
| 3 | Client → Server | ACK | 接続確立 |

### 確認方法

1. 表示フィルタに `tcp.flags.syn == 1` を入力
2. 最初のSYNパケットを見つける
3. 「Follow → TCP Stream」で関連パケットを追跡

## 4. データ転送の確認

### パケットの流れ

| No. | 方向 | フラグ | 説明 |
|-----|------|--------|------|
| 4 | Client → Server | PSH, ACK | データ送信 |
| 5 | Server → Client | ACK | 受信確認 |
| 6 | Server → Client | PSH, ACK | 応答データ |
| 7 | Client → Server | ACK | 受信確認 |

### 確認方法

1. 表示フィルタに `tcp.flags.push == 1` を入力
2. データが含まれるパケットを確認
3. 下部ペインでデータ内容を確認

## 5. 4ウェイハンドシェイクの確認

### パケットの流れ

| No. | 方向 | フラグ | 説明 |
|-----|------|--------|------|
| 8 | Client → Server | FIN, ACK | 終了要求 |
| 9 | Server → Client | ACK | 終了受諾 |
| 10 | Server → Client | FIN, ACK | サーバー側終了 |
| 11 | Client → Server | ACK | 接続終了完了 |

### 確認方法

1. 表示フィルタに `tcp.flags.fin == 1` を入力
2. FINパケットとその後のACKを確認

## 6. 便利な機能

### 6.1 TCPストリームの追跡

1. パケットを右クリック
2. 「Follow」→「TCP Stream」を選択
3. 通信の全データが読みやすい形式で表示される

### 6.2 統計情報

**メニュー → Statistics → Conversations**
- 通信の統計情報（パケット数、バイト数など）を確認

**メニュー → Statistics → TCP Stream Graphs → Time Sequence (Stevens)**
- シーケンス番号の変化をグラフで確認

### 6.3 パケットの色分け

Wiresharkはパケットを自動的に色分けします：

- **緑**: HTTP
- **薄青**: TCP
- **黄**: SMB/CIFS
- **赤**: パケットにエラーがある

## 7. トラブルシューティング

### キャプチャできない場合

```bash
# Wiresharkに必要な権限を確認
sudo chmod +x /usr/bin/dumpcap
sudo setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' /usr/bin/dumpcap

# wiresharkグループに追加（要ログアウト）
sudo usermod -aG wireshark $USER
```

### ループバックが見つからない場合

Linuxでは、ループバックインターフェースでのキャプチャに追加設定が必要な場合があります：

```bash
# 確認
ip link show lo

# 有効化（通常は不要）
sudo ip link set lo up
```
