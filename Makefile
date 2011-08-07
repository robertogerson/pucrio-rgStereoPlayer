EXE1=rganaglyphplayer
EXE2=imganaglyph

CSRC1=$(wildcard src/*.c)
CSRC2=$(wildcard img_player/*.c)

INCLUDE_DIR=3rd_party/iup/include

LIB_DIR=3rd_paty/iup

LIBS=-lm -lGL -lGLU -lXm -L3rd_party/iup -liup -lim \
     -liupimglib -liupim -liupgl

CPPFLAGS=-g -I$(INCLUDE_DIR)
LDFLAGS=-L$(LIB_DIR) $(LIBS)

all: $(EXE1) $(EXE2)

$(EXE1): $(CSRC1:.c=.o)
	@echo Criando arquivo executável: $@
	@g++ -o $@ $^ $(LDFLAGS)

$(EXE2): $(CSRC2:.c=.o)
	@echo Criando arquivo executável: $@
	@g++ -o $@ $^ $(LDFLAGS)
		
%.o: %.c
	@echo Compilando arquivo objeto: $@
	@g++ -Wall -c $< $(CPPFLAGS) -o $@
clean:
	rm img_player/*.o ${EXE2}
	rm src/*.o ${EXE1} *.log

check:
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(LIB_DIR)
	./${EXE}
