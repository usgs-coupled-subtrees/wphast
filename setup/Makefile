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
	$(EXAMPLES)/ex3/decay.wphast\
	$(EXAMPLES)/ex3/ex3.wphast\
	$(EXAMPLES)/ex4/ex4.wphast\
	$(EXAMPLES)/ex4/test.wphast\
	$(EXAMPLES)/free/free.wphast\
	$(EXAMPLES)/kindred4.4/kindred4.4.wphast\
	$(EXAMPLES)/leaky/leaky.wphast\
	$(EXAMPLES)/leakyx/leakyx.wphast\
	$(EXAMPLES)/leakyz/leakyz.wphast\
	$(EXAMPLES)/linear_bc/linear_bc.wphast\
	$(EXAMPLES)/linear_ic/linear_ic.wphast\
	$(EXAMPLES)/phrqex11/phrqex11.wphast\
	$(EXAMPLES)/radial/radial.wphast\
	$(EXAMPLES)/river/river.wphast\
	$(EXAMPLES)/unconf/unconf.wphast\
	$(EXAMPLES)/well/well.wphast
	
WPhast.msi : WPhast.wixobj ../AutoRelease/WPhast.exe $(BINARIES) $(WPHAST_FILES)
	$(LIGHT) $(LIGHT_FLAGS) WPhast.wixobj

WPhast.wixobj : WPhast.wxs
	$(CANDLE) $(CANDLE_FLAGS) -out $@ WPhast.wxs
	
install :
	$(MSIEXEC) /i WPhast.msi /qn+ INSTALLLOCATION=C:\DOCUME~1\charlton\PROGRA~1\WPhast /log WPhast.log
	
uninstall :
	$(MSIEXEC) /x WPhast.msi /qn+

check : WPhast.msi
	$(MSIVAL2) WPhast.msi 'C:\Program Files\MsiVal2\logo.cub' -f

import : ../AutoRelease/WPhast.exe
	$(CSCRIPT) /NOLOGO import_files.vbs
