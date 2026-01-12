#!/bin/bash
for i in {1..2}; do
    curl -X POST http://localhost:7777   -H "Content-Type: application/json"   -d '{"EVENT":"test_'$i'","TIME":"2024-12-30"}'
done
