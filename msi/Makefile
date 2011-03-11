# $Id: Makefile 4914 2010-10-28 01:30:53Z charlton $

MSI := bin/Release/phast4windows.msi

DEVENV := devenv

SLN := ../WPhast.sln

WPHAST_EXE := ../src/AutoRelease/WPhast.exe

VTK_DLL_DIR := ../src/AutoRelease
HDF_DLL_DIR := ../src/AutoRelease

###CANDLE = candle
###CANDLE_FLAGS=\
###	-nologo \
###	-dWPHAST_AUTOMATION \
###	-dVS2005

###LIGHT = light
###LIGHT_FLAGS=\
###	-nologo

CSCRIPT = cscript

MSIVAL2 := /cygdrive/c/Program\ Files/MsiVal2/msival2

MSIEXEC := msiexec

PHAST := $(PHAST_EXE_PATH)

DEV_VTK42_VS8_DLL_BIN_CYG = /cygdrive/c/PROGRA~1/VTK-4-~1/bin

DEV_HDF5_LIBDLL_CYG = /cygdrive/c/3RDPARTY/HDF5/5-143-~1/c/release/dll


%.wphast : %.trans.dat $(WPHAST_EXE) $(HDF_DLLS) $(VTK_DLLS)
	$(CSCRIPT) /NOLOGO import.vbs $<


EXAMPLES = phast/examples
	
WPHAST_FILES=\
	$(EXAMPLES)/diffusion1d/diffusion1d.wphast\
	$(EXAMPLES)/diffusion2d/diffusion2d.wphast\
	$(EXAMPLES)/disp2d/disp2d.wphast\
	$(EXAMPLES)/ex1/ex1.wphast\
	$(EXAMPLES)/ex2/ex2.wphast\
	$(EXAMPLES)/ex3/ex3.wphast\
	$(EXAMPLES)/ex4/ex4.wphast\
	$(EXAMPLES)/ex4_ddl/ex4_ddl.wphast\
	$(EXAMPLES)/ex4_noedl/ex4_noedl.wphast\
	$(EXAMPLES)/ex4restart/ex4restart.wphast\
	$(EXAMPLES)/ex4_start_time/ex4_start_time.wphast\
	$(EXAMPLES)/ex4_transient/ex4_transient.wphast\
	$(EXAMPLES)/ex5/ex5.wphast\
	$(EXAMPLES)/ex6/ex6.wphast\
	$(EXAMPLES)/kindred4.4/kindred4.4.wphast\
	$(EXAMPLES)/leaky/leaky.wphast\
	$(EXAMPLES)/leakysurface/leakysurface.wphast\
	$(EXAMPLES)/leakyx/leakyx.wphast\
	$(EXAMPLES)/leakyz/leakyz.wphast\
	$(EXAMPLES)/linear_bc/linear_bc.wphast\
	$(EXAMPLES)/linear_ic/linear_ic.wphast\
	$(EXAMPLES)/phrqex11/phrqex11.wphast\
	$(EXAMPLES)/river/river.wphast\
	$(EXAMPLES)/shell/shell.wphast\
	$(EXAMPLES)/simple/simple.wphast\
	$(EXAMPLES)/unconf/unconf.wphast\
	$(EXAMPLES)/well/well.wphast\
	$(EXAMPLES)/zf/zf.wphast
	
###	$(EXAMPLES)/radial/radial.wphast\
###	$(EXAMPLES)/property/property.wphast\
	
VTK_DLLS=\
	$(VTK_DLL_DIR)/vtkCommon.dll\
	$(VTK_DLL_DIR)/vtkDICOMParser.dll\
	$(VTK_DLL_DIR)/vtkexoIIc.dll\
	$(VTK_DLL_DIR)/vtkexpat.dll\
	$(VTK_DLL_DIR)/vtkFiltering.dll\
	$(VTK_DLL_DIR)/vtkfreetype.dll\
	$(VTK_DLL_DIR)/vtkftgl.dll\
	$(VTK_DLL_DIR)/vtkGraphics.dll\
	$(VTK_DLL_DIR)/vtkHybrid.dll\
	$(VTK_DLL_DIR)/vtkImaging.dll\
	$(VTK_DLL_DIR)/vtkIO.dll\
	$(VTK_DLL_DIR)/vtkjpeg.dll\
	$(VTK_DLL_DIR)/vtkmetaio.dll\
	$(VTK_DLL_DIR)/vtkMFC.dll\
	$(VTK_DLL_DIR)/vtkNetCDF.dll\
	$(VTK_DLL_DIR)/vtkParallel.dll\
	$(VTK_DLL_DIR)/vtkpng.dll\
	$(VTK_DLL_DIR)/vtkRendering.dll\
	$(VTK_DLL_DIR)/vtksys.dll\
	$(VTK_DLL_DIR)/vtktiff.dll\
	$(VTK_DLL_DIR)/vtkverdict.dll\
	$(VTK_DLL_DIR)/vtkWidgets.dll\
	$(VTK_DLL_DIR)/vtkzlib.dll

	
HDF_DLLS=\
	$(HDF_DLL_DIR)/hdf5dll.dll\
	$(HDF_DLL_DIR)/szlibdll.dll\
	$(HDF_DLL_DIR)/zlib.dll


all : $(MSI)

$(MSI) : msi.sln  $(WPHAST_EXE) $(WPHAST_FILES)
	MSBuild.exe msi.sln /t:msi /p:Configuration=Release /p:Platform=x86 /p:TargetName=${FULLPKG} /p:Major=${MAJOR} /p:Minor=${MINOR} /p:Patch=${PATCH} /p:Build=${REL}
	touch $(MSI)

$(WPHAST_EXE) : $(SLN)
	@rm -rf AutoRelease.log
	$(DEVENV) /out AutoRelease.log /build AutoRelease $(SLN)
	@cat AutoRelease.log
	
install :
	$(MSIEXEC) /i `cygpath -w $(MSI)` /qn+ APPLICATIONFOLDER=C:\\DOCUME~1\\charlton\\PROGRA~1\\WPhast /log WPhast.log
	
uninstall :
	$(MSIEXEC) /x `cygpath -w $(MSI)`

check : $(MSI)
	$(MSIVAL2) $(MSI) 'C:\Program Files\MsiVal2\logo.cub' -f

import : $(WPHAST_EXE) $(HDF_DLLS) $(VTK_DLLS)
	$(CSCRIPT) /NOLOGO import_files.vbs

import_clean :
	rm -rf $(WPHAST_FILES)

.SECONDARY : $(VTK_DLLS) $(HDF_DLLS)

$(VTK_DLL_DIR)/%.dll :: $(DEV_VTK42_VS8_DLL_BIN_CYG)/%.dll
	install -m 755 $^ $@

$(HDF_DLL_DIR)/%.dll :: $(DEV_HDF5_LIBDLL_CYG)/%.dll
	install -m 755 $^ $@

dlls : $(HDF_DLLS) $(VTK_DLLS)

dll_clean :
	rm -rf $(HDF_DLLS) $(VTK_DLLS)
