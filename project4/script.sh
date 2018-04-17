#!/bin/sh
for i in 0 1 2 3 4 5
do
  ./"node$i" $i &
done
