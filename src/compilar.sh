#!/usr/bin/env bash
if (g++ main.cpp Lexer.cpp parser.cpp -o compilador $(pkg-config --cflags --libs gtk+-3.0) -std=c++17); then
  ./compilador
fi
