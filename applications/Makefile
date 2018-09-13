
all:
	cd createEDP; make
	cd createEVENT; make
	cd createSAM; make
	cd performSIMULATION; make
	cd performUQ; make

debug:
	cd createEDP; make debug
	cd createEVENT; make debug
	cd createSAM; make debug
	cd performSIMULATION; make debug
	cd performUQ; make debug

clean:
	cd createEVENT; make clean
	cd createSAM; make clean
	cd createEDP; make clean
	cd performSIMULATION; make clean
	cd performUQ; make clean
	$(RM) $(OBJS) *~ \#*

distclean: clean
	cd createEVENT; make distclean
	cd createSAM; make distclean
	cd createEDP; make distclean
	cd performSIMULATION; make distclean
	cd performUQ; make distclean


