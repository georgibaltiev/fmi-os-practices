#!/bin/bash

# Намерете командите на 10-те най-стари процеси в системата.
ps -e -o comm=,etimes= | tr -s ' ' | sort -n -k 2 -t ' '  | head | cut -d ' ' -f 1

