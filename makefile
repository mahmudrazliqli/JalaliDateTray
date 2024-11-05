all:clean
ifeq ($(OS), Windows_NT)
	gcc main.c  `pkg-config --cflags --libs gtk+-3.0`  -c -o main.o
	windres -I.  -i winicon/resource.rc  -o   winicon/resource.o
	gcc  main.o winicon/resource.o `pkg-config --cflags --libs gtk+-3.0` -mwindows  -lhid -lsetupapi    -Wl,--export-all-symbols -o  main
else ifeq ($(shell uname -s), Linux)
	gcc -Wall -Os -c -o main.o main.c `pkg-config --cflags --libs gtk+-3.0` 
	gcc -o main main.o   `pkg-config --cflags --libs gtk+-3.0`  -Wl,--export-dynamic 
	strip main
else ifeq ($(shell uname -s), Darwin)
	gcc -Wall -Os -c -o main.o main.c `pkg-config --cflags --libs gtk+-3.0` 
	gcc -o main main.o   `pkg-config --cflags --libs gtk+-3.0` -Wl,--export-dynamic 
	strip main
endif
	rm -rf *.o
	
clean:
	@echo + OS = $(OS) 
	@echo - shell uname= $(shell uname -s)
	rm -rf *.o winicon/*.o *.exe main 
	
program:all
	./main
	
#.deb file build
#sudo apt update
#sudo apt install build-essential debhelper dh-make libgtk-3-dev
deb:all
	rm -rf jalalidate-1.0.deb jalalidate-1.0
	mkdir -p jalalidate-1.0/usr/share/applications/
	mkdir -p jalalidate-1.0/usr/share/icons/hicolor/48x48/apps
	#mkdir -p jalalidate-1.0/usr/share/autostart
	mkdir -p jalalidate-1.0/etc/xdg/autostart
	mkdir -p jalalidate-1.0/usr/bin
	mkdir -p jalalidate-1.0/DEBIAN
	cp ./deb/control 						jalalidate-1.0/DEBIAN/
	cp ./main 								jalalidate-1.0/usr/bin/jalalidate
	cp ./deb/jalalidate.desktop 			jalalidate-1.0/usr/share/applications/
	cp ./deb/jalalidate.png 				jalalidate-1.0/usr/share/icons/hicolor/48x48/apps/
	#cp ./deb/jalalidate-autostart.desktop 	jalalidate-1.0/usr/share/autostart/
	cp ./deb/jalalidate-autostart.desktop 	jalalidate-1.0/etc/xdg/autostart
	chmod 755 jalalidate-1.0/DEBIAN
	chmod 644 jalalidate-1.0/usr/share/applications/jalalidate.desktop
	chmod 644 jalalidate-1.0/etc/xdg/autostart/jalalidate-autostart.desktop
	#chmod 644 jalalidate-1.0/usr/share/autostart/jalalidate-autostart.desktop
	dpkg-deb --build jalalidate-1.0
	rm -rf jalalidate-1.0

install:	
	sudo dpkg -i jalalidate-1.0.deb
	
uninstall:
	sudo dpkg -r jalalidate
#in msys2
# pacman -S mingw-w64-x86_64-libconfig