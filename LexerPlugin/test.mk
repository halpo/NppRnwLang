
COPY ?= cp
BIN_DIR ?= ../bin
CONFIG_DIR ?= $(BIN_DIR)/config
DEL ?= rm -f

vpath %.dll $(BIN_DIR)
vpath %.xml $(CONFIG_DIR)

CONFIG_FILES ?= $(addsuffix .xml, $(basename $(PLUGIN)))

UNICODE ?= 1
ifeq ($(UNICODE), 1)
NPP_DIR         ?= ../Npp_unicode
else
NPP_DIR         ?= ../Npp_ansi
endif
NPP_EXE         ?= $(NPP_DIR)/Notepad++.exe
NPP_PLUGINS_DIR ?= $(NPP_DIR)/plugins
NPP_PLUGINS_CONFIG_DIR ?= $(NPP_PLUGINS_DIR)/Config
NPP_FLAGS += -multiInst 

NPP_CONFIG_FILES ?= $(addprefix $(NPP_PLUGINS_CONFIG_DIR)/, $(CONFIG_FILES))

.PHONEY: copy run test cleantest
test:copy run

$(NPP_PLUGINS_DIR)/$(PLUGIN): $(BIN_DIR)/$(PLUGIN)
	$(COPY) -f $< $@
  
$(NPP_PLUGINS_CONFIG_DIR)/%.xml: $(CONFIG_DIR)/%.xml 
	$(COPY) -f $< $@

copy: $(NPP_PLUGINS_DIR)/$(PLUGIN) $(NPP_CONFIG_FILES)

run:
	$(NPP_EXE) $(NPP_FLAGS) $(TEST_FILES)
cleantest:
	$(DEL) $(NPP_PLUGINS_CONFIG_DIR)/*.xml $(NPP_PLUGINS_DIR)/*.dll
echo:
	echo $(CONFIG_FILES)