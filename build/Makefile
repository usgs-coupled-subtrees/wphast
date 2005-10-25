# $Id$

NAME := wphast

FORCE_UPDATE := $(shell svn up ..)

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

all : check_release $(BIN_PACKAGE)

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
	
check_release :
	@if [ "$(VERSION)"x = "x" ]; \
	  then \
		echo "usage: make VERSION=<number> DATE=<xx/xx/xxxx> PHAST_BUILD=<filename> [RELEASE=<number>] [SIG=<0|1>]"; \
		echo "    ie make VERSION=0.1 DATE=10/24/2005 PHAST_BUILD=phast-1.2-577.tar.bz2"; \
		echo "       make VERSION=0.1 DATE=10/24/2005 PHAST_BUILD=phast-1.2-577.tar.bz2 RELEASE=620 SIG=1"; \
		exit 1; \
	    else \
		exit 0; \
	fi
	@if [ "$(PHAST_BUILD)"x = "x" ]; \
	  then \
		echo "usage: make VERSION=<number> DATE=<xx/xx/xxxx> PHAST_BUILD=<filename> [RELEASE=<number>] [SIG=<0|1>]"; \
		echo "    ie make VERSION=0.1 DATE=10/24/2005 PHAST_BUILD=phast-1.2-577.tar.bz2"; \
		echo "       make VERSION=0.1 DATE=10/24/2005 PHAST_BUILD=phast-1.2-577.tar.bz2 RELEASE=620 SIG=1"; \
		exit 1; \
	    else \
		exit 0; \
	fi
	@if [ "$(RELEASE)"x = "x" ]; \
	  then \
		echo "usage: make VERSION=<number> DATE=<xx/xx/xxxx> PHAST_BUILD=<filename> [RELEASE=<number>] [SIG=<0|1>]"; \
		echo "    ie make VERSION=0.1 DATE=10/24/2005 PHAST_BUILD=phast-1.2-577.tar.bz2"; \
		echo "       make VERSION=0.1 DATE=10/24/2005 PHAST_BUILD=phast-1.2-577.tar.bz2 RELEASE=620 SIG=1"; \
		exit 1; \
	    else \
		exit 0; \
	fi
	@if [ "$(DATE)"x = "x" ]; \
	  then \
		echo "usage: make VERSION=<number> DATE=<xx/xx/xxxx> PHAST_BUILD=<filename> [RELEASE=<number>] [SIG=<0|1>]"; \
		echo "    ie make VERSION=0.1 DATE=10/24/2005 PHAST_BUILD=phast-1.2-577.tar.bz2"; \
		echo "       make VERSION=0.1 DATE=10/24/2005 PHAST_BUILD=phast-1.2-577.tar.bz2 RELEASE=620 SIG=1"; \
		exit 1; \
	    else \
		exit 0; \
	fi
