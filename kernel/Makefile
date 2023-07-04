NAME := kernel
SRCS := kernel.c
CFLAGS += -DDEBUG -ggdb
export MODULE  := L0
export AM_HOME := $(PWD)/../abstract-machine
ifeq ($(ARCH),)
export ARCH := x86_64-qemu
endif

include $(AM_HOME)/Makefile
include ../Makefile.lab
image: git

QEMUFLAGS += -m 256M -smp 1


go:
	qemu-system-x86_64 $(QEMUFLAGS) -nographic build/kernel-x86_64-qemu
test:
	qemu-system-x86_64 $(QEMUFLAGS) -S -s -serial none -nographic build/kernel-x86_64-qemu
