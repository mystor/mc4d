CXX=g++
LDFLAGS=$(shell pkg-config --static --libs glew glfw3) -lpng
CPPFLAGS=$(shell pkg-config --cflags glew glfw3) -g
CXXFLAGS=-Wall -O0

FILES=src/main.o src/tesseract.o src/config.o src/gpuprogram.o src/noise.o src/world.o src/roundworld.o src/project.o src/gl.o src/readpng.o

mc4d: src/shaders.h $(FILES)
	$(CXX) $(CXXFLAGS) $(FILES) -o mc4d $(LDFLAGS)

# Ensure that the shader headers have been built before the main program is compiled
# If we don't do this, then after cleaning the shaders won't exist and building will fail
src/shaders.h: src/vertShader.h src/fragShader.h src/geomShader.h src/blendvertShader.h src/blendfragShader.h src/wirevertShader.h src/wirefragShader.h src/sbvertShader.h src/sbfragShader.h


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

src/blendvertShader.h: src/blendvert.glsl
	echo "char blendvertGlsl[] = {" > src/blendvertShader.h
	xxd -i < src/blendvert.glsl >> src/blendvertShader.h
	echo ", 0x00\n};" >> src/blendvertShader.h

src/blendfragShader.h: src/blendfrag.glsl
	echo "char blendfragGlsl[] = {" > src/blendfragShader.h
	xxd -i < src/blendfrag.glsl >> src/blendfragShader.h
	echo ", 0x00\n};" >> src/blendfragShader.h

src/wirevertShader.h: src/wirevert.glsl
	echo "char wirevertGlsl[] = {" > src/wirevertShader.h
	xxd -i < src/wirevert.glsl >> src/wirevertShader.h
	echo ", 0x00\n};" >> src/wirevertShader.h

src/wirefragShader.h: src/wirefrag.glsl
	echo "char wirefragGlsl[] = {" > src/wirefragShader.h
	xxd -i < src/wirefrag.glsl >> src/wirefragShader.h
	echo ", 0x00\n};" >> src/wirefragShader.h

src/sbvertShader.h: src/sbvert.glsl
	echo "char sbvertGlsl[] = {" > src/sbvertShader.h
	xxd -i < src/sbvert.glsl >> src/sbvertShader.h
	echo ", 0x00\n};" >> src/sbvertShader.h

src/sbfragShader.h: src/sbfrag.glsl
	echo "char sbfragGlsl[] = {" > src/sbfragShader.h
	xxd -i < src/sbfrag.glsl >> src/sbfragShader.h
	echo ", 0x00\n};" >> src/sbfragShader.h
