

all: usb.x

usb.x: bulk_usb_reader.c
	gcc -g -o $@ $^ -lusb-1.0

clean:
	rm -rf *.x
