# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
BUILD_DIR = build
TARGET = my_project

# Check for global vcpkg first, fallback to local
VCPKG_ROOT ?= $(shell if [ -n "$$VCPKG_ROOT" ]; then echo "$$VCPKG_ROOT"; elif [ -d "$(HOME)/vcpkg" ]; then echo "$(HOME)/vcpkg"; else echo "$(PWD)/vcpkg"; fi)
VCPKG_TOOLCHAIN = $(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake

# Default target
.PHONY: all
all: build

# Build the project
.PHONY: build
build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN) && make
	@echo "✓ Build complete: $(BUILD_DIR)/$(TARGET)"

# Run the program
.PHONY: run
run: 
	@./$(BUILD_DIR)/$(TARGET)

# Run tests
.PHONY: test
test: build
	@cd $(BUILD_DIR) && ctest --output-on-failure

# Clean build artifacts
.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
	@echo "✓ Cleaned build directory"

# Install vcpkg globally (one-time setup)
.PHONY: setup
setup:
	@if [ -n "$$VCPKG_ROOT" ]; then \
		echo "✓ Using global vcpkg at $$VCPKG_ROOT"; \
	elif [ -d "$(HOME)/vcpkg" ]; then \
		echo "✓ Using global vcpkg at $(HOME)/vcpkg"; \
	else \
		echo "Installing vcpkg globally to $(HOME)/vcpkg..."; \
		cd $(HOME) && git clone https://github.com/Microsoft/vcpkg.git; \
		cd $(HOME)/vcpkg && ./bootstrap-vcpkg.sh; \
		echo ""; \
		echo "✓ vcpkg installed!"; \
		echo ""; \
		echo "Add this to your ~/.zshrc (or ~/.bash_profile):"; \
		echo "  export VCPKG_ROOT=\"\$$HOME/vcpkg\""; \
		echo "  export PATH=\"\$$VCPKG_ROOT:\$$PATH\""; \
		echo ""; \
		echo "Then run: source ~/.zshrc"; \
	fi

# Help
.PHONY: help
help:
	@echo "Available commands:"
	@echo "  make build   - Build the project"
	@echo "  make run     - Build and run the project"
	@echo "  make test    - Build and run tests"
	@echo "  make clean   - Remove build directory"
	@echo "  make setup   - Install vcpkg (one-time)"
	@echo "  make help    - Show this help"