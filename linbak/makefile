CC=g++ -c -std=c++0x -I/usr/local/boost_1_74_0
GCC=g++ -olinbak  -Wall 
all: linbak

debug: CC += -g
debug: GCC += -g
debug: linbak

objfiles = main.o sync.o mainloop.o utcpsock.o GetFiles.o DirExistsOrMake.o util.o GetDiff.o doupdates.o sequence.o addtovectors.o requestupdates.o initialise.o startfileupdate.o endfileupdate.o buffer.o readfailed.o ReadDir.o pathmake.o log.o readconfig.o

linbak : $(objfiles)
	$(GCC) $(objfiles)

main.o:	main.cpp sync.h
	$(CC) main.cpp

sync.o: sync.cpp sync.h
	$(CC) sync.cpp

mainloop.o : mainloop.cpp utcpsock.h sync.h 
	$(CC) mainloop.cpp
utcpsock.o : utcpsock.cpp utcpsock.h util.h
	$(CC) utcpsock.cpp

GetFiles.o: GetFiles.cpp
	$(CC) GetFiles.cpp

DirExistsOrMake.o: DirExistsOrMake.cpp util.h sync.h
	$(CC) DirExistsOrMake.cpp

util.o: util.cpp util.h
	$(CC) util.cpp

GetDiff.o: GetDiff.cpp sync.h
	$(CC) GetDiff.cpp

doupdates.o: doupdates.cpp sync.h
	$(CC) doupdates.cpp

sequence.o: sequence.cpp sync.h
	$(CC) sequence.cpp

addtovectors.o: addtovectors.cpp sync.h
	$(CC) addtovectors.cpp

requestupdates.o: requestupdates.cpp sync.h
	$(CC) requestupdates.cpp

initialise.o: initialise.cpp sync.h
	$(CC) initialise.cpp

endfileupdate.o: endfileupdate.cpp sync.h
	$(CC) endfileupdate.cpp

startfileupdate.o: startfileupdate.cpp sync.h
	$(CC) startfileupdate.cpp

#SyncRead.o: SyncRead.cpp sync.h
#	$(CC) SyncRead.cpp

buffer.o: buffer.cpp buffer.h
	$(CC) buffer.cpp

readfailed.o: readfailed.cpp sync.h
	$(CC) readfailed.cpp

ReadDir.o: ReadDir.cpp sync.h
	$(CC) ReadDir.cpp

pathmake.o: pathmake.cpp sync.h
	$(CC) pathmake.cpp

log.o: log.cpp sync.h
	$(CC) log.cpp

readconfig.o: readconfig.cpp sync.h
	$(CC) readconfig.cpp

clean : FORCE
	rm linbak $(objfiles)

FORCE:
