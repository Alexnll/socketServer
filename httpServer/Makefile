all: server login.cgi
LIBS = -lpthread

server: httpServer.c
	gcc $< $(LIBS) -o $@

login.cgi: login.c
	gcc $< -o $@

clean:
	rm server login.cgi