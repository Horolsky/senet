PLANTUML 	:= java -jar plantuml.jar
CFG_F		:= "global.skin"
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

SRC_D 		:= "$(ROOT_DIR)/src"
OUT_D		:= "$(ROOT_DIR)/output"

.PHONY: test all info


OUT := $(patsubst $(SRC_D)/%.puml, $(OUT_D)/%.png, $(wildcard $(SRC_D)/*.puml))

source = $(patsubst $(OUT_D)/%.png, $(SRC_D)/%.plantuml, $(1))

#build object file
$(OUT):
	@echo "processing " $@ "\n"\
	$(PLANTUML) -config $(CFG_F) $(call source $@) -o $@

all:
	$(PLANTUML) -config $(CFG_F) -o $(OUT_D) $(SRC_D)
	
# 
info:
	@echo $(OUT)

test:
	@echo i=$(call source "output/objects.png")