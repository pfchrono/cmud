# Custom Makfile for SocketMud, based on Zarius's MC25b Makefile
CC      = gcc

#IMC = 1

OPT_FLAG = -pipe -Os
DEBUG_FLAG = -g2
W_FLAGS = -Wall -DCMUD
C_FLAGS = $(MACHINE) $(W_FLAGS) $(DEBUG_FLAG) $(OPT_FLAG) $(PROF_FLAG) $(NOCRYPT) $(SAVEMEM) -DIMC
L_FLAGS = $(MACHINE) $(DEBUG_FLAG) $(OPT_FLAG) $(PROF_FLAG) -lz -lcrypt -pthread

EXECPRM = 4770

EXECFILE = CMud

#various Makefile variables
ECHOCMD = echo -e

L_RED     = \e[1;31m
L_BLUE    = \e[1;34m
L_GREEN   = \e[1;32m
L_WHITE   = \e[1;37m
L_NRM     = \e[0;00m

OBJ_DIR = obj

C_FILES = socket.c io.c strings.c utils.c interpret.c help.c  \
	  action_safe.c mccp.c save.c event.c event-handler.c \
	  list.c stack.c version.c comm.c imc.c sha256.c move.c \
	  signal.c random.c wiz.c

O_FILES := $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_FILES))

H_FILES = $(wildcard *.h)

#O_FILES = socket.o io.o strings.o utils.o interpret.o help.o  \
#	  action_safe.o mccp.o save.o event.o event-handler.o \
#	  list.o stack.o


all:
	@touch version.c
	@$(MAKE) build
	@$(MAKE) -s cmud

cmud: $(O_FILES)
	@$(ECHOCMD) "[- Rebuilding MUD executable: $(L_BLUE)$(EXECFILE)$(L_NRM) -]"
	@rm -f $(EXECFILE)
	@$(CC)  -o $(EXECFILE) $(O_FILES) $(L_FLAGS)
	@$(ECHOCMD) "[- Done Compiling : $(L_BLUE)$(EXECFILE)$(L_NRM) -]"
	@$(ECHOCMD) "[- Fixing MUD permissions to $(L_WHITE)$(EXECPRM)$(L_NRM) -]"
	@chmod g+w $(EXECFILE)
	@chmod a+x $(EXECFILE)
	@chmod g+w $(O_FILES)

purge:
	rm -f $(OBJ_DIR)/*.o $(EXECFILE) core
	rm -f ../area/core*

$(OBJ_DIR)/%.o: %.c
	@$(ECHOCMD) "-->   Compiling file: $(L_WHITE)$<$(L_NRM)   <--"
	@$(CC) -c $(C_FLAGS) -o $@ $<

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean :
	rm -f $(OBJ_DIR)/*.o ; rm -f $(EXECFILE)

build:
	@if [ -f ../txt/buildnum ]; then \
	expr `cat ../txt/buildnum` + 1 > ../txt/buildnum; \
	else \
		echo '1' > ../txt/buildnum; \
	fi
	@if [ -f ../txt/versionnum ]; then \
		expr `cat ../txt/versionnum` + 1 > ../txt/versionnum; \
	else \
		echo '10001' > ../txt/versionnum; \
	fi
	@if [ `cat ../txt/buildnum` -ge 51 ]; then \
	echo '1' > ../txt/buildnum; \
	fi
	@echo '#ifndef BUILD_H' > build.h
	@echo '#define BUILD_H' >> build.h
	@echo "#define LINKCOUNT   `cat ../txt/buildnum`" >> build.h
	@echo '#endif' >> build.h
	@echo '#ifndef VERSION_H' > version.h
	@echo '#define VERSION_H' >> version.h
	@echo "#define VERSION   `cat ../txt/versionnum`" >> version.h
	@echo '#endif' >> version.h
	@$(ECHOCMD) "$(L_WHITE)Build number $(L_RED)`cat ../txt/buildnum`$(L_WHITE), version $(L_RED)`cat ../txt/versionnum` $(L_WHITE)set.$(L_NRM)"

