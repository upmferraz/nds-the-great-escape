# NDS Standard Makefile (v12 - Circular Dependency Fix)
.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

# --- CONFIGURAÇÃO ---
TARGET      := GreatEscape
BUILD       := build
SOURCES     := source
INCLUDES    := include
DATA        := data

# 1. Configuração de Arquitetura
ARCH        := -mthumb -mthumb-interwork
CFLAGS      := -g -Wall -O2 -mcpu=arm946e-s -mtune=arm946e-s \
               -fomit-frame-pointer -ffast-math \
               -DARM9 $(ARCH)

CXXFLAGS    := $(CFLAGS) -fno-rtti -fno-exceptions

# --- LÓGICA DE BUILD ---

ifneq ($(BUILD),$(notdir $(CURDIR)))
# --- FASE 1: Raiz ---

export ROOT_DIR := $(CURDIR)
export OUTPUT   :=  $(CURDIR)/$(TARGET)
export VPATH    :=  $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR  :=  $(CURDIR)/$(BUILD)

CFILES      :=  $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES    :=  $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES      :=  $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES      :=  $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES    :=  $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

export OFILES   :=  $(addsuffix .o,$(BINFILES)) \
                    $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
                    $(sFILES:.s=.o) $(SFILES:.S=.o)

export CFLAGS
export CXXFLAGS

.PHONY: $(BUILD) clean all

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@echo "-> Entrando no build com ROOT_DIR=$(ROOT_DIR)"
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).nds $(TARGET).elf $(TARGET).map

else
# --- FASE 2: Build ---

LIBNDS_PATH := /opt/devkitpro/libnds
CALICO_PATH := /opt/devkitpro/calico

INCLUDE     := -I$(ROOT_DIR)/$(INCLUDES) \
               -I$(LIBNDS_PATH)/include \
               -I$(CALICO_PATH)/include

CFLAGS      += $(INCLUDE)
CXXFLAGS    += $(INCLUDE)

# Caminho para specs e bibliotecas
LDFLAGS     := -g $(ARCH) -specs=$(CALICO_PATH)/share/ds9.specs -Wl,-Map,$(notdir $@).map \
               -L$(LIBNDS_PATH)/lib \
               -L$(CALICO_PATH)/lib

# [CORREÇÃO FINAL] --start-group resolve a dependência circular
# O linker vai ciclar entre nds9 e calico_ds9 até resolver tudo.
LIBS        := -Wl,--start-group -lnds9 -lcalico_ds9 -Wl,--end-group -lm

DEPENDS :=  $(OFILES:.o=.d)

$(OUTPUT).nds   :   $(OUTPUT).elf
$(OUTPUT).elf   :   $(OFILES)
	@echo linking $(notdir $@)
	@$(CXX) $(LDFLAGS) $(OFILES) $(LIBS) -o $@
	@echo "✅ BUILD SUCCESS! $(notdir $@) created."

-include $(DEPENDS)

endif
