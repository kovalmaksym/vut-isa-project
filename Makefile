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
CC = g++

# Compiler flags
# 	compiler flags:
#  		-g     - this flag adds debugging information to the executable file
#  		-Wall  - this flag is used to turn on most compiler warnings
CFLAGS = -std=c++11 -g -Wall

# Source file
SRC = src/main.cpp

# Output executable
OUTPUT = dns

# Additions
ADDITIONS = Makefile

# LIST = main.hpp
HEADERS = main.hpp

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC)

run: $(OUTPUT)
	./$(OUTPUT)

tar:
	$(ARCH) cvf $(LOGIN).tar $(SRC) $(LIST) $(HEADERS) $(ADDITIONS)

clean:
	rm -f $(OUTPUT) $(LOGIN).tar


# The build target
# TARGET = main
# OUT = out.txt
# ADDITIONS = Makefile

# $(TARGET): $(TARGET).cpp
# 	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp $(LIST) $(LDLIBS)

# all: clean $(TARGET) tar

# clean:
# 	$(RM) $(TARGET) $(LOGIN).tar $(OUT)



# test1:
# 	./$(TARGET) --mx 1 --my 0 --ma 45 --v 70 --cd 0 --wx 0 --wy 0 --wz 0 --wf 0 --d 600 --ro 0.1

# test2:
# 	./$(TARGET) --mx 1 --my 0 --ma 45 --v 70 --cd 0.47 --wx 0 --wy 0 --wz 0 --wf 0 --d 600 --ro 0.1

# test3:
# 	./$(TARGET) --mx 1 --my 0 --ma 45 --v 70 --cd 0.5 --wx 0 --wy 0 --wz 0 --wf 0 --d 600 --ro 0.1

# test4:
# 	./$(TARGET) --mx 1 --my 0 --ma 45 --v 70 --cd 0.04 --wx 1 --wy 0 --wz 0 --wf 5 --d 600 --ro 0.1

# test5:
# 	./$(TARGET) --mx 1 --my 0 --ma 45 --v 70 --cd 0.47 --wx -1 --wy 0 --wz 0 --wf 5 --d 600 --ro 0.1


