MODULES = sample batbox
MODULES_BIN = $(addsuffix .bin,$(MODULES))
MODULES_CLEAN = $(addsuffix .clean,$(MODULES))

all: $(MODULES)
bin: $(MODULES_BIN)
clean: $(MODULES_CLEAN)

$(MODULES):
	$(MAKE) -C $@ all 
	
$(MODULES_BIN):
	$(MAKE) -C $(basename $@) bin

$(MODULES_CLEAN):
	$(MAKE) -C $(basename $@) clean