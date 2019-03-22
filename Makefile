C = gcc
CFLAGS = -g
TARGET1 = master
TARGET2 = palin
OBJS1 = master.o
OBJS2 = palin.o

all: $(TARGET1) $(TARGET2)

master: $(OBJS1)
	$(CC) $(CFLAGS) -o master master.o -lpthread

palin: $(OBJS2)
	$(CC) $(CFLAGS) -o palin palin.o -lpthread

clean:
	/bin/rm -f *.o $(TARGET)
	/bin/rm -f master $(TARGET)
	/bin/rm -f palin $(TARGET)
	/bin/rm -f *.out $(TARGET)
