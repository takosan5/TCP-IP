# Lesson 01: Echo Server - Makefile
# C++17でコンパイル、警告を有効化

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# ビルド対象
TARGETS = server client

# デフォルトターゲット: すべてビルド
all: $(TARGETS)

# サーバーのビルド
server: server.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp

# クライアントのビルド
client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

# クリーンアップ
clean:
	rm -f $(TARGETS)

# ヘルプ
help:
	@echo "使用方法:"
	@echo "  make        - server と client をビルド"
	@echo "  make server - server のみビルド"
	@echo "  make client - client のみビルド"
	@echo "  make clean  - ビルド成果物を削除"

.PHONY: all clean help
