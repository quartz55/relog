#!/bin/bash

for file in examples/**.bs.js; do
    echo "";
    echo "Running example: $file";
    echo "";
    node "$file";
done