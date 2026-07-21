BUILD         := build
SOURCES       := $(shell find src -name '*.cpp')
OBJECTS       := $(patsubst src/%.cpp,$(BUILD)/%.o,$(SOURCES))
DEPENDENCIES  := $(OBJECTS:.o=.d)
TARGET        := $(BUILD)/Dynamox
CC  		  := g++
CCFLAGS  	  := -Wall -Wextra -std=c++2c -pedantic -Iinclude -g
LDFLAGS  	  := 

execute: $(TARGET)
	xterm -hold -e "./$(TARGET) CLIENT" &
	xterm -hold -e "./$(TARGET) SERVER" &

kill:
	pkill -f "Dynamox CLIENT"
	pkill -f "Dynamox SERVER"

build: $(TARGET)

clean:
	rm -rf $(BUILD)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD)/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -MMD -MP -c $< -o $@

-include $(DEPENDENCIES)
