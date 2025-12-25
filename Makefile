CC = gcc
CFLAGS =  -O2
LIBS = -lm
SRC_DIR = src

all: step1 step2 step3

# 编译 Step 1
step1: $(SRC_DIR)/step1_grouping.c $(SRC_DIR)/common.c
	$(CC) $(CFLAGS) -o step1 $(SRC_DIR)/step1_grouping.c $(SRC_DIR)/common.c

# 编译 Step 2
step2: $(SRC_DIR)/step2_analysis.c $(SRC_DIR)/common.c
	$(CC) $(CFLAGS) -o step2 $(SRC_DIR)/step2_analysis.c $(SRC_DIR)/common.c $(LIBS)

# 编译 Step 3
step3: $(SRC_DIR)/step3_post_process.c
	$(CC) $(CFLAGS) -o step3 $(SRC_DIR)/step3_post_process.c $(LIBS)

clean:
	rm -f step1 step2 step3 *.o
