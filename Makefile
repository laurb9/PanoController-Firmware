#
#
#

ARDUINO := arduino --verbose --preserve-temp-files
# Mac
#ARDUINO := Arduino.app/Contents/MacOS/Arduino --verbose --preserve-temp-files
TMPDIR := /tmp/gigapanplus-build

SOURCES := gigapanplus.ino *.cpp *.h

#
# Specify which of the below targets to build (default teensyLC)
#
all: teensyLC

# Arduino UNO target
# arduino options hardware/arduino/avr/boards.txt
uno: BOARD = arduino:avr:uno

# Teensy LC target
# Teensy options hardware/teensy/avr/boards.txt
teensyLC: BOARD = teensy:avr:teensyLC

# 
# Common rule for building and uploading the project
#
# See https://github.com/arduino/Arduino/blob/ide-1.5.x/build/shared/manpage.adoc
uno teensyLC: $(SOURCES) $(TMPDIR)
	$(ARDUINO) --upload $< --board $(BOARD) --pref build.path=$(TMPDIR)

#
# Menu definition generation
#
menu_cfg.cpp: menu_cfg.py menu_cfg.json
	python menu_cfg.py menu_cfg.json > $@
	
$(TMPDIR):
	mkdir -p $(TMPDIR)
