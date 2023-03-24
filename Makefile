CXX=clang++
CPPFLAGS=-isystem${PWD} -Wall -Wextra -Werror
CXXFLAGS=-std=c++20

LOGIC_OBJECTS=Build/Logic/Card.o Build/Logic/Game.o Build/Logic/Hand.o Build/Logic/AIPlayer.o

default: terminal

terminal: Build/whist

Build/whist: Terminal/*.cpp Build/libwhist.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

Build/libwhist.a: $(LOGIC_OBJECTS)
	ar rcs $@ $^


Build/libwhist.so: $(LOGIC_OBJECTS)
	gcc -shared -o $@ $^

$(LOGIC_OBJECTS): Build/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC -c -o $@ $^

clean:
	@rm -rf ./Build/*