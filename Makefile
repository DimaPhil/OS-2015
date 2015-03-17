DIRECTORIES = lib cat revwords

all: $(DIRECTORIES)

$(DIRECTORIES):
	$(MAKE) -C $@

.PHONY: all $(DIRECTORIES)

clean:
	$(MAKE) -C $(CURDIR)/lib clean
	$(MAKE) -C $(CURDIR)/cat clean
	$(MAKE) -C $(CURDIR)/revwords clean

