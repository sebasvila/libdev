#################################################################

PLATFORM=ArduinoONE
DEVICE=/dev/ttyACM0

CC=avr-gcc
CPPFLAGS=-I../../libaire/src
CFLAGS=-Wall -std=gnu99 -Os -fshort-enums
LDFLAGS=-L../../libaire/build -std=c99
LDLIBS=-laire
AR=avr-ar

#################################################################

SRC_LIB = libdev.a

SRC_MODS = lcd motor shielditic rtc1307 bcd

SRC_TESTS = test_rtc1307_1

# Library deps
$(SRC_LIB): $(SRC_LIB)($(addsuffix .o, $(SRC_MODS)))


# Link rules for tests
test_rtc1307_1: bcd.o rtc1307.o


#################################################################


# Platform and shield dependent parameters

ifeq ($(PLATFORM),ArduinoONE)
  MCU = atmega328p
  PROGRAMMER = arduino
  DUDEOPTS =
  FREQ = 16000000UL
else ifeq ($(PLATFORM),ArduinoMEGA)
  MCU = atmega2560
  PROGRAMMER = wiring
  DUDEOPTS = -D
  FREQ = 16000000UL
endif



# Processing parameters

CFLAGS   +=  -mmcu=$(MCU)
LDFLAGS  +=  -mmcu=$(MCU)
CPPFLAGS +=  -D$(PLATFORM) -DF_CPU=$(FREQ) -I$(SRCDIR)


# Project structure

DEPSDIR = .deps
SRCDIR  = ../src
TESTDIR = ../test
BUILDDIR = .


# Sets of files

SOURCES = $(SRC_MODS) $(SRC_TESTS)
DEPFILES = $(addprefix $(DEPSDIR)/, $(addsuffix .d, $(SOURCES)))

# Search path

VPATH = $(SRCDIR):$(TESTDIR)


# Default targets

.PHONY: lib tests
lib:	$(SRC_LIB)
tests:  $(SRC_TESTS)


# Utility targets

.PHONY: clean veryclean
clean:
	@\rm -f *~ *.o *.s *.hex

veryclean: clean
	@\rm -f  \#*\# $(SRC_LIB) $(SRC_TESTS)
	@\rm -rf $(DEPSDIR)



# Generic rules

%.hex: %
	avr-objcopy -Oihex  $< $@

implanta_%: %.hex
	avrdude $(DUDEOPTS) -c $(PROGRAMMER) -p $(MCU) -P $(DEVICE) -U $<

%.s: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -S $<

$(DEPSDIR)/%.d: %.c | $(DEPSDIR)
	@set -e; rm -f $@; \
        $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(DEPSDIR):
	 @mkdir -p $@


# Automagic dependencies

-include $(DEPFILES)
