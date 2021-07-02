CC := gcc

PROGNAME := mozxdg

PREFIX  ?= $(HOME)/.local
BINDIR  ?= bin
DESTDIR ?= $(PREFIX)/$(BINDIR)
DESTBIN  = $(DESTDIR)/$(PROGNAME)

LINK_DIR    ?= $(HOME)/.local/bin
WRAPPED_BIN ?= firefox

PROJECT_DIR   := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD         := $(PROJECT_DIR)/build
BUILD_OBJDIR  := $(BUILD)/obj
BUILD_BINDIR  := $(BUILD)/bin
TARGET        := $(BUILD_BINDIR)/$(PROGNAME)
EXTERN_DIR    := $(PROJECT_DIR)/extern
BUILD_DEPSDIR := $(BUILD)/extern

INCLUDE := -I $(BUILD_DEPSDIR)/include

CFLAGS  := -std=gnu99 -Wall -Wextra -pedantic
LDFLAGS := -L $(BUILD_DEPSDIR)/lib


.PHONY: all dirs deps run clean install link-firefox link-thunderbird link-other uninstall

all: dirs deps $(TARGET)

dirs:
	@mkdir -p $(BUILD_BINDIR) $(BUILD_DEPSDIR)

debug: CFLAGS += -DDEBUG -g
debug: all

release: CFLAGS += -O2
release: all

PROPGWRAP_H := $(BUILD_DEPSDIR)/include/progwrap.h

deps: $(PROPGWRAP_H)

$(PROPGWRAP_H):
	cd $(EXTERN_DIR)/libProgWrap && LIBDIR=$(BUILD_DEPSDIR)/lib INCLUDEDIR=$(BUILD_DEPSDIR)/include ./install.sh -l c


$(TARGET): main.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $(TARGET) main.c sys_helpers.c $(LDFLAGS)

run: all
	$(TARGET) $(RUN_ARGS)

clean:
	rm -rf $(BUILD)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTBIN)

link-all: link-firefox link-thunderbird

link-firefox:
	ln -sf $(DESTBIN) $(LINK_DIR)/firefox

link-thunderbird:
	ln -sf $(DESTBIN) $(LINK_DIR)/thunderbird

link-other:
	ln -sf $(DESTBIN) $(LINK_DIR)/$(WRAPPED_BIN)

uninstall:
	rm $(DESTBIN)
