
all: oss user


oss: oss.c queue.c system.c
	gcc -g oss.c -o oss -pthread

user: user.c queue.c system.c
	gcc -g user.c -o user -pthread


clean:
	rm -f *.o
	rm -f oss
	rm -f user
	rm -f logfile
	rm -r -d *.dSYM
