

OBJS = strategy.o blobwar.o main.o font.o mouse.o image.o widget.o rollover.o button.o label.o board.o rules.o blob.o network.o bidiarray.o shmem.o

OBJS_launchComputation = launchStrategy.o strategy.o bidiarray.o shmem.o
#OBJS_testPerf = testPerf.o strategy.o bidiarray.o shmem.o


LIBS = -lSDL_image -lSDL_ttf -lm `sdl-config --libs` -lSDL_net -lpthread -L ../../../SDL/lib

CFLAGS = -Wall -Werror -O3 -g `sdl-config --cflags`   -Wno-strict-aliasing -DDEBUG -I ../../../SDL/include/SDL
CC = g++ -fopenmp

# $(sort) remove duplicate object
OBJS_ALL = $(sort $(OBJS) $(OBJS_launchComputation))

blobwar: $(OBJS) launchStrategy
	$(CC) $(OBJS) $(CFLAGS) -o blobwar $(LIBS)
$(OBJS_ALL):	%.o:	%.cc common.h
	$(CC) -c $<  $(CFLAGS)
launchStrategy: $(OBJS_launchComputation)
	$(CC) $(OBJS_launchComputation) $(CFLAGS) -fopenmp -o launchStrategy $(LIBS) -pg -lm
#testPerf: $(OBJS_testPerf)
#	$(CC) $(OBJS_testPerf) $(CFLAGS) -o testPerf $(LIBS) -pg

clean:
	rm -f *.o core blobwar launchStrategy *~
