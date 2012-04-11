#
# John's Bag-of-Tricks (trx) library
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


SRC=buffer.c debug.c list.c string.c date.c
OBJ=$(SRC:.c=.o)
#CFLAGS=-fPIC -g -c -Wall -DDEBUG
CFLAGS=-g -Wall -DDEBUG -DLIST_SHUFFLE

all: libtrx.a

include $(SRC:.c=.d)

libtrx.a: $(OBJ)
	ar rcs libtrx.a $(SRC:.c=.o)

libtrx.so: $(OBJ)
	$(CC) -shared -Wl,-soname,libtrx.so.1 \
		-o libtrx.so.1.0.1 $(OBJ) -lc

unit_tests: libtrx.a unit_tests.o
	$(CC) $(CFLAGS) unit_tests.o libtrx.a -lm -o unit_tests

list_unit_tests: libtrx.a list_unit_tests.o
	$(CC) $(CFLAGS) list_unit_tests.o libtrx.a -lm -o list_unit_tests

clean:
	rm -f *.d *.o libtrx.a unit_tests list_unit_tests

distclean:
	rm -f *~ *.o *.d


# this little bit of magic generates the *.d dependency files
# see section "4.14 Generating Prerequisites Automatically"
# of the GNU make info pages for details
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$; \
	echo generated $@
