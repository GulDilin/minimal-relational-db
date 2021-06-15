cmake_minimum_required(VERSION 3.17)
project(spo_lab_1_5 C)

set(CMAKE_C_STANDARD 99)

add_executable(spo_lab_1_5 src/main.c src/cursor/cursor.c src/cursor/cursor.h src/client/client.c src/client/client.h src/server/server.c src/server/server.h src/common/net.h src/common/net.c src/client/parser.c src/client/parser.h)