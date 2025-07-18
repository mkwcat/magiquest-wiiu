#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)

include $(DEVKITPRO)/wut/share/wut_rules

export VER_MAJOR	:=	1
export VER_MINOR	:=	0
export VER_PATCH	:=	1

VERSION	:=	$(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)

#-------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
#-------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source \
				source/fs \
				source/gui \
				source/system \
				source/video \
				source/video/shaders \
		
DATA		:=	data
INCLUDES	:=	source \
				include
DEPSDIR     :=  build

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CFLAGS	:=	-Wall -Werror -save-temps \
			-ffunction-sections -fdata-sections \
			$(MACHDEP) \
			$(BUILD_CFLAGS)

CFLAGS	+=	$(INCLUDE) -D__WIIU__ -D__WUT__

CXXFLAGS	:= $(CFLAGS) -std=gnu++17

ASFLAGS	:=	$(MACHDEP)

LDFLAGS	=	$(ARCH) -Wl,--gc-sections


LIBS	:= 

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(WUT_ROOT)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq (build,$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
DEFFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.def)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(DEFFILES:.def=.o) $(SFILES:.s=.o) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I. -I$(PORTLIBS_PATH)/ppc/include/freetype2

.PHONY: all dist-bin dist-src dist install clean

#---------------------------------------------------------------------------------
all: lib/libgui.a

lib/libgui.a :$(SOURCES) $(INCLUDES)
	@mkdir -p build lib
	@$(MAKE) OUTPUT=$(CURDIR)/$@ \
	BUILD_CFLAGS="-DNDEBUG=1 -O2 -s" \
	--no-print-directory -C build \
	BUILD=$(CURDIR)/build DEPSDIR=$(CURDIR)/build \
	-f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -rf build lib

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT)	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES)

#---------------------------------------------------------------------------------
%_bin.h %.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)


-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
