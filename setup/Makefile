# $Id$

DEVENV := /cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ 8/Common7/IDE/devenv

SLN := ../WPhast.sln

WPHAST_EXE := ../AutoRelease/WPhast.exe

VTK_DLL_DIR := ../AutoRelease
HDF_DLL_DIR := ../AutoRelease

CANDLE = candle
CANDLE_FLAGS=\
	-nologo \
	-dWPHAST_AUTOMATION \
	-dMergeDir=$(MERGE_DIR) \
	-dVS2005

LIGHT = light
LIGHT_FLAGS=\
	-nologo

CSCRIPT = cscript

MERGE_DIR := "C:\Program Files\Common Files\Merge Modules"

MSIVAL2 := /cygdrive/c/Program\ Files/MsiVal2/msival2

MSIEXEC := msiexec

PHAST := $(PHAST_EXE_PATH)

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
	
EXAMPLES = phast/examples
	
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
	
VTK_DLLS=\
	$(VTK_DLL_DIR)/vtkCommon.dll\
	$(VTK_DLL_DIR)/vtkexpat.dll\
	$(VTK_DLL_DIR)/vtkFiltering.dll\
	$(VTK_DLL_DIR)/vtkfreetype.dll\
	$(VTK_DLL_DIR)/vtkftgl.dll\
	$(VTK_DLL_DIR)/vtkGraphics.dll\
	$(VTK_DLL_DIR)/vtkHybrid.dll\
	$(VTK_DLL_DIR)/vtkImaging.dll\
	$(VTK_DLL_DIR)/vtkIO.dll\
	$(VTK_DLL_DIR)/vtkjpeg.dll\
	$(VTK_DLL_DIR)/vtkParallel.dll\
	$(VTK_DLL_DIR)/vtkpng.dll\
	$(VTK_DLL_DIR)/vtkRendering.dll\
	$(VTK_DLL_DIR)/vtktiff.dll\
	$(VTK_DLL_DIR)/vtkzlib.dll
	
HDF_DLLS=\
	$(HDF_DLL_DIR)/hdf5dll.dll\
	$(HDF_DLL_DIR)/zlib.dll


all : WPhast.msi

WPhast.msi : WPhast.wixobj $(WPHAST_EXE) $(BINARIES) $(WPHAST_FILES) $(PHAST)
	$(LIGHT) $(LIGHT_FLAGS) WPhast.wixobj

WPhast.wixobj : WPhast.wxs Version.wxs
	$(CANDLE) $(CANDLE_FLAGS) -out $@ WPhast.wxs
	
$(WPHAST_EXE) : $(SLN)
	@rm -rf AutoRelease.log
	$(DEVENV) /out AutoRelease.log /build AutoRelease $(SLN)
	@cat AutoRelease.log
	
%.wphast : %.trans.dat $(WPHAST_EXE) $(HDF_DLLS) $(VTK_DLLS)
	$(CSCRIPT) /NOLOGO import.vbs $<	

install :
	$(MSIEXEC) /i WPhast.msi /qn+ INSTALLLOCATION=C:\DOCUME~1\charlton\PROGRA~1\WPhast /log WPhast.log
	
uninstall :
	$(MSIEXEC) /x WPhast.msi /qn+

check : WPhast.msi
	$(MSIVAL2) WPhast.msi 'C:\Program Files\MsiVal2\logo.cub' -f

import : $(WPHAST_EXE) $(HDF_DLLS) $(VTK_DLLS)
	$(CSCRIPT) /NOLOGO import_files.vbs

import_clean :
	rm -rf $(WPHAST_FILES)

.SECONDARY : $(VTK_DLLS) $(HDF_DLLS)

$(VTK_DLL_DIR)% :: $(DEV_VTK42_VS7_DLL_LIB)/%
	install -m 755 $^ $@

$(HDF_DLL_DIR)% :: $(DEV_HDF5_LIBDLL)/%
	install -m 755 $^ $@

dll_clean :
	rm -rf $(HDF_DLLS) $(VTK_DLLS)
