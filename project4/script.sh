#!/bin/sh
for i in  5 4 3 2 1
do
  ./"node$i" $i &
done
