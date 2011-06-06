EXE=anaglyphsim
CSRC=$(wildcard *.c)
#INCLUDE_DIR=
#LIB_DIR=
LIBS=-lm -lGL -lGLU -lglut

CPPFLAGS=-g -I$(INCLUDE_DIR)
LDFLAGS=-L$(LIB_DIR) $(LIBS)

all: $(EXE)

$(EXE): $(CSRC:.c=.o)
	@echo Criando arquivo executável: $@
	@g++ -o $@ $^ $(LDFLAGS)
		
%.o: %.c
	@echo Compilando arquivo objeto: $@
	@g++ -c $< $(CPPFLAGS) -o $@
clean:
	rm src/*.o imgfilters *.log

check:
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(LIB_DIR)
	./all
