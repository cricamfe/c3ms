# Configuración básica
INCL := include
SRC := src
OBJ := obj
BIN := bin
DEBUG_DIR := debug
RELEASE_DIR := release

# Compiladores
CC := gcc
CXX := g++

# Modo de compilación
ifdef DEBUG
  BUILD_DIR := $(DEBUG_DIR)
  CFLAGS := -I$(INCL) -g -DDEBUG
  CPPFLAGS := -I$(INCL) -g -DDEBUG
else
  BUILD_DIR := $(RELEASE_DIR)
  CFLAGS := -I$(INCL) -O3
  CPPFLAGS := -I$(INCL) -O3
endif

# Archivos fuente explícitos
# Añadir aquí nuevos archivos .cpp
SRC_FILES := src/CodeMetrics.cpp src/CodeUtils.cpp c3ms.cpp

# Archivos objeto
OBJS := $(patsubst %.cpp,$(OBJ)/$(BUILD_DIR)/%.o,$(notdir $(SRC_FILES))) \
        $(OBJ)/$(BUILD_DIR)/c3ms.tab.o \
        $(OBJ)/$(BUILD_DIR)/c3mslex.o

# Nombre del ejecutable
EXECUTABLE := c3ms

# Reglas
all: setup $(BIN)/$(BUILD_DIR)/$(EXECUTABLE)

setup:
	@mkdir -p $(OBJ)/$(DEBUG_DIR) $(OBJ)/$(RELEASE_DIR)
	@mkdir -p $(BIN)/$(DEBUG_DIR) $(BIN)/$(RELEASE_DIR)

$(BIN)/$(BUILD_DIR)/$(EXECUTABLE): $(OBJS)
	$(CXX) $(CPPFLAGS) -o $@ $^

$(OBJ)/$(BUILD_DIR)/%.o: $(SRC)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJ)/$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@

# Reglas para bison y flex
$(OBJ)/$(BUILD_DIR)/c3ms.tab.o: $(INCL)/c3ms.tab.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(INCL)/c3ms.tab.cpp: $(INCL)/c3ms.y
	bison -d -o $@ $<

$(OBJ)/$(BUILD_DIR)/c3mslex.o: $(INCL)/c3mslex.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(INCL)/c3mslex.cpp: $(INCL)/c3ms.l $(INCL)/c3ms.tab.cpp
	flex -Cemr -o $@ $<

# Limpieza
clean:
	-@rm -f $(OBJ)/$(DEBUG_DIR)/*.o $(OBJ)/$(RELEASE_DIR)/*.o $(INCL)/c3ms.tab* $(INCL)/c3mslex.cpp

veryclean: clean
	-@rm -rf $(BIN)/$(DEBUG_DIR)/$(EXECUTABLE) $(BIN)/$(RELEASE_DIR)/$(EXECUTABLE)