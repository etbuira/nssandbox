
AM_CFLAGS = -Wall -pedantic

bin_PROGRAMS = nssandbox
nssandbox_SOURCES = src/nssandbox.c

install-exec-hook:
	$(CHMOD) u+s $(DESTDIR)$(bindir)/nssandbox

