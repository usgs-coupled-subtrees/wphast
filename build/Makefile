# $Id$

NAME := wphast

RELEASE := $(shell svn st -v dist.sh | cut -b 10- | ( read one two ; echo $$one ) )

DATE_STAMP := $(shell echo $(DATE) | sed -e "y^/^-^" )

TARBALL := $(NAME)-$(VERSION)-$(RELEASE).tar.gz

PHAST_TARBALL := phast-$(PHAST_VERSION)-$(RELEASE).tar.gz

DIST_MSI := $(NAME)-$(VERSION)-$(RELEASE).msi

BUILD_SCRIPT := $(NAME)-$(VERSION)-$(RELEASE).sh

DEVENV := /cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ .NET/Common7/IDE/devenv

SLN := ../WPhast.sln

# (ie wphast-0.1-584-src.tar.bz2)
SRC_PACKAGE := $(NAME)-$(VERSION)-$(RELEASE)-src.tar.bz2

# (ie wphast-0.1-584.tar.bz2)
BIN_PACKAGE := $(NAME)-$(VERSION)-$(RELEASE).tar.bz2

CANDLE = candle
CANDLE_FLAGS=\
	-nologo \
	-dWPHAST_AUTOMATION \
	-dMergeDir=$(MERGE_DIR) \

LIGHT = light
LIGHT_FLAGS=\
	-nologo

CSCRIPT = cscript

MERGE_DIR="C:\Program Files\Common Files\Merge Modules"

MSIVAL2=/cygdrive/c/Program\ Files/MsiVal2/msival2

MSIEXEC=msiexec

###PHAST=/home/charlton/programs/phast/srcphast/win32/ser/phast.exe
###PHAST_BUILD=phast-1.2-577.tar.bz2

BINARIES=\
	Binary/Banner.bmp\
	Binary/Banner.bmp\
	Binary/Complete.ico\
	Binary/Custom.ico\
	Binary/Dialog.bmp\
	Binary/Exclam.ico\
	Binary/Info.ico\
	Binary/License.rtf\
	Binary/New.ico\
	Binary/Remove.ico\
	Binary/Repair.ico\
	Binary/Typical.ico\
	Binary/Up.ico
	
EXAMPLES = tree/USGS/WPhast\ 0.0/examples
	
WPHAST_FILES=\
	$(EXAMPLES)/decay/decay.wphast\
	$(EXAMPLES)/disp2d/disp2d.wphast\
	$(EXAMPLES)/ex1/ex1.wphast\
	$(EXAMPLES)/ex2/ex2.wphast\
	$(EXAMPLES)/ex3/ex3.wphast\
	$(EXAMPLES)/ex4/ex4.wphast\
	$(EXAMPLES)/free/free.wphast\
	$(EXAMPLES)/kindred4.4/kindred4.4.wphast\
	$(EXAMPLES)/leaky/leaky.wphast\
	$(EXAMPLES)/leakyx/leakyx.wphast\
	$(EXAMPLES)/leakyz/leakyz.wphast\
	$(EXAMPLES)/linear_bc/linear_bc.wphast\
	$(EXAMPLES)/linear_ic/linear_ic.wphast\
	$(EXAMPLES)/phrqex11/phrqex11.wphast\
	$(EXAMPLES)/river/river.wphast\
	$(EXAMPLES)/unconf/unconf.wphast\
	$(EXAMPLES)/well/well.wphast
	
all : check_release $(BIN_PACKAGE)
##all : check_release $(TARBALL) $(BUILD_SCRIPT) $(PHAST_BUILD)
###all : WPhast.msi

$(BIN_PACKAGE) : $(TARBALL) $(BUILD_SCRIPT)
	@echo "Building package $(BIN_PACKAGE) / $(SRC_PACKAGE)"	
	time $(BUILD_SCRIPT) all 2>&1 | tee build-$(RELEASE).log

$(TARBALL) : dist.sh $(DATE_STAMP).tstamp
	@echo "Creating $(TARBALL)"
	time ./dist.sh -v $(VERSION) -r $(RELEASE) -d $(DATE) -pr trunk 2>&1 | tee dist-$(RELEASE).log
	
$(BUILD_SCRIPT) : build.sh
	rm -rf  $(BUILD_SCRIPT)
	ln build.sh $(BUILD_SCRIPT)

$(DATE_STAMP).tstamp : 
	rm -rf *.tstamp
	touch $(DATE_STAMP).tstamp
	
msi : $(DIST_MSI) build_msi_file
	
$(DIST_MSI) : $(TARBALL) $(PHAST_TARBALL) $(BUILD_SCRIPT)
	touch $(DIST_MSI)
	
build_msi_file :  $(BUILD_SCRIPT) $(TARBALL) $(PHAST_TARBALL)
	time $(BUILD_SCRIPT) all 2>&1 | tee build-$(RELEASE).log

check_release :
	@if [ "$(VERSION)"x = "x" ]; \
	  then \
		echo "usage: make VERSION=<number> DATE=<xx/xx/xxxx> PHAST_BUILD=<filename> [RELEASE=<number>] [SIG=<0|1>]"; \
		echo "    ie make VERSION=0.1 DATE=10/6/2005 PHAST_BUILD=phast-1.2-577.tar.bz2"; \
		echo "       make VERSION=0.1 DATE=10/6/2005 PHAST_BUILD=phast-1.2-577.tar.bz2 RELEASE=584 SIG=1"; \
		exit 1; \
	    else \
		exit 0; \
	fi
	@if [ "$(PHAST_BUILD)"x = "x" ]; \
	  then \
		echo "usage: make VERSION=<number> DATE=<xx/xx/xxxx> PHAST_BUILD=<filename> [RELEASE=<number>]"; \
		echo "    ie make VERSION=0.1 DATE=10/6/2005 PHAST_BUILD=phast-1.2-577.tar.bz2"; \
		echo "       make VERSION=0.1 DATE=10/6/2005 PHAST_BUILD=phast-1.2-577.tar.bz2 RELEASE=584"; \
		exit 1; \
	    else \
		exit 0; \
	fi
	@if [ "$(RELEASE)"x = "x" ]; \
	  then \
		echo "usage: make VERSION=<number> DATE=<xx/xx/xxxx> PHAST_BUILD=<filename> [RELEASE=<number>]"; \
		echo "    ie make VERSION=0.1 DATE=10/6/2005 PHAST_BUILD=phast-1.2-577.tar.bz2"; \
		echo "       make VERSION=0.1 DATE=10/6/2005 PHAST_BUILD=phast-1.2-577.tar.bz2 RELEASE=584"; \
		exit 1; \
	    else \
		exit 0; \
	fi
	@if [ "$(DATE)"x = "x" ]; \
	  then \
		echo "usage: make VERSION=<number> DATE=<xx/xx/xxxx> PHAST_BUILD=<filename> [RELEASE=<number>]"; \
		echo "    ie make VERSION=0.1 DATE=10/6/2005 PHAST_BUILD=phast-1.2-577.tar.bz2"; \
		echo "       make VERSION=0.1 DATE=10/6/2005 PHAST_BUILD=phast-1.2-577.tar.bz2 RELEASE=584"; \
		exit 1; \
	    else \
		exit 0; \
	fi

	
WPhast.msi : WPhast.wixobj ../AutoRelease/WPhast.exe $(BINARIES) $(WPHAST_FILES) $(PHAST)
	$(LIGHT) $(LIGHT_FLAGS) WPhast.wixobj

WPhast.wixobj : WPhast.wxs Version.wxs
	$(CANDLE) $(CANDLE_FLAGS) -out $@ WPhast.wxs
	
../AutoRelease/WPhast.exe : $(SLN)
	@rm AutoRelease.log
	$(DEVENV) /out AutoRelease.log /build AutoRelease $(SLN)
	@cat AutoRelease.log

install :
	$(MSIEXEC) /i WPhast.msi /qn+ INSTALLLOCATION=C:\DOCUME~1\charlton\PROGRA~1\WPhast /log WPhast.log
	
uninstall :
	$(MSIEXEC) /x WPhast.msi /qn+

check : WPhast.msi
	$(MSIVAL2) WPhast.msi 'C:\Program Files\MsiVal2\logo.cub' -f

import : ../AutoRelease/WPhast.exe
	$(CSCRIPT) /NOLOGO import_files.vbs
