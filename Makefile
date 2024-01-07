INCLUDE = -I.

all: build_encoder build_decoder

build_encoder:
	gcc -O3 -o encoder_411086030.exe encoder_411086030.c -lm

build_decoder:
	gcc -O3 -o decoder_411086030.exe decoder_411086030.c -lm

demo0:
	./encoder_411086030 0 KimberlyNCat.bmp R.txt G.txt B.txt dim.txt
	./decoder_411086030 0 ResKimberlyNCat.bmp R.txt G.txt B.txt dim.txt

clean:
	rm -f encoder_411086030.exe decoder_411086030.exe R.txt G.txt B.txt dim.txt

.PHONY: build_encoder build_decoder demo0 clean
