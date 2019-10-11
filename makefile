.RECIPEPREFIX +=
CC = gcc
CFLAGS = -Wall -lm -lrt -lwiringPi

PROG = bin/*
OBJS = obj/*

default:
    mkdir -p bin obj
    $(CC) $(CFLAGS) -c main.cpp -o obj/main
    $(CC) $(CFLAGS) obj/main -o bin/main

run:
    sudo ./bin/main

clean:
    rm $(PROG) $(OBJS)