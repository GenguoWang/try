testMod.so:testMod.c
	gcc -g -Wall --shared -fPIC -I /home/kingo/torch/install/include/ -L /home/kingo/torch/install/lib/ -o testMod.so testMod.c -lluajit -lm
clean:
	rm -rf *.so *.o
