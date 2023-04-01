all: server client

server: bin/aurrasd

client: bin/aurras

bin/aurrasd: obj/aurrasd.o obj/Communication.o obj/InputHandler.o obj/ReadFilters.o
	gcc -g obj/aurrasd.o obj/Communication.o obj/InputHandler.o obj/ReadFilters.o -o bin/aurrasd

obj/aurrasd.o: src/aurrasd.c
	gcc -Wall -g -c src/aurrasd.c -o obj/aurrasd.o

obj/Communication.o: src/Communication.c
	gcc -Wall -g -c src/Communication.c -o obj/Communication.o

obj/InputHandler.o: src/InputHandler.c
	gcc -Wall -g -c src/InputHandler.c -o obj/InputHandler.o

obj/ReadFilters.o: src/ReadFilters.c
	gcc -Wall -g -c src/ReadFilters.c -o obj/ReadFilters.o

bin/aurras: obj/aurras.o obj/Communication.o obj/InputHandler.o obj/ReadFilters.o
	gcc -g obj/aurras.o obj/Communication.o obj/InputHandler.o obj/ReadFilters.o -o bin/aurras

obj/aurras.o: src/aurras.c 
	gcc -Wall -g -c src/aurras.c -o obj/aurras.o

clean:
	rm obj/*
	rm bin/aurras
	rm bin/aurrasd
	rm tmp/*

commands:
	bin/aurras

openserver:
	bin/aurrasd etc/aurrasd.conf bin/aurrasd-filters

status:
	bin/aurras status

filters:
	bin/aurras filters

transform1:
	bin/aurras transform samples/sample-2-miei.m4a miei1.mp3 eco

transform2:
	bin/aurras transform samples/sample-2-miei.m4a miei2.mp3 rapido rapido alto

transform3:
	bin/aurras transform samples/sample-3-lcc.m4a lcc1.mp3 lento baixo

transform4:	
	bin/aurras transform samples/sample-3-lcc.m4a lcc2.mp3 eco rapido alto

transformeco:
	bin/aurras transform samples/sample-1-so.m4a so.mp3 eco

transformmusic:
	bin/aurras transform samples/Ievan_Polkka.m4a polkka.mp3 rapido rapido

three: transform1 transform2 transform3 status
