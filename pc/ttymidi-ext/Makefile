all:
	gcc src/*.c src/com/uart.c  -o ttymidi -lasound -pthread
clean:
	rm ttymidi
install:
	install -m 0755 ttymidi /usr/local/bin
uninstall:
	rm /usr/local/bin/ttymidi

