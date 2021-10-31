CFLAGS = -O0 -g -std=c99 -Werror -Wall -Wextra
# CFLAGS = -O0 \
# -Werror	-Wall -Wextra\
# -Wpedantic -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 \
# -Wformat-security -Wnull-dereference -Wstack-protector -Wtrampolines -Walloca\
# -Wvla -Warray-bounds=2 -Wimplicit-fallthrough=3 -Wtraditional-conversion\
# -Wshift-overflow=2 -Wcast-qual -Wstringop-overflow=4 -Wconversion\
# -Warith-conversion -Wlogical-op -Wduplicated-cond -Wduplicated-branches\
# -Wformat-signedness -Wshadow -Wstrict-overflow=4 -Wundef -Wstrict-prototypes\
# -Wswitch-default -Wswitch-enum -Wstack-usage=1000000 -Wcast-align=strict \
# -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fstack-clash-protection -fPIE \
# -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-z,separate-code

TDD_SRC = tdd/tdd.h
SRC = simpl.c simpl.h assert.c assert.h loc.c loc.h lex.c lex.h

SUBDIRS := $(wildcard */.)

.PHONY: all clean
all : $(SUBDIRS) test main
	./test

$(SUBDIRS) :
	$(MAKE) -C $@

test : test.c $(TDD_SRC) $(SRC)

main : main.c $(SRC)

clean :
	$(RM) main test *.o output.*
