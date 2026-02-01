# Lesson 03: Wiresharkによるパケット解析 - Makefile
#
# 使い方:
#   make        - サーバーとクライアントをビルド
#   make clean  - ビルド成果物を削除

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# ビルドターゲット
TARGETS = server client

# デフォルトターゲット: 全てをビルド
all: $(TARGETS)

# サーバーのビルド
server: server.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# クライアントのビルド
client: client.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# クリーンアップ
clean:
	rm -f $(TARGETS)

.PHONY: all clean
