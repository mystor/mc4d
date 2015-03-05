CXX=g++
LDFLAGS=$(shell pkg-config --static --libs glew glfw3)
CPPFLAGS=$(shell pkg-config --cflags glew glfw3) -g

FILES=src/main.o src/tesseract.o src/controls.o src/more.o src/config.o src/gpuprogram.o src/noise.o

mc4d: $(FILES)
	$(CXX) $(CXXFLAGS) $(FILES) -o mc4d $(LDFLAGS)

.PHONY: clean
clean:
	rm src/*.o

.PHONY: run
run: mc4d
	./mc4d

src/vertShader.h: src/vert.glsl
	echo "char vertGlsl[] = {" > src/vertShader.h
	xxd -i < src/vert.glsl >> src/vertShader.h
	echo ", 0x00\n};" >> src/vertShader.h

src/fragShader.h: src/frag.glsl
	echo "char fragGlsl[] = {" > src/fragShader.h
	xxd -i < src/frag.glsl >> src/fragShader.h
	echo ", 0x00\n};" >> src/fragShader.h

src/main.o: src/tesseract.h src/controls.h src/config.h src/gpuprogram.h src/vertShader.h src/fragShader.h src/noise.h src/gl.h
src/tesseract.o: src/tesseract.h src/more.h src/config.h src/gl.h
src/controls.o: src/controls.h src/gl.h
src/more.o: src/more.h src/gl.h
src/config.o: src/config.h src/gl.h
src/gpuprogram.o: src/gpuprogram.h src/gl.h
src/noise.o: src/noise.h src/gl.h
