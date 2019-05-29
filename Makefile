CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -g -I/usr/include/opencv4 -Isrc
LDFLAGS = -g -lopencv_highgui -lopencv_imgcodecs -lopencv_xphoto \
		-lopencv_photo -lopencv_imgproc -lopencv_core -lopencv_videostab \
		-lopencv_video -lopencv_videoio

SRC = src/main.cpp src/BasicFbdrv.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = fbdrv

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LBLIBS)

clean:
	rm -rf $(OBJ) $(EXEC)
