-include package_version

include opusfile_sources.mk

EXESUFFIX =
LIBPREFIX = lib
LIBSUFFIX = .a
OBJSUFFIX = .o

TOOLCHAIN_PREFIX = $(DEVKITPPC)/bin/powerpc-eabi-

CC     = $(TOOLCHAIN_PREFIX)gcc$(TOOLCHAIN_SUFFIX)
AR     = $(TOOLCHAIN_PREFIX)ar
RANLIB = $(TOOLCHAIN_PREFIX)ranlib
CP     = $(TOOLCHAIN_PREFIX)cp

cppflags-from-defines   = $(addprefix -D,$(1))
cppflags-from-includes  = $(addprefix -I,$(1))
ldflags-from-ldlibdirs  = $(addprefix -L,$(1))
ldlibs-from-libs        = $(addprefix -l,$(1))

WARNINGS = -Wall -W -Wstrict-prototypes -Wextra -Wcast-align -Wnested-externs -Wshadow
CFLAGS  += -O2 -g $(WARNINGS) -DHAVE_CONFIG_H
CINCLUDES = include . ../libopus/include $(DEVKITPRO)/portlibs/ppc/include

CFLAGS  += $(call cppflags-from-defines,$(CDEFINES))
CFLAGS  += $(call cppflags-from-includes,$(CINCLUDES))

COMPILE.c.cmdline   = $(CC) -c $(CFLAGS) -o $@ $<

ARCHIVE.cmdline     = $(AR) $(ARFLAGS) $@ $^ && $(RANLIB) $@

%$(OBJSUFFIX):%.c
	$(COMPILE.c.cmdline)

%$(OBJSUFFIX):%.cpp
	$(COMPILE.cpp.cmdline)

# Variable definitions
LIB_NAME = opusfile
TARGET = $(LIBPREFIX)$(LIB_NAME)$(LIBSUFFIX)

SRCS_C = $(OPUSFILE_SOURCES)

OBJS := $(patsubst %.c,%$(OBJSUFFIX),$(SRCS_C))

# Rules
all: lib

lib: $(TARGET)

$(TARGET): $(OBJS)
	$(ARCHIVE.cmdline)

package_version: force
	@if [ -x ./update_version ]; then \
		./update_version || true; \
	elif [ ! -e ./package_version ]; then \
		echo 'PACKAGE_VERSION="unknown"' > ./package_version; \
	fi

force:

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all lib clean force
