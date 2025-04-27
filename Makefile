BUILD_DIR := build
BIN_DIR := bin

all:
	@mkdir -p $(BUILD_DIR)
	@cmake -S . -B $(BUILD_DIR)
	@cmake --build $(BUILD_DIR) --target webserver

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
