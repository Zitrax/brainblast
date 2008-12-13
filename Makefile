# This Makefile just forwards to the real one in the src directory

MAKE    += -C src
TARGETS += all remake package install clean

default:
	$(MAKE)

$(TARGETS):
	$(MAKE) $@

.PHONY: default $(TARGETS)

