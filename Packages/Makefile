

PACKAGES = debian rpm freebsd

all: $(PACKAGES)
target: $(PACKAGES)

.PHONY: $(PACKAGES)

$(PACKAGES):
	cd $@ && $(MAKE)

