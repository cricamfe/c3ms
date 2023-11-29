# Copyright (c) 2009, 2010 Basilio B. Fraguela. Universidade da Coruña
# Upgrade (2023) by Campos-Ferrer, Cristian. Universidad de Málaga

INCL := include
SRC := src
OBJ := obj
BIN := bin
DEBUG_DIR := debug
RELEASE_DIR := release

CC  := gcc
CXX := g++

# Determinar modo de compilación y configurar las carpetas correspondientes
ifdef DEBUG
  BUILD_DIR := $(DEBUG_DIR)
  CFLAGS := -I$(INCL) -g -DDEBUG
  CPPFLAGS := -I$(INCL) -g -DDEBUG
else
  BUILD_DIR := $(RELEASE_DIR)
  CFLAGS := -I$(INCL) -O3
  CPPFLAGS := -I$(INCL) -O3
endif

# Definir archivos fuente y objetos
SRCS := $(wildcard $(SRC)/*.cpp) $(wildcard *.cpp) $(INCL)/c3ms.tab.cpp $(INCL)/c3mslex.cpp
OBJS := $(patsubst $(SRC)/%.cpp,$(OBJ)/$(BUILD_DIR)/%.o,$(wildcard $(SRC)/*.cpp)) \
        $(patsubst %.cpp,$(OBJ)/$(BUILD_DIR)/%.o,$(wildcard *.cpp)) \
        $(patsubst $(INCL)/%.cpp,$(OBJ)/$(BUILD_DIR)/%.o,$(INCL)/c3ms.tab.cpp $(INCL)/c3mslex.cpp)

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

$(OBJ)/$(BUILD_DIR)/%.o: $(INCL)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(INCL)/c3ms.tab.cpp: $(INCL)/c3ms.y
	bison -d -o $@ $<

$(INCL)/c3mslex.cpp: $(INCL)/c3ms.l $(INCL)/c3ms.tab.cpp
	flex -Cemr -o $@ $<

clean:
	-@rm -f $(OBJ)/$(DEBUG_DIR)/*.o $(OBJ)/$(RELEASE_DIR)/*.o $(INCL)/c3ms.tab* $(INCL)/c3mslex.cpp

veryclean: clean
	-@rm -rf $(BIN)/$(DEBUG_DIR)/$(EXECUTABLE) $(BIN)/$(RELEASE_DIR)/$(EXECUTABLE)
