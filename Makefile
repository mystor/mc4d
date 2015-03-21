CXX=g++
LDFLAGS=$(shell pkg-config --static --libs glew glfw3)
CPPFLAGS=$(shell pkg-config --cflags glew glfw3) -g
CXXFLAGS=-Wall -O0

FILES=src/main.o src/tesseract.o src/more.o src/config.o src/gpuprogram.o src/noise.o src/world.o src/project.o

mc4d: src/shaders.h $(FILES)
	$(CXX) $(CXXFLAGS) $(FILES) -o mc4d $(LDFLAGS)

# Ensure that the shader headers have been built before the main program is compiled
# If we don't do this, then after cleaning the shaders won't exist and building will fail
src/shaders.h: src/vertShader.h src/fragShader.h src/geomShader.h

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

src/geomShader.h: src/geom.glsl
	echo "char geomGlsl[] = {" > src/geomShader.h
	xxd -i < src/geom.glsl >> src/geomShader.h
	echo ", 0x00\n};" >> src/geomShader.h
