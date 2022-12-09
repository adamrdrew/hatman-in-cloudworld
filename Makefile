ifdef OS
   RM = del /Q $1
else
   ifeq ($(shell uname), Linux)
      RM = rm -f $1
   endif
endif


clean:
	$(RM) *.o *.nes *.dbg

build: clean
	ca65 .\main.s -o main.o -g
	ld65 main.o -o hatman_in_cloudworld.nes -C nes.cfg	--dbgfile hatman_in_cloudworld.dbg

run-fceux: build
	fceux.exe hatman_in_cloudworld.nes 		

run-messen: build
	messen.exe hatman_in_cloudworld.nes 		
