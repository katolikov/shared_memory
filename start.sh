#!/bin/sh
gcc server_new.c -o server -std=c89
gcc client_new.c -o client -std=c89
./server&
osascript -e 'tell app "Terminal"
    do script "cd Desktop
    cd lab4
    ./client"
end tell'
exit 0
