# Copyright (c) 2009, 2010 Basilio B. Fraguela. Universidade da Coruña

INCL := .
CC  := gcc
CXX := g++

objs           := c3ms.o c3mslex.o c3ms.tab.o
outputbinaries := c3ms

ifdef PRODUCTION
  CFLAGS := -I$(INCL) -O3 -DNDEBUG -fomit-frame-pointer
  #-fomit-frame-pointer makes exceptions not work properly for some compilers
  CPPFLAGS := -I$(INCL) -O3 -DNDEBUG
else
  CFLAGS := -I$(INCL) -g
  CPPFLAGS := -I$(INCL) -g
endif

all :  $(outputbinaries)

c3ms : $(objs)
	$(CXX) $(CPPFLAGS) -o $@ $^

%.tab.cpp: %.y
	bison -d -o $@ $<

%lex.cpp:  %.l %.tab.cpp
	flex -Cemr -o$@ $<

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@

clean:
	-@rm -f $(objs) c3ms.tab* c3mslex.cpp

veryclean: clean
	-@rm -rf $(outputbinaries)