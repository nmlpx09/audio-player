CC = clang++-20
CCFLAGS = -O3 -ffast-math -std=c++23 -pedantic -Wall -W -Werror -Wextra -c -I.
DEFINES =

OBJ_ALSA = bin/main.o send/alsa.o read/wav.o
LDFLAGS_ALSA = -lasound

OBJ_PULSE = bin/main.o send/pulse.o read/wav.o
LDFLAGS_PULSE = -lpulse-simple -lpulse

RUN = bin/play

alsa: DEFINES += -DALSA
alsa: $(OBJ_ALSA)
	$(CC) $^ -o $(RUN) $(LDFLAGS_ALSA)

pulse: $(OBJ_PULSE)
	$(CC) $^ -o $(RUN) $(LDFLAGS_PULSE)

%.o: %.cpp
	$(CC) $(DEFINES) $(CCFLAGS) $< -o $@

install:
	strip $(RUN)
	cp $(RUN) /usr/bin/

install_termux:
	cp $(RUN) /data/data/com.termux/files/usr/bin/

clean:
	rm -f $(OBJ_ALSA) $(OBJ_PULSE) $(RUN)
