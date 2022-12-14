
all: oss user

oss: oss.c system.c
	gcc -g oss.c -o oss

user: user.c system.c
	gcc -g user.c -o user

clean:
	rm -f *.o
	rm -f oss
	rm -f user
	rm -f logfile
	rm -f logfile*
	rm -r -d *.dSYM
