# Define directory paths
SRC         := ./src
BIN         := ./bin
BUILD_DIR   := ./build
SCRIPTS     := ./scripts
INCLUDE     := ./include

# Collect all C source files in the directory
SOURCE_FILES = $(wildcard $(SRC)/*.c)
TMP_SOURCE_FILES := $(SOURCE_FILES:$(SRC)/%=%) # Remove the src/ prefix
OBJ_FILES := $(addprefix $(BUILD_DIR)/,$(TMP_SOURCE_FILES:.c=.o)) # Append build/ and .o

###########################################################################################
#	COMPILATION
###########################################################################################

all: clean compile

# Compile object files
object: $(OBJ_FILES)

# Compile the parser
compile: $(INCLUDE) $(SRC) $(BIN)
	gcc -g -I $(INCLUDE) src/**.c -lm -o $(BIN)/avm.out
	
# Compile each source file into its object file individually
$(BUILD_DIR)/%.o: $(SRC)/%.c | $(BUILD_DIR)
	gcc -I $(INCLUDE) -c $< -o $@

###########################################################################################
#	SCRIPTS
###########################################################################################

##########################################################################################
#	UTILITIES
###########################################################################################

# Clean up build artifacts
clean:
	@rm -rf $(BUILD_DIR) $(BIN) *.txt .vscode test_results

# Ensure that the build and bin directories exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN):
	mkdir -p $(BIN)
	
