#
# Sunspec Test Tool Makefile
#
# Copyright (c) 2011, John D. Blair <jdb@moship.net>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
#     * Neither the name of John D. Blair nor his lackeys may be used
#       to endorse or promote products derived from this software
#       without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# JOHN D. BLAIR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

BISON_OUT=suns_lang.tab.c
FLEX_OUT=suns_lang.yy.c

SRC=suns_parser.c suns_model.c suns_app.c suns_output.c \
	$(BISON_OUT) $(FLEX_OUT)
OBJ=$(SRC:.c=.o)
BINFILES=suns unit_tests

UNIT_TESTS_SRC=suns_unit_tests.c suns_model.c suns_output.c
UNIT_TESTS_OBJ=$(UNIT_TESTS_SRC:.c=.o)

TEST_SERVER_SRC=test_server.c
TEST_SERVER_OBJ=$(TEST_SERVER_SRC:.c=.o)

LIBTRX=trx/libtrx.a
LIBEZXML=ezxml/libezxml.a

CFLAGS=-std=gnu99 -g -DDEBUG -Wall $(shell pkg-config --cflags libmodbus)
LDFLAGS=-lm $(shell pkg-config --libs libmodbus) 

#LDLIBS=$(LIBTRX) $(shell pkg-config --libs libmodbus)

all: $(BINFILES)

include $(SRC:.c=.d)
#include $(UNIT_TESTS_SRC:.c=.d)


# when possible the implicit rule for generating *.o files is used

unit_tests: $(UNIT_TESTS_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(UNIT_TESTS_OBJ) $(LIBTRX) -o unit_tests

suns: $(OBJ) $(LIBTRX) $(LIBEZXML)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(LIBEZXML) $(LIBTRX) -o suns

suns_lang.tab.c: suns_lang.y
	$(YACC) -d -o suns_lang.tab.c suns_lang.y

suns_lang.tab.h: suns_lang.tab.c

suns_lang.yy.c: suns_lang.l suns_lang.tab.h
	$(LEX) -o suns_lang.yy.c suns_lang.l

check: $(SRC) suns_unit_tests.c
	splint -posixlib $(SRC) suns_unit_tests.c

$(LIBTRX):
	make -C trx libtrx.a

$(LIBEZXML):
	make -C ezxml libezxml.a

clean:
	rm -f	suns_lang.tab.c suns_lang.tab.h \
		suns_lang.yy.c *.o *.d $(BINFILES)
	make -C trx clean
	make -C ezxml clean

distclean:
	rm -f *~ *.o *.d $(BINFILES)
	make -C trx distclean
	make -C ezxml distclean

# this little bit of magic generates the *.d dependency files
# see section "4.14 Generating Prerequisites Automatically"
# of the GNU make info pages for details
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$; \
	echo generated $@
