#!/bin/sh

mkdir -p logs
echo > ./logs/tlink_debug.log 
killall tail
tail -f ./logs/tlink_debug.log &

while [ 1 ]
do
./tlink -a <xxxx> -p <xxx> -i eth0 -t 3
sleep 1
done
