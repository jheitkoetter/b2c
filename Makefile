#
#	$Id: Makefile,v 2.1 1997/11/21 14:51:34 joke Rel $
#
VERSION	= 2.3

CC	= gcc
CFLAGS	= -I. -O

SRCS	= ./*.c
HDRS	= ./*.h

PRG	= b2c
VERS	= $(PRG)-$(VERSION)

DEMOS	= you ball
TROFF	= groff

all:	$(PRG) $(DEMOS)

$(PRG):	$(PRG).o
	$(CC) -o $(PRG) $(PRG).o

######## demonstration ######################################################
#
$(DEMOS):	you.o ball.o
	$(CC) -o you you.o
	$(CC) -o ball ball.o

you.o:	youvegot.h
ball.o: ballshot.h

youvegot.h: $(PRG)
	rm -f youvegot.h
	./$(PRG) -i youvegot -D YOUVESIZ -G _youvegot_h_ -o youvegot.h youvegotmail.au
ballshot.h: $(PRG)
	rm -f ballshot.h
	./$(PRG) -i ballshot -D BALLSIZ -G _ballshot_h_ -o ballshot.h ballshot.au

######## packaging ###########################################################
#
doc man: readme
	nroff -man $(PRG).1 >$(PRG).man
	$(TROFF) -man $(PRG).1 >$(PRG).ps

readme: b2c.1
	@echo "README file for $(PRG) version $(VERSION)" >README
	@echo "Copyright (c) 1991-2016 Joerg Heitkoetter. All rights reserved." >> README
	@echo "" >> README
	groff -man -Tascii b2c.1 | ul -t dumb | uniq >>README
	cp README README.md

######## packaging ###########################################################
#
DISTFILES = Makefile COPYING README.md THANKS \
	$(PRG).c $(PRG).1 $(PRG).man $(PRG).ps you.c youvegot.h \
	youvegotmail.au ball.c ballshot.h ballshot.au
DISTDIR = $(PRG)-$(VERSION)

links:	youvegot.h man
	rm -rf $(DISTDIR)
	mkdir $(DISTDIR)
	chmod 777 $(DISTDIR)
	@echo "Copying distribution files"
	for i in $(DISTFILES) ; do \
	  ln ./$$i $(DISTDIR) 2> /dev/null || cp -p $$i $(DISTDIR); \
	done
	chmod -R a+r $(DISTDIR)

dist tar:	links
	rm -f $(VERS).tar*
	tar chovf $(VERS).tar $(DISTDIR)
	gzip -v9 $(VERS).tar
	rm -rf $(DISTDIR)

uue:	tar $(PRG)
	uuencode $(VERS).tar.gz $(VERS).tar.gz >$(VERS).uue

shar:	links
	shar -c -V -n "$(VERS)" -a -M \
		-s joke@verizon.com \
		-L50 -d "END-OF-FUN" -o "$(VERS).shar" \
		$(DISTDIR)
	rm -rf $(DISTDIR)

######## cleanup #############################################################
#
clean:
	rm -f *~ *.o *.BAK *.man *.ps $(VERS).*

realclean clobber: clean
	rm -f $(PRG) $(DEMOS) youvegot.h ballshot.h README.md
###
