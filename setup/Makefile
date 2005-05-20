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
	
WPhast.msi : WPhast.wixobj ../Release/WPhast.exe $(BINARIES)
	light -nologo WPhast.wixobj

WPhast.wixobj : WPhast.wxs
	candle -nologo WPhast.wxs
	
install :
	msiexec.exe /i WPhast.msi /qn+ INSTALLLOCATION=C:\DOCUME~1\charlton\PROGRA~1\WPhast /log WPhast.log
	
uninstall :
	msiexec.exe /x WPhast.msi /qn+

check : WPhast.msi
	/cygdrive/c/Program\ Files/MsiVal2/msival2 WPhast.msi 'C:\Program Files\MsiVal2\logo.cub' | egrep "ERROR|WARNING"
	