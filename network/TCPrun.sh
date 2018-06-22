#!/bin/bash

name=$(echo $1 | cut -d '-' -f1 -)
node1=$(echo $1 | cut -d '-' -f2 - | tr -d '[')
node2=$(echo $1 | cut -d '-' -f3 - | tr -d ']')
if [ "$(hostname)" == "$name-$node1" ]
then
    //exports/home/sajmera4/network/MyNETBench-TCP S $name-$node2 $2 $3
else
    //exports/home/sajmera4/network/MyNETBench-TCP C $name-$node1 $2 $3
fi
