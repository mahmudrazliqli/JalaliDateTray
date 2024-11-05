APPVER= 1.1
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
	rm -rf jalalidate-$(APPVER).deb jalalidate-$(APPVER)
	mkdir -p jalalidate-$(APPVER)/usr/share/applications/
	mkdir -p jalalidate-$(APPVER)/usr/share/icons/hicolor/48x48/apps
	#mkdir -p jalalidate-$(APPVER)/usr/share/autostart
	mkdir -p jalalidate-$(APPVER)/etc/xdg/autostart
	mkdir -p jalalidate-$(APPVER)/usr/bin
	mkdir -p jalalidate-$(APPVER)/DEBIAN
	cp ./deb/control 						jalalidate-$(APPVER)/DEBIAN/
	cp ./main 								jalalidate-$(APPVER)/usr/bin/jalalidate
	cp ./deb/jalalidate.desktop 			jalalidate-$(APPVER)/usr/share/applications/
	cp ./deb/jalalidate.png 				jalalidate-$(APPVER)/usr/share/icons/hicolor/48x48/apps/
	#cp ./deb/jalalidate-autostart.desktop 	jalalidate-$(APPVER)/usr/share/autostart/
	cp ./deb/jalalidate-autostart.desktop 	jalalidate-$(APPVER)/etc/xdg/autostart
	chmod 755 jalalidate-$(APPVER)/DEBIAN
	chmod 644 jalalidate-$(APPVER)/usr/share/applications/jalalidate.desktop
	chmod 644 jalalidate-$(APPVER)/etc/xdg/autostart/jalalidate-autostart.desktop
	#chmod 644 jalalidate-$(APPVER)/usr/share/autostart/jalalidate-autostart.desktop
	dpkg-deb --build jalalidate-$(APPVER)
	rm -rf jalalidate-$(APPVER)

install:deb	
	sudo dpkg -i jalalidate-$(APPVER).deb
	
uninstall:
	sudo dpkg -r jalalidate
#in msys2
# pacman -S mingw-w64-x86_64-libconfig
