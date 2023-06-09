############################################################################
# FILE  : Makefile
# AUTHOR: (C) Copyright 2023 by Peter Chapin
#
# This is the GNU Make makefile for the netstream library.
############################################################################

CC      = gcc
CXX     = g++
AR      = ar
OPTIONS = -std=c++20 -Wall -c -g
LIBS    = 

# Implicit rules for converting source to object.
%.o:	%.cpp
	$(CXX) $(OPTIONS) $<

%.o:	%.c
	$(CC) $(OPTIONS) $<

############################################################################

# Object file list.
OBJS = netstream.o connection.o


# The overall target and how to make it.
libnetstream.a:	$(OBJS) 
	$(AR) -rcs libnetstream.a $(OBJS)

# The usual nicities
clean:
	rm -f *.a *.o *~ core

############################################################################
# Module dependencies

HEADERS = environ.hpp netstream.hpp connection.hpp

netstream.o:	netstream.cpp $(HEADERS)
connection.o:	connection.cpp $(HEADERS)
