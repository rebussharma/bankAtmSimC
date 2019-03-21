all:
	touch client
	touch server
	rm client
	rm server
	g++ tcpcli01.c -o client
	g++ tcpserv01.c -o server
