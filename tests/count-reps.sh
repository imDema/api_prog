#!/bin/bash
sort tests/hashed.txt | uniq -cd | sort -nr > tests/repetitions.txt