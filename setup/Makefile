CANDLE = candle
CANDLE_FLAGS=\
	-nologo \
	-dWPHAST_AUTOMATION \
	-dMergeDir=$(MERGE_DIR) \

LIGHT = light
LIGHT_FLAGS=\
	-nologo

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
	
WPhast.msi : WPhast.wixobj ../AutoRelease/WPhast.exe $(BINARIES)
	$(LIGHT) $(LIGHT_FLAGS) WPhast.wixobj

WPhast.wixobj : WPhast.wxs
	$(CANDLE) $(CANDLE_FLAGS) -out $@ WPhast.wxs
	
install :
	$(MSIEXEC) /i WPhast.msi /qn+ INSTALLLOCATION=C:\DOCUME~1\charlton\PROGRA~1\WPhast /log WPhast.log
	
uninstall :
	$(MSIEXEC) /x WPhast.msi /qn+

check : WPhast.msi
	$(MSIVAL2) WPhast.msi 'C:\Program Files\MsiVal2\logo.cub' -f
