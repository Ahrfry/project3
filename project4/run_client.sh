#!/bin/sh
max=10
for i in `seq 1 $max`
do
  	./client1 1 && ./client2 2 && ./client3 3 && ./client4 4 && ./client5 5
done

exit 0
