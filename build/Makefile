# $Id$

#
# To run step by step
# 1. make source
# 2. make script
# 3. make build_all
# and step 3 may be broken using the following
# 3a. make prep
# 3b. make conf
# 3c. make build
# 3d. make install
# 3e. make pkg
# 3f. make spkg
# 3g. make finish
#

NAME := wphast

FORCE_UPDATE := $(shell svn up ..)

RELEASE ?= $(shell svn st -v dist.sh | cut -b 10- | ( read one two ; echo $$one ) )

DATE_STAMP := $(shell date -d $(DATE) "+%m-%d-%Y")

TARBALL := $(NAME)-$(VERSION)-$(RELEASE).tar.gz

BUILD_SCRIPT := $(NAME)-$(VERSION)-$(RELEASE).sh

DEVENV := /cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ 8/Common7/IDE/devenv

SLN := ../WPhast.sln

REPOS_PATH ?= trunk

# (ie wphast-0.1-584-src.tar.bz2)
SRC_PACKAGE := $(NAME)-$(VERSION)-$(RELEASE)-src.tar.bz2

# (ie wphast-0.1-584.tar.bz2)
BIN_PACKAGE := $(NAME)-$(VERSION)-$(RELEASE).tar.bz2

all : check_args $(BIN_PACKAGE)

build_all : check_args $(BIN_PACKAGE)

prep : check_args
	time ./$(BUILD_SCRIPT) prep 2>&1 | tee prep-$(RELEASE).log

conf : check_args
	time ./$(BUILD_SCRIPT) conf 2>&1 | tee conf-$(RELEASE).log

build : check_args
	time ./$(BUILD_SCRIPT) build 2>&1 | tee build-$(RELEASE).log

install : check_args
	time ./$(BUILD_SCRIPT) install 2>&1 | tee install-$(RELEASE).log

pkg : check_args
	time ./$(BUILD_SCRIPT) pkg 2>&1 | tee pkg-$(RELEASE).log

spkg : check_args
	time ./$(BUILD_SCRIPT) spkg 2>&1 | tee spkg-$(RELEASE).log

finish : check_args
	time ./$(BUILD_SCRIPT) finish 2>&1 | tee finish-$(RELEASE).log

$(BIN_PACKAGE) : $(TARBALL) $(BUILD_SCRIPT) $(PHAST_BUILD)
	@echo "Building package $(BIN_PACKAGE) / $(SRC_PACKAGE)"	
	time ./$(BUILD_SCRIPT) all 2>&1 | tee all-$(RELEASE).log
	
source : check_args $(TARBALL)
	@echo "created $(TARBALL)"

$(TARBALL) : dist.sh $(DATE_STAMP).tstamp
	@echo "Creating $(TARBALL)"
	time ./dist.sh -v $(VERSION) -r $(RELEASE) -d $(DATE) -pr $(REPOS_PATH) 2>&1 | tee dist-$(RELEASE).log
	
script : check_version $(BUILD_SCRIPT)	
	@echo "created $(BUILD_SCRIPT)"
	
$(BUILD_SCRIPT) : build.sh
	ln -f build.sh $(BUILD_SCRIPT)
	
$(DATE_STAMP).tstamp : 
	rm -rf *.tstamp
	touch $(DATE_STAMP).tstamp
	
check_args : check_version check_phast_build check_date
	
check_version :
ifeq ($(strip $(VERSION)),)
	@echo "No version specified"
	@make -s usage
	@exit 1
endif	

check_phast_build :
ifeq ($(strip $(PHAST_BUILD)),)
	@echo "No PHAST_BUILD specified"
	make usage
	exit 1
endif

check_release :
ifeq ($(strip $(DATE)),)
	@echo "No release specified"
	make usage
	exit 1
endif

check_date :
ifeq ($(strip $(DATE)),)
	@echo "No date specified"
	make usage
	exit 1
endif

usage:
	@echo ""
	@echo "usage: make VERSION=<number> DATE=<xx/xx/xxxx> PHAST_BUILD=<file> [RELEASE=<number>] [SIG=<0|1>]"
	@echo "    ie make VERSION=0.1 DATE=10/24/2005 PHAST_BUILD=phast-1.2-577.tar.bz2"
	@echo "       make VERSION=0.1 DATE=10/24/2005 PHAST_BUILD=phast-1.2-577.tar.bz2 RELEASE=620 SIG=1"
	@echo ""
