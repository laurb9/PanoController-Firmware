# Default build architecture and board
TARGET ?= adafruit:samd:adafruit_feather_m0
CORE = $(shell echo $(TARGET) | cut -d: -f1,2)
LIBS ?= "StepperDriver" "Adafruit BluefruitLE nRF51" "Adafruit SSD1306" "Adafruit GFX Library"

# Where to save the Arduino support files, this should match what is in arduino-cli.yaml
ARDUINO_DIR ?= .arduino

default:
	#################################################################################################
	# Initial setup: make .arduino/arduino-cli setup
	#
	# Build all the examples: make all TARGET=adafruit:samd:adafruit_feather_m0
	#
	# Install more cores: make core TARGET=adafruit:samd:adafruit_feather_m0
	# (edit arduino-cli.yaml and add repository if needed)
	#################################################################################################

# See https://arduino.github.io/arduino-cli/installation/
ARDUINO_CLI_URL = https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz
ARDUINO_CLI ?= $(ARDUINO_DIR)/arduino-cli --config-file arduino-cli.yaml

COMPILE = $(ARDUINO_CLI) compile --warnings all --fqbn $(TARGET)

all: PanoController

PanoController:: examples/PanoController core
	$(ARDUINO_CLI) compile --warnings all --fqbn $(TARGET) --output-dir build $<
	ls -l build

# Remove built objects
clean:
	rm -rfv build

core: $(ARDUINO_DIR)/arduino-cli
	$(ARDUINO_CLI) core install $(CORE)
	for lib in $(LIBS); do \
		$(ARDUINO_CLI) lib install "$${lib}" || true ; \
	done

$(ARDUINO_DIR)/arduino-cli:  # Download and install arduino-cli
$(ARDUINO_DIR)/arduino-cli:
	mkdir -p $(ARDUINO_DIR)
	cd $(ARDUINO_DIR)
	curl -L -s $(ARDUINO_CLI_URL) \
	| tar xfz - -C $(ARDUINO_DIR) arduino-cli
	chmod 755 $@
	$(ARDUINO_CLI) version

setup: # Configure cores and libraries for arduino-cli (which it will download if missing)
setup: $(ARDUINO_DIR)/arduino-cli
	mkdir -p $(ARDUINO_DIR)/libraries
	ln -sf $(CURDIR) $(ARDUINO_DIR)/libraries/
	$(ARDUINO_CLI) config dump
	$(ARDUINO_CLI) core update-index
	$(ARDUINO_CLI) core list

.PHONY: clean all setup PanoController
