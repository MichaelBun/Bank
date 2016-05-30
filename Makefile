CC = gcc
CXXFLAGS = -g -Wall
CXXLINK = $(CC) -std=c99
LIBS = -lpthread -lm
OBJS = atm.o bank.o  main.o 
RM = rm -f

bank: $(OBJS)
	$(CXXLINK) -o bank $(OBJS) $(LIBS)  

atm.o: atm.c atm.h bank.h
bank.o: bank.c bank.h
main.o: main.c bank.h atm.h

clean:
	$(RM) bank *.o *.bak *~ "#"* core
	$(RM) log.txt
