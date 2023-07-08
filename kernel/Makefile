NAME := kernel
SRCS := kernel.c picture.c
CFLAGS += -DDEBUG -ggdb
export MODULE  := L0
export AM_HOME := $(PWD)/../abstract-machine
ifeq ($(ARCH),)
export ARCH := x86_64-qemu
endif

include $(AM_HOME)/Makefile
include ../Makefile.lab
image: git

QEMUFLAGS += -m 4M -smp 1 -vga std


go:
	qemu-system-x86_64 $(QEMUFLAGS) build/kernel-x86_64-qemu
test:
	qemu-system-x86_64 $(QEMUFLAGS) -S -s build/kernel-x86_64-qemu
