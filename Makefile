#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif
export LIBOGC_INC	:=	$(DEVKITPRO)/libogc/include
export LIBOGC_LIB	:=	$(DEVKITPRO)/libogc/lib/wii
export PORTLIBS		:=	$(DEVKITPRO)/portlibs/ppc

TOPDIR ?= $(CURDIR)

#-------------------------------------------------------------------------------
# APP_NAME sets the long name of the application
# APP_SHORTNAME sets the short name of the application
# APP_AUTHOR sets the author of the application
#-------------------------------------------------------------------------------
#APP_NAME	:= Application Name
#APP_SHORTNAME	:= App Name
#APP_AUTHOR	:= Built with devkitPPC & wut

include $(DEVKITPRO)/wut/share/wut_rules

#-------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
#-------------------------------------------------------------------------------
TARGET		:=	magiquest-wiiu
BUILD		:=	build
BUILD_DBG	:=	$(TARGET)_dbg
CODEDIR     :=  code
SOURCES		:=  src

#-------------------------------------------------------------------------------
# options for code generation
#-------------------------------------------------------------------------------
CFLAGS	:=	-Wall -O4 -ffunction-sections -fexceptions -fno-use-linker-plugin -fno-lto \
			$(MACHDEP)

CFLAGS	+=	$(INCLUDE) -D__WIIU__ -D__WUT__ -Wno-narrowing

CXXFLAGS	:= $(CFLAGS)

ASFLAGS	:=	$(ARCH)
LDFLAGS	=	$(ARCH) $(RPXSPECS) -Wl,-Map,$(notdir $*.map) -Wl,--gc-sections

LIBS	:=  -lgui -lfreetype -lgd -lpng -ljpeg -lz -lopusfile -lopus -logg -lmad -lwut

#-------------------------------------------------------------------------------
# list of directories containing libraries
#-------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(WUT_ROOT) $(WUT_ROOT)/usr $(CURDIR)/vendor/libopus $(CURDIR)/vendor/opusfile $(CURDIR)/vendor/libgui
INCLUDES	:= $(CURDIR)/src $(PORTLIBS)/include/freetype2

#-------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#-------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#-------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(CODEDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export CONTENT 	:=  $(CURDIR)/content
export META		:=	$(CURDIR)/meta

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.bin)))

#-------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#-------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#-------------------------------------------------------------------------------
	export LD	:=	$(CC)
#-------------------------------------------------------------------------------
else
#-------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------

export OFILES	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o) \
					$(BINFILES:.bin=.o)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD) -I$(LIBOGC_INC) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)) $(foreach dir,$(LIBDIRS),-L$(dir)/lib)


.PHONY: $(BUILD) clean all wuhb

#-------------------------------------------------------------------------------
all: $(BUILD)
wuhb:
	@[ -d $(BUILD) ] || mkdir -p $(BUILD) $(CODEDIR)
	@$(MAKE) wuhb --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(BUILD):
	@[ -d $@ ] || mkdir -p $@ $(CODEDIR)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#-------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(CODEDIR) $(TARGET).elf $(OUTPUT).bin

#-------------------------------------------------------------------------------
else
.PHONY:	all wuhb

DEPENDS	:=	$(OFILES:.o=.d)

#-------------------------------------------------------------------------------
# main targets
#-------------------------------------------------------------------------------
all:	$(OUTPUT).rpx
wuhb:	$(OUTPUT).wuhb

# https://stackoverflow.com/a/18258352
rwildcard=$(foreach d,$(wildcard $(1:=/*)), $(call rwildcard,$d,$2) $(filter $(subst *,%,$2), $d))

$(OUTPUT).wuhb:	$(OUTPUT).rpx $(META)/boot.png $(META)/iconTex.png $(call rwildcard,$(CONTENT),*)
	@wuhbtool $(OUTPUT).rpx $@ --content=$(CONTENT) --icon=$(META)/iconTex.png --tv-image=$(META)/boot.png --drc-image=$(META)/boot.png
$(OUTPUT).rpx:	$(OUTPUT).elf
$(OUTPUT).elf:	$(OFILES)

$(OFILES_SRC): $(HFILES_BIN)

#-------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#-------------------------------------------------------------------------------
%.o: %.cpp
	@echo $(notdir $<)
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
%.o: %.c
	@echo $(notdir $<)
	$(SILENTCMD)$(CC) -MMD -MP -MF $*.d $(CFLAGS) $(if $(filter memdebug.o,$@),-fno-sanitize=all )-c $< -o $@ $(ERROR_FILTER)

#-------------------------------------------------------------------------------
%.o %.bin.o : %.bin
#-------------------------------------------------------------------------------
	@echo $(notdir $<)
	@bin2s -a 32 $< | $(AS) -o $(@)

-include $(DEPENDS)

#-------------------------------------------------------------------------------
endif
