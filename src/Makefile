DEVENV = /cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ .NET/Common7/IDE/devenv
SLN = WPhast.sln

all : AutoRelease AutoDebug cppunit

.PHONY : AutoRelease
AutoRelease : $(SLN)
	@rm -f AutoRelease/AutoRelease.log
	$(DEVENV) /out AutoRelease/AutoRelease.log /build AutoRelease $(SLN)
	@cat AutoRelease/AutoRelease.log

.PHONY : AutoDebug
AutoDebug :
	@rm -f AutoDebug/AutoDebug.log
	$(DEVENV) /out AutoDebug/AutoDebug.log  /build AutoDebug $(SLN)
	@cat AutoDebug/AutoDebug.log

.PHONY : cppunit
cppunit : $(SLN)
	@rm -f cppunit/cppunit.log
	$(DEVENV) /out cppunit/cppunit.log /build cppunit $(SLN)
	@cat cppunit/cppunit.log
