WPhast.msi : WPhast.wixobj ../Release/WPhast.exe
	light -nologo WPhast.wixobj

WPhast.wixobj : WPhast.wxs
	candle -nologo WPhast.wxs
	
install :
	msiexec.exe /i WPhast.msi /qn+ INSTALLLOCATION=C:\DOCUME~1\charlton\PROGRA~1\WPhast /log WPhast.log
	
uninstall :
	msiexec.exe /x WPhast.msi /qn+

check : WPhast.msi
	/cygdrive/c/Program\ Files/MsiVal2/msival2 WPhast.msi 'C:\Program Files\MsiVal2\logo.cub' | egrep ERROR
	