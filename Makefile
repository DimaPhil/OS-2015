DIRECTORIES = lib cat revwords filter bufcat simplesh filesender bipiper

all: $(DIRECTORIES)

$(DIRECTORIES):
	$(MAKE) -C $@

.PHONY: all $(DIRECTORIES)

clean:
	$(MAKE) -C $(CURDIR)/lib clean
	$(MAKE) -C $(CURDIR)/cat clean
	$(MAKE) -C $(CURDIR)/revwords clean
	$(MAKE) -C $(CURDIR)/filter clean
	$(MAKE) -C $(CURDIR)/bufcat clean
	$(MAKE) -C $(CURDIR)/simplesh clean
	$(MAKE) -C $(CURDIR)/filesender clean
	$(MAKE) -C $(CURDIR)/bipiper clean
