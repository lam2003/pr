CC      =g++ -std=c++14
TARGET  =pr
DIR_INC =${PWD}/include
DIR_SRC =${PWD}/src
DIR_OBJ =${PWD}/obj
SRC	=$(wildcard ${DIR_SRC}/*.cpp) 
OBJ	=$(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC})) 
CFLAGS	=-I$(DIR_INC) `pkg-config --cflags opencv --libs opencv` -DDISPLAY_PROCESS -DDEBUG -lpthread
${TARGET}:${OBJ}
	$(CC)   $^		$(CFLAGS)	-o			$@    
${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC)   -c      $<      	$(CFLAGS)    -o      $@

.PHONY:clean
clean:
	rm -rf  ${DIR_OBJ}/*.o
	rm -rf  ./${TARGET}
