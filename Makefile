EXE=rgstereoplayer
CSRC=$(wildcard src/*.c)
INCLUDE_DIR=3rd_party/iup/include
LIB_DIR=3rd_paty/iup
LIBS=-lm -lGL -lGLU -lXm -L3rd_party/iup -liup -lim \
     -liupimglib -liupim -liupgl 

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
	rm src/*.o ${EXE} *.log

check:
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(LIB_DIR)
	./${EXE}
