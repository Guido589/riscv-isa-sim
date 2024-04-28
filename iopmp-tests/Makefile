# Makefile
# Build directory
BUILD_DIR = build

# Compiler and flags
CC = g++
CFLAGS = -I $(BUILD_DIR) -I../build/ -I../riscv/ -I../softfloat/ -I../riscv/insns/ -std=c++2a

# Source files
SRCS = $(wildcard *.cc) ../riscv/csrs.cc ../riscv/mmu.cc ../riscv/processor.cc ../riscv/insn_template.cc

# Target executable
TARGET = unit-tests.out

# Default target
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR) && \
	cd $(BUILD_DIR) && \
	../../configure
	# make -j$(shell nproc)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Clean up object files and the target executable
clean:
	rm -f $(TARGET) && \
	rm -rf $(BUILD_DIR)