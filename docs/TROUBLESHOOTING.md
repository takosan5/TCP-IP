# トラブルシューティング

## よくある問題と解決策

### 1. "Address already in use" エラー

**症状**:
```
エラー: ポートへのバインドに失敗しました
原因: ポート 8080 は既に使用中の可能性があります
```

**解決策**:
```bash
# 使用中のプロセスを確認
lsof -i :8080

# プロセスを終了
kill <PID>

# または、少し待ってから再試行（前のプロセスが完全に終了するまで）
sleep 5
./server
```

### 2. "Connection refused" エラー

**症状**:
```
エラー: サーバーへの接続に失敗しました
原因: サーバーが起動していないか、ポート番号が間違っています
```

**解決策**:
1. サーバーが起動しているか確認
2. サーバーとクライアントで同じポート番号を使用しているか確認
3. ファイアウォールがブロックしていないか確認

### 3. コンパイルエラー: "g++: command not found"

**症状**:
```bash
make: g++: Command not found
```

**解決策**:
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install g++ build-essential
```

### 4. Wiresharkでlocalhostがキャプチャできない

**症状**:
- Wiresharkでloインターフェースが表示されない
- キャプチャしてもパケットが見えない

**解決策**:
```bash
# 方法1: Wiresharkを管理者権限で起動
sudo wireshark

# 方法2: tcpdumpを使用（代替手段）
sudo tcpdump -i lo port 8080 -X

# 方法3: Wiresharkのキャプチャ権限を設定
sudo usermod -aG wireshark $USER
# 再ログインが必要
```

### 5. サーバーが応答しない

**症状**:
- クライアントからメッセージを送っても返答がない

**解決策**:
1. サーバーのターミナルでエラーメッセージを確認
2. サーバーを再起動
3. `netstat -tlnp | grep 8080` でサーバーがリッスンしているか確認

### 6. 文字化け

**症状**:
- 日本語のエラーメッセージが文字化けする

**解決策**:
```bash
# ロケール設定を確認
locale

# UTF-8に設定
export LANG=ja_JP.UTF-8
```

## それでも解決しない場合

1. 各レッスンのREADME.mdを再確認
2. `make clean && make` でクリーンビルドを試す
3. システムを再起動してポートを解放
