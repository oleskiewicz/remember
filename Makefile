PREFIX    = /usr/local
MANPREFIX = $(PREFIX)/share
CFLAGS    = -std=c99 -Wall -O2

BIN=forget
SCRIPT=remember
MAN1=forget.1 \
	remember.1

all: $(BIN) .gitignore

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin && install -m0755 $(BIN) $(SCRIPT) $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(MANPREFIX)/man/man1 && install -m0644 bib.1 $(DESTDIR)$(MANPREFIX)/man/man1

uninstall:
	for bin in $(BIN); do rm -f $(DESTDIR)$(PREFIX)/bin/$$bin; done
	for script in $(SCRIPT); do rm -f $(DESTDIR)$(PREFIX)/bin/$$script; done
	for man1 in $(MAN1); do rm -f $(DESTDIR)$(MANPREFIX)/man/man1/$$man1; done

clean:
	rm -f $(BIN)

.gitignore:
	echo $(BIN) > .gitignore
