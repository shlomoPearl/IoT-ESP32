#!/bin/bash
echo '{"EVENT":"test","TIME":"2024-12-30"}' | nc localhost 7777
for i in {1..20}; do
    echo '{"EVENT":"test_'$i'","TIME":"2024-12-30"}' | nc localhost 7777
done
