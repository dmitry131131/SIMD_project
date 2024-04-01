CXX = g++
CXXFLAGS =  -O3 -march=core-avx2

#-O3 -march=core-avx2

TARGET = simd
SourcePrefix = src/
BuildPrefix = build/
BuildFolder = build
Include = -Iinclude

Sources = simd.cpp FPS.cpp image.cpp
Main = main.cpp

Libs = -lsfml-graphics -lsfml-window -lsfml-system

Source = $(addprefix $(SourcePrefix), $(Sources))
MainObject = $(patsubst %.cpp, $(BuildPrefix)%.o, $(Main))

objects = $(patsubst $(SourcePrefix)%.cpp, $(BuildPrefix)%.o, $(Source))

.PHONY : all clean folder release debug

all : prepare folder $(TARGET)

test : CXXFLAGS = -O3 -march=core-avx2 -DCOMPARE_MODE
test : all

cat : CXXFLAGS = -O3 -march=core-avx2 -DCAT
cat : all

prepare: 
	
$(BuildPrefix)%.o : $(SourcePrefix)%.cpp
	@echo [CXX] -c $< -o $@
	@$(CXX) $(CXXFLAGS) $(Include) -c $< -o $@

$(TARGET) : $(objects) $(MainObject)
	@echo [CC] $^ -o $@
	@$(CXX) $(CXXFLAGS) $(Include) $^ $(Libs) -o $@

clean :
	rm $(BuildFolder)/*.o
	rm $(TARGET)

folder :
	mkdir -p $(BuildFolder)