CPPFLAGS = -std=c++11 -Wall -Werror -pedantic -ggdb -O3 -pthread -w
PROGRAMS = HuffmanMain 

all : $(PROGRAMS)

% : %.cpp %.h
	g++ $(CPPFLAGS) $< -o $@

clean :
	rm -f $(PROGRAMS)
