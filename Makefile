SHELL := /bin/bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c

CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -O0 -g

SRC_DIR := src
BIN_DIR := bin
LOG_DIR := logs

TIMEOUT := timeout
TMO := 2s
N ?= 5

TASK1_SRC := $(SRC_DIR)/task1_create_process.c
TASK2_SRC := $(SRC_DIR)/task2_sync_wait.c
TASK3_SRC := $(SRC_DIR)/task3_pipe_ipc.c
TASK4_SRC := $(SRC_DIR)/task4_multiple_children.c
TASK5_SRC := $(SRC_DIR)/task5_shared_memory.c

TASK1_BIN := $(BIN_DIR)/task1
TASK2_BIN := $(BIN_DIR)/task2
TASK3_BIN := $(BIN_DIR)/task3
TASK4_BIN := $(BIN_DIR)/task4
TASK5_BIN := $(BIN_DIR)/task5

.PHONY: all clean run1 run2 run3 run4 run5 test1 test2 test3 test4 test5 test stress1 stress2 stress3 stress4 stress5 capture-logs help

all: $(TASK1_BIN) $(TASK2_BIN) $(TASK3_BIN) $(TASK4_BIN) $(TASK5_BIN)

$(BIN_DIR) $(LOG_DIR):
	mkdir -p $@

$(TASK1_BIN): $(TASK1_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(TASK2_BIN): $(TASK2_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(TASK3_BIN): $(TASK3_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(TASK4_BIN): $(TASK4_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(TASK5_BIN): $(TASK5_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

run1: $(TASK1_BIN) ; ./$(TASK1_BIN)
run2: $(TASK2_BIN) ; ./$(TASK2_BIN)
run3: $(TASK3_BIN) ; ./$(TASK3_BIN)
run4: $(TASK4_BIN) ; ./$(TASK4_BIN)
run5: $(TASK5_BIN) ; ./$(TASK5_BIN)

test1: $(TASK1_BIN)
	out="$$( $(TIMEOUT) $(TMO) ./$(TASK1_BIN) )"
	echo "$$out"
	echo "$$out" | grep -q "Parent starting"
	echo "$$out" | grep -q "Child here"

test2: $(TASK2_BIN)
	out="$$( $(TIMEOUT) $(TMO) ./$(TASK2_BIN) )"
	echo "$$out"
	echo "$$out" | grep -q "exit_code=42"

test3: $(TASK3_BIN)
	out="$$( $(TIMEOUT) $(TMO) ./$(TASK3_BIN) )"
	echo "$$out"
	echo "$$out" | grep -q "Child received"
	echo "$$out" | grep -q "Hello from parent (task3)."

test4: $(TASK4_BIN)
	out="$$( $(TIMEOUT) $(TMO) ./$(TASK4_BIN) )"
	echo "$$out"
	[ "$$(echo "$$out" | grep -c "Reaped child")" -eq 3 ]

test5: $(TASK5_BIN)
	out="$$( $(TIMEOUT) $(TMO) ./$(TASK5_BIN) )"
	echo "$$out"
	echo "$$out" | grep -q "marked segment for deletion"

test: test1 test2 test3 test4 test5

stress4: $(TASK4_BIN)
	for i in $$(seq 1 $(N)); do
		out="$$( $(TIMEOUT) $(TMO) ./$(TASK4_BIN) )"
		echo "$$out"
		echo "----"
		[ "$$(echo "$$out" | grep -c "Reaped child")" -eq 3 ]
	done

stress1: $(TASK1_BIN)
	for i in $$(seq 1 $(N)); do $(TIMEOUT) $(TMO) ./$(TASK1_BIN); echo "----"; done
stress2: $(TASK2_BIN)
	for i in $$(seq 1 $(N)); do $(TIMEOUT) $(TMO) ./$(TASK2_BIN); echo "----"; done
stress3: $(TASK3_BIN)
	for i in $$(seq 1 $(N)); do $(TIMEOUT) $(TMO) ./$(TASK3_BIN); echo "----"; done
stress5: $(TASK5_BIN)
	for i in $$(seq 1 $(N)); do $(TIMEOUT) $(TMO) ./$(TASK5_BIN); echo "----"; done

capture-logs: all | $(LOG_DIR)
	./$(TASK1_BIN) | tee $(LOG_DIR)/task1.txt
	./$(TASK2_BIN) | tee $(LOG_DIR)/task2.txt
	./$(TASK3_BIN) | tee $(LOG_DIR)/task3.txt
	./$(TASK4_BIN) | tee $(LOG_DIR)/task4.txt
	./$(TASK5_BIN) | tee $(LOG_DIR)/task5.txt

clean:
	rm -rf $(BIN_DIR) $(LOG_DIR)

help:
	@echo "make all              -> build everything"
	@echo "make run4             -> run task 4"
	@echo "make test             -> run smoke tests"
	@echo "make stress4 N=5       -> run task 4 N times + checks"
	@echo "make capture-logs      -> save outputs into logs/"
	@echo "make clean            -> remove bin/ and logs/"