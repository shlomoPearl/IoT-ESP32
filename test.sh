#!/bin/bash
for i in {1..5}; do
    curl -X POST http://localhost:7777   -H "Content-Type: application/json"   -d '{"EMAIL":"test@gmail.com","EVENT":"test_'$i'","TIME":"2024-12-30"}'
done

curl -v http://localhost:7777/TEST
