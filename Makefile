## kc3
## Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted the above
## copyright notice and this permission paragraph are included in all
## copies and substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

PROG = sort
PROG_DEBUG = sort_debug

VERSION = 0.1-git

SRCS = sort.c

OBJS = sort.o

OBJS_DEBUG = sort.debug.o

DIST = ${PROG}-${VERSION}

COMMON_CFLAGS = -W -Wall -Werror -std=c89 -pedantic
COMMON_CFLAGS += -D_POSIX_C_SOURCE=200809 # make glibc happy

CFLAGS = ${COMMON_CFLAGS} -O2 -pipe

CFLAGS_DEBUG = ${COMMON_CFLAGS} -g -O0

CLEANFILES = *.o ${PROG} ${PROG_DEBUG} ${DIST}.tar.gz

prefix ?= /usr/local

bindir = ${prefix}/bin

all: build debug

include configure.mk

build: ${PROG}

clean:
	rm -f ${CLEANFILES}
	rm -rf ${DIST}

debug: ${PROG_DEBUG}

dist: ${DIST}.tar.gz

${DIST}.tar.gz:
	rm -rf ${DIST}.old
	! test -d ${DIST} || mv ${DIST} ${DIST}.old
	mkdir ${DIST}
	cpio -pdl ${DIST} < ${PROG}.index
	tar czf ${DIST}.tar.gz ${DIST}

gdb: debug
	gdb ${PROG_DEBUG}

install:
	install -d -m 0755 ${DESTDIR}${bindir}
	install -m 0755 ${PROG} ${DESTDIR}${bindir}/${PROG}

${PROG}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} ${LDFLAGS} -o ${PROG}

${PROG_DEBUG}: ${OBJS_DEBUG}
	${CC} ${CFLAGS_DEBUG} ${OBJS_DEBUG} ${LDFLAGS} -o ${PROG_DEBUG}

sort.o: ${src_dir}/sort.c
	${CC} ${CFLAGS} -c ${src_dir}/sort.c -o sort.o

sort.debug.o: ${src_dir}/sort.c
	${CC} ${CFLAGS_DEBUG} -c ${src_dir}/sort.c -o sort.debug.o

uninstall:
	rm -f ${bindir}/${PROG}

.PHONY: all build clean debug dist ${DIST}.tar.gz gdb
