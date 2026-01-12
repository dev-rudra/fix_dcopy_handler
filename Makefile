BUILD_DIR := build
CMAKE := cmake

.PHONY: build clean run

build:
	@mkdir -p $(BUILD_DIR)
	@$(CMAKE) -S . -B $(BUILD_DIR)
	@$(CMAKE) --build $(BUILD_DIR)

clean:
	@rm -rf $(BUILD_DIR)

run: build
	@./$(BUILD_DIR)/fix_dropcopy
