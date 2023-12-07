# VUT FIT ISA - DNS resolver.
#
# DNS resolver Makefile.
# Author - Maksym Koval <xkoval20@stud.fit.vutbr.cz>
#

# Login
LOGIN = xkoval20

# Arch type
ARCH = tar

# Compiler
CC = gcc

# Compiler flags
# 	compiler flags:
#  		-g     - this flag adds debugging information to the executable file
#  		-Wall  - this flag is used to turn on most compiler warnings
CFLAGS = -std=c99 -g -Wall -pedantic

# Source file
SRC = src/dns.c

# Output executable
OUTPUT = dns

# Additions
ADDITIONS = Makefile README.md manual.pdf

HEADERS = src/dns.h

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC)

run: $(OUTPUT)
	./$(OUTPUT) -r -s 8.8.8.8 www.github.com

tar:
	$(ARCH) cvf $(LOGIN).tar $(SRC) $(LIST) $(HEADERS) $(ADDITIONS)

clean:
	rm -f $(OUTPUT) $(LOGIN).tar

TEST_CASES = test1 test2 test3 test4 test5 test6 test7 test8

test1:
	./$(OUTPUT) -r -s 8.8.8.8 www.github.com
test2:
	./$(OUTPUT) -x -r -s 8.8.8.8 www.github.com
test3:
	./$(OUTPUT) -r -s 8.8.8.8 www.facebook.com
test4:
	./$(OUTPUT) -x -r -s 8.8.8.8 www.facebook.com
test5:
	./$(OUTPUT) -r -s kazi.fit.vutbr.cz www.fit.vut.cz
test6:
	./$(OUTPUT) -r -s kazi.fit.vutbr.cz www.github.com
test7:
	./$(OUTPUT) -6 -s 8.8.8.8 www.facebook.com
test8:
	./$(OUTPUT) -r -6 -s 8.8.8.8 www.facebook.com

test: $(TEST_CASES)
.PHONY: test $(TEST_CASES)

