.PHONY: relay
relay: 
	gcc -DTESTWHOLE relay.c socket.c -lpthread -o relay


.PHONY: fetch
fetch:
	scp pi@nanoVNA.local:relay/Makefile .
	scp pi@nanoVNA.local:relay/relay.c .
	scp pi@nanoVNA.local:relay/socket.c .

.PHONY: push
push:
	ssh pi@nanoVNA.local "rm -rf relay ; mkdir relay"
	scp Makefile pi@nanoVNA.local:relay/Makefile
	scp relay.c pi@nanoVNA.local:relay/relay.c
	scp socket.c pi@nanoVNA.local:relay/socket.c
