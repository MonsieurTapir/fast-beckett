CC=g++
CFLAGS= -march=native -O3 -std=c++11
DIR_OBJ=obj/
DIR_SRCS=src/
EXEC=beckett
OBJS=main.o search.o heuristics.o
DIR_OBJS=$(addprefix $(DIR_OBJ), $(OBJS))
all: $(EXEC)

beckett: $(DIR_OBJS)
	$(CC) -o bin/$@ $^ $(CFLAGS)

$(DIR_OBJ)%.o: $(DIR_SRCS)%.cc
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(DIR_OBJ)*.o
