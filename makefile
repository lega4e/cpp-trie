cflags  := -std=gnu++17 -c -Wall
ldflags :=
libs    :=





# main
build: main

run: main
	main 


re: clean build

rerun: clean run


mainsrc_dirs    := src/test src
include_dirs    := src/sct src/tmpls
search_wildcard := $(wildcard $(addsuffix /*.cpp,$(mainsrc_dirs)))
mainobj_files   := $(patsubst %.cpp,%.o,$(search_wildcard))
mainobj_files   := $(notdir $(mainobj_files))
mainobj_files   := $(addprefix target/,$(mainobj_files))
VPATH           := $(mainsrc_dirs)

main: ./target $(mainobj_files)
	g++ $(ldflags) -o $@ $(mainobj_files) $(libs)

./target:
	if ! [ -d ./target ]; then mkdir target; fi

target/%.o: %.cpp
	g++ $(cflags) -o $@ -MD $(addprefix -I,$(include_dirs)) $<

include $(wildcard target/*.d)



# clean
clean:
	-rm target/*.o target/*.d main





# end
