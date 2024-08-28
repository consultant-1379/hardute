CC=/usr/sfw/bin/gcc
CFLAGS=-g -Wall
OBJS=getstat.o hardute.o readfile.o statcpu.o statcrontab.o statdisk.o statmem.o statmnt.o statnic.o statproc.o statsystem.o

pkg: hardute hardute.sh expire.sh pkginfo prototype
	echo $(PWD)/src
	rm -Rf pkg
	mkdir pkg
	mkdir -p bin/hardute/bin
	cp hardute hardute.sh hardute-sar-00.sh hardute-sar-15.sh hardute-sar-30.sh hardute-sar-45.sh expire.sh bin/hardute/bin
	pkgmk -d $(PWD)/src/pkg -o -b / -r $(PWD)/src/bin
	pkgtrans $(PWD)/src/pkg $(PWD)/hardute-$(BUILD) hardute

clean:
	rm -Rf *.o hardute pkg bin /var/spool/pkg/hardute

hardute: $(OBJS)
	$(CC) $(CFLAGS) -o hardute $(OBJS)
	
%.o: %.c %.h platform.h
	$(CC) $(CFLAGS) -c $<

