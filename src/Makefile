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

# default install command, allow to be overridden
INSTALL ?= install

# default install PREFIX, allow to be overridden
PREFIX ?= /usr/local

# location of model files
SUNS_MODELPATH="\"$(PREFIX)/lib/suns/models\""


BISON_OUT=suns_lang.tab.c
FLEX_OUT=suns_lang.yy.c

SRC=suns_parser.c suns_model.c suns_app.c suns_output.c \
	suns_host_parser.c suns_host.c \
	$(BISON_OUT) $(FLEX_OUT)
OBJ=$(SRC:.c=.o)
BINFILES=suns unit_tests

UNIT_TESTS_SRC=suns_unit_tests.c suns_model.c suns_output.c
UNIT_TESTS_OBJ=$(UNIT_TESTS_SRC:.c=.o)

TEST_SERVER_SRC=test_server.c
TEST_SERVER_OBJ=$(TEST_SERVER_SRC:.c=.o)

HOST_TEST_SRC=suns_model.c suns_host_parser.c suns_host_test.c suns_host.c \
	suns_parser.c suns_output_sqlite.c suns_output.c $(BISON_OUT) $(FLEX_OUT)
HOST_TEST_OBJ=$(HOST_TEST_SRC:.c=.o)

SUNS_STORE_SRC=suns_model.c suns_store.c suns_read_sqlite.c \
	suns_parser.c suns_output_sqlite.c suns_output.c $(BISON_OUT) $(FLEX_OUT)
SUNS_STORE_OBJ=$(SUNS_STORE_SRC:.c=.o)

LIBTRX=../lib/trx/libtrx.a
LIBEZXML=../lib/ezxml/libezxml.a

# cflags
CFLAGS=-std=gnu99 -g
CFLAGS+=-DDEBUG
ifdef SUNS_MODELPATH
CFLAGS+=-DSUNS_MODELPATH="$(SUNS_MODELPATH)"
endif
CFLAGS+=-Wall $(shell pkg-config --cflags libmodbus)
CFLAGS+=-I ../lib

# ldflags
LDFLAGS=-lm $(shell pkg-config --libs libmodbus) 


all: $(BINFILES)

include $(SRC:.c=.d)
#include $(UNIT_TESTS_SRC:.c=.d)


# when possible the implicit rule for generating *.o files is used

unit_tests: $(UNIT_TESTS_OBJ)
	$(CC) $(CFLAGS) $(UNIT_TESTS_OBJ) $(LDFLAGS) $(LIBTRX) -o unit_tests

suns: $(OBJ) $(LIBTRX) $(LIBEZXML)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) $(LIBEZXML) $(LIBTRX) -o suns

suns_lang.tab.c: suns_lang.y
	$(YACC) -d -o suns_lang.tab.c suns_lang.y

suns_lang.tab.h: suns_lang.tab.c

suns_lang.yy.c: suns_lang.l suns_lang.tab.h
	$(LEX) -o suns_lang.yy.c suns_lang.l

check: $(SRC) suns_unit_tests.c
	splint -posixlib $(CFLAGS) $(SRC) suns_unit_tests.c

suns_host_test: $(HOST_TEST_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -lsqlite3 $(HOST_TEST_OBJ) $(LIBEZXML) $(LIBTRX) -o suns_host_test

suns_store: $(SUNS_STORE_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -lsqlite3 $(SUNS_STORE_OBJ) $(LIBEZXML) $(LIBTRX) -o suns_store


$(LIBTRX):
	make -C ../lib libtrx

$(LIBEZXML):
	make -C ../lib ezxml

MODELS=../models/0xx_common.mdl \
	../models/10x_inverter.mdl \
	../models/2xx_meter.mdl \
	../models/3xx_environ.mdl \
	../models/4xx_string_combiner.mdl \
	../models/5xx_panel.mdl
# ../models/12x_inverter_control.mdl

install: suns
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) -o 755 suns $(PREFIX)/bin
	$(INSTALL) -d $(PREFIX)/lib/suns/models
	$(INSTALL) -o 644 $(MODELS) $(PREFIX)/lib/suns/models
	# make sure the old model file is removed
	rm -f $(PREFIX)/lib/suns/models/device.model

clean:
	rm -f	suns_lang.tab.c suns_lang.tab.h \
		suns_lang.yy.c *.o *.d $(BINFILES)

distclean:
	rm -f *~ *.o *.d $(BINFILES)
	make -C trx distclean
	make -C ezxml distclean

# this little bit of magic generates the *.d dependency files
# see section "4.14 Generating Prerequisites Automatically"
# of the GNU make info pages for details
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$; \
	echo generated $@
