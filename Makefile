CXX=g++
LDFLAGS=$(shell pkg-config --static --libs glew glfw3)
CPPFLAGS=$(shell pkg-config --cflags glew glfw3) -g
CXXFLAGS=-Wall -O0

FILES=src/main.o src/tesseract.o src/controls.o src/more.o src/config.o src/gpuprogram.o src/noise.o src/world.o src/project.o

mc4d: $(FILES)
	$(CXX) $(CXXFLAGS) $(FILES) -o mc4d $(LDFLAGS)

# Pull in generated dependency information for existing .o files
-include $(FILES:.o=.d)

# Modify the default c++ compiler rule to generate dependency info too
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $*.cpp -o $*.o
	$(CXX) -MM $(CXXFLAGS) $(CPPFLAGS) $*.cpp > $*.d

# Destroy everything we can generate
.PHONY: clean
clean:
	rm src/*.o
	rm src/*.d
	rm src/*Shader.h
	rm mc4d

# Run the project
.PHONY: run
run: mc4d
	./mc4d

# Special generation rules for shaders to embed in program
src/vertShader.h: src/vert.glsl
	echo "char vertGlsl[] = {" > src/vertShader.h
	xxd -i < src/vert.glsl >> src/vertShader.h
	echo ", 0x00\n};" >> src/vertShader.h

src/fragShader.h: src/frag.glsl
	echo "char fragGlsl[] = {" > src/fragShader.h
	xxd -i < src/frag.glsl >> src/fragShader.h
	echo ", 0x00\n};" >> src/fragShader.h

# Ensure that the shader headers have been built before the main program is compiled
# Unfortunately, -MM assumes that the header files aren't generated
src/main.o: src/vertShader.h src/fragShader.h
