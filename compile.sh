rm -f ./out/*.o
rm -f ./out/*.exe
gcc -g -c event_loop.c -o ./out/el.o
gcc -g -c el_demo.c -o ./out/demo.o
gcc -g ./out/demo.o ./out/el.o -o ./out/demo.exe -lpthread