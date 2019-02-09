# Default build architecture and board
TARGET ?= adafruit:samd:adafruit_feather_m0

# Libraries needed for this application
LIBS ?= "StepperDriver" "Adafruit BluefruitLE nRF51" "Adafruit SSD1306" "Adafruit GFX Library"

# Default list of cores to install with `make setup`
CORES ?= adafruit:samd

# Where to save the Arduino support files, this should match what is in .cli-config.yml
ARDUINO_DIR ?= .arduino

default:
	#################################################################################################
	# Initial setup: make .arduino/arduino-cli setup
	#
	# Build: make PanoController.hex TARGET=adafruit:samd:adafruit_feather_m0
	#
	# Install more cores: make setup CORES=arduino:samd
	# (edit .cli-config.yml and add repository if needed)
	#################################################################################################

ARDUINO_CLI_URL = http://downloads.arduino.cc/arduino-cli/arduino-cli-latest-linux64.tar.bz2
ARDUINO_CLI ?= $(ARDUINO_DIR)/arduino-cli --config-file .cli-config.yml
EXAMPLES := $(shell ls examples)

all: # Build all example sketches
all: $(EXAMPLES:%=%.hex)

%.hex: # Generic rule for compiling sketch to uploadable hex file
%.hex: examples/%
	$(ARDUINO_CLI) compile --warnings all --fqbn $(TARGET) $< --output $@
	ls -l $@*

# Remove built objects
clean:
	rm -fv $(EXAMPLES:%=%.{hex,elf}*)

$(ARDUINO_DIR)/arduino-cli:  # Download and install arduino-cli
$(ARDUINO_DIR)/arduino-cli:
	mkdir -p $(ARDUINO_DIR)
	cd $(ARDUINO_DIR)
	curl -s $(ARDUINO_CLI_URL) \
	| tar xfj - -O -C $(ARDUINO_DIR) \
	> $@
	chmod 755 $@

setup: # Configure cores and libraries for arduino-cli (which it will download if missing)
setup: $(ARDUINO_DIR)/arduino-cli
	mkdir -p $(ARDUINO_DIR)/libraries
	ln -sf $(CURDIR) $(ARDUINO_DIR)/libraries/
	$(ARDUINO_CLI) config dump
	$(ARDUINO_CLI) core update-index
	$(ARDUINO_CLI) core install $(CORES)
	$(ARDUINO_CLI) core list
	for lib in $(LIBS); do \
		$(ARDUINO_CLI) lib install "$${lib}" || true ; \
	done

.PHONY: clean %.hex all setup
