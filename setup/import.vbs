' VBScript source code

curdir = Mid(WScript.ScriptFullName, 1, Len(Wscript.ScriptFullName) - Len(Wscript.ScriptName ))

' set up registry
'
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

clsid = "{36353903-2137-43FD-9AD6-40B65A96A839}"
classes = "HKEY_CURRENT_USER\Software\Classes\"
doc = "WPhast.Document"
exepath = curdir & "..\AutoRelease\WPhast.exe"

Set Sh = CreateObject("WScript.Shell")

' [HKEY_CURRENT_USER\Software\Classes\WPhast.Document]
' @="WPhast.Document"
'
Sh.RegWrite classes & doc & "\", doc

' [HKEY_CURRENT_USER\Software\Classes\WPhast.Document\CLSID]
' @="{36353903-2137-43FD-9AD6-40B65A96A839}"
'
Sh.RegWrite classes & doc & "\CLSID\", clsid

' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}]
' @="WPhast.Document"
'
Sh.RegWrite classes & "\CLSID\" & clsid & "\", doc

' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\LocalServer32]
' @="<curdir>..\AutoRelease\WPhast.exe
'
Sh.RegWrite classes & "\CLSID\" & clsid & "\LocalServer32\", exepath

' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\ProgID]
' @="WPhast.Document"
'
Sh.RegWrite classes & "\CLSID\" & clsid & "\ProgID\", doc

If WScript.Arguments.Count <> 1 Then
	WScript.Echo "Usage import.vbs xxx.trans.dat"
	WScript.Quit 1
End If

Set Fso = CreateObject("Scripting.FileSystemObject")
Set trans_dat_file = Fso.GetFile(WScript.Arguments(0))
wphast_file = Mid(trans_dat_file, 1, Len(trans_dat_file) - Len(".trans.dat")) & ".wphast"

Set WPhast = WScript.CreateObject("WPhast.Document")
If WPhast.Import(trans_dat_file.Path) Then
	WPhast.SaveAs(wphast_file)
Else
	WScript.Echo "An error occured during import: " & trans_dat_file.Path
End If

' clean up reg
'
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

' [-HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\LocalServer32]
'
Sh.RegDelete classes & "\CLSID\" & clsid & "\LocalServer32\"

' [-HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\ProgID]
'
Sh.RegDelete classes & "\CLSID\" & clsid & "\ProgID\"

' [-HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}]
'
Sh.RegDelete classes & "\CLSID\" & clsid & "\"

' [-HKEY_CURRENT_USER\Software\Classes\WPhast.Document\CLSID]
'
Sh.RegDelete classes & doc & "\CLSID\"

' [-HKEY_CURRENT_USER\Software\Classes\WPhast.Document]
'
Sh.RegDelete classes & doc & "\"
