CC = clang++-20
CCFLAGS = -O3 -ffast-math -std=c++23 -pedantic -Wall -W -Werror -Wextra -c -I.
DEFINES =
LDFLAGS = -lpulse-simple -lpulse
OBJ = bin/main.o send/socket.o send/pulse.o rtp/rtp.o read/wav.o
RUN = bin/play

all: $(OBJ)
	$(CC) $^ -o $(RUN) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(DEFINES) $(CCFLAGS) $< -o $@

install:
	strip $(RUN)
	cp $(RUN) /usr/bin/

termux: 
	cp $(RUN) /data/data/com.termux/files/usr/bin/

clean:
	rm -f $(OBJ) $(RUN)
