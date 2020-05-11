IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR) -Wl

ODIR=obj
LDIR =../lib

LIBS=../lib/libMPSSE.a ../lib/libftd2xx.lib -ldl

_DEPS = ftd2xx.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

TARGET = run_test4


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sample-static: $(OBJ)
	#$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	gcc -o main main.c ../lib/ftd2xx.lib ../lib/libMPSSE.a -I../include #-ldl
	#echo 'sudo rmmod ftdi_sio \nsudo rmmod usbserial \nsudo ./main' > $(TARGET)
	#sudo chmod +x $(TARGET)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ main $(TARGET)