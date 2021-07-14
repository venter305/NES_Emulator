color_emphasis_test: color_emphasis_test.o
	/opt/cc65/bin/ld65 color_emphasis_test.o -t nes -o color_emphasis_test.nes

color_emphasis_test.o: color_emphasis_test.s
	/opt/cc65/bin/ca65 color_emphasis_test.s -o color_emphasis_test.o

clean:
	rm -f color_emphasis_test.o color_emphasis_test.nes
