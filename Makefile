# This Makefile just forwards to the real one in the src directory

MAKE    += -C src
TARGETS += all remake package install clean depend rmp lcov

default:
	$(MAKE)

$(TARGETS):
	$(MAKE) $@

.PHONY: default $(TARGETS)

