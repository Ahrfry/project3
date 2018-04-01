all: basic multi abort multi-abort truncate 
basic: 
	$(CC) $(OPTIONS) -o basic basic.c ../librvm.a

multi: 
	$(CC) $(OPTIONS) -o multi multi.c ../librvm.a

abort: 
	$(CC) $(OPTIONS) -o abort abort.c ../librvm.a

multi-abort: 
	$(CC) $(OPTIONS) -o multi-abort multi-abort.c ../librvm.a

truncate: 
	$(CC) $(OPTIONS) -o truncate truncate.c ../librvm.a


clean:
	rm -rf *.o basic multi abort multi-abort truncate
