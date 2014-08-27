# $Id$

MSI := bin/Release/phast4windows.msi

DEVENV := devenv

SLN := ../WPhast.sln

P4W_EXE := ../src/AutoRelease/P4W.exe

VTK_DLL_DIR := ../src/AutoRelease
HDF_DLL_DIR := ../src/AutoRelease

CSCRIPT = cscript

MSIVAL2 := /cygdrive/c/Program\ Files/MsiVal2/msival2

MSIEXEC := msiexec

PHAST := $(PHAST_EXE_PATH)

DEV_VTK42_VS8_DLL_BIN_CYG = /cygdrive/c/PROGRA~1/VTK-4-~1/bin

DEV_HDF5_LIBDLL_CYG = /cygdrive/c/3RDPARTY/HDF5/5-143-~1/c/release/dll


%.p4w : %.trans.dat $(P4W_EXE) $(HDF_DLLS) $(VTK_DLLS)
	$(CSCRIPT) /NOLOGO import.vbs $<


EXAMPLES = phast/examples
	
P4W_FILES=\
	$(EXAMPLES)/diffusion1d/diffusion1d.p4w\
	$(EXAMPLES)/diffusion2d/diffusion2d.p4w\
	$(EXAMPLES)/disp2d/disp2d.p4w\
	$(EXAMPLES)/ex1/ex1.p4w\
	$(EXAMPLES)/ex2/ex2.p4w\
	$(EXAMPLES)/ex3/ex3.p4w\
	$(EXAMPLES)/ex4/ex4.p4w\
	$(EXAMPLES)/ex4_ddl/ex4_ddl.p4w\
	$(EXAMPLES)/ex4_noedl/ex4_noedl.p4w\
	$(EXAMPLES)/ex4restart/ex4restart.p4w\
	$(EXAMPLES)/ex4_start_time/ex4_start_time.p4w\
	$(EXAMPLES)/ex4_transient/ex4_transient.p4w\
	$(EXAMPLES)/ex5/ex5.p4w\
	$(EXAMPLES)/ex6/ex6.p4w\
	$(EXAMPLES)/kindred4.4/kindred4.4.p4w\
	$(EXAMPLES)/leaky/leaky.p4w\
	$(EXAMPLES)/leakysurface/leakysurface.p4w\
	$(EXAMPLES)/leakyx/leakyx.p4w\
	$(EXAMPLES)/leakyz/leakyz.p4w\
	$(EXAMPLES)/linear_bc/linear_bc.p4w\
	$(EXAMPLES)/linear_ic/linear_ic.p4w\
	$(EXAMPLES)/phrqex11/phrqex11.p4w\
	$(EXAMPLES)/property/property.p4w\
	$(EXAMPLES)/radial/radial.p4w\
	$(EXAMPLES)/river/river.p4w\
	$(EXAMPLES)/shell/shell.p4w\
	$(EXAMPLES)/simple/simple.p4w\
	$(EXAMPLES)/unconf/unconf.p4w\
	$(EXAMPLES)/well/well.p4w\
	$(EXAMPLES)/zf/zf.p4w
	
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

$(MSI) : msi.sln  $(P4W) $(P4W_FILES)
	MSBuild.exe msi.sln /t:msi /p:Configuration=Release /p:Platform=x64 /p:TargetName=phast4windows /p:Major=${MAJOR} /p:Minor=${MINOR} /p:Patch=${PATCH} /p:Build=${REL} /p:phastSerExePath="${PHAST_SER_EXE_PATH}" /p:phastMPICH2ExePath="${PHAST_MPICH2_EXE_PATH}"
	touch $(MSI)

$(P4W_EXE) : $(SLN)
	@rm -rf AutoRelease.log
	$(DEVENV) /out AutoRelease.log /build AutoRelease $(SLN)
	@cat AutoRelease.log
	
install :
	$(MSIEXEC) /i `cygpath -w $(MSI)` /qn+ APPLICATIONFOLDER=C:\\DOCUME~1\\charlton\\PROGRA~1\\WPhast /log WPhast.log
	
uninstall :
	$(MSIEXEC) /x `cygpath -w $(MSI)`

check : $(MSI)
	$(MSIVAL2) $(MSI) 'C:\Program Files\MsiVal2\logo.cub' -f

import : $(P4W_EXE) $(HDF_DLLS) $(VTK_DLLS)
	$(CSCRIPT) /NOLOGO import_files.vbs

import_clean :
	rm -rf $(P4W_FILES)

.SECONDARY : $(VTK_DLLS) $(HDF_DLLS)

$(VTK_DLL_DIR)/%.dll :: $(DEV_VTK42_VS8_DLL_BIN_CYG)/%.dll
	install -m 755 $^ $@

$(HDF_DLL_DIR)/%.dll :: $(DEV_HDF5_LIBDLL_CYG)/%.dll
	install -m 755 $^ $@

dlls : $(HDF_DLLS) $(VTK_DLLS)

dll_clean :
	rm -rf $(HDF_DLLS) $(VTK_DLLS)
