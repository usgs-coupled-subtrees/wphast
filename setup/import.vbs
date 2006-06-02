' VBScript source code

On Error resume next

If WScript.Arguments.Count <> 1 Then
	WScript.Echo "Usage import.vbs xxx.trans.dat"
	WScript.Quit 1
End If

curdir = Mid(WScript.ScriptFullName, 1, Len(Wscript.ScriptFullName) - Len(Wscript.ScriptName ))

' set up registry
'
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

clsid = "{36353903-2137-43FD-9AD6-40B65A96A839}"
classes = "HKEY_CURRENT_USER\Software\Classes\"
doc = "WPhast.Document"
exepath = curdir & "..\AutoRelease\WPhast.exe"

Set Sh = CreateObject("WScript.Shell")

bCreateDoc = false
Sh.RegRead(classes & doc & "\CLSID\")
If Err <> 0 Then
  bCreateDoc = true
End if

If bCreateDoc Then
	' [HKEY_CURRENT_USER\Software\Classes\WPhast.Document]
	' @="WPhast.Document"
	'
	Sh.RegWrite classes & doc & "\", doc
End If

bCreateDocCLSID = false
Sh.RegRead(classes & doc & "\CLSID\")
If Err <> 0 Then
  bCreateDocCLSID = true
End if

If bCreateDocCLSID Then
	' [HKEY_CURRENT_USER\Software\Classes\WPhast.Document\CLSID]
	' @="{36353903-2137-43FD-9AD6-40B65A96A839}"
	'
	Sh.RegWrite classes & doc & "\CLSID\", clsid
End If


bCreateClassCLSID = false
Sh.RegRead(classes & "\CLSID\" & clsid & "\")
If Err <> 0 Then
  bCreateClassCLSID = true
End if
If bCreateClassCLSID Then
	' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}]
	' @="WPhast.Document"
	'
	Sh.RegWrite classes & "\CLSID\" & clsid & "\", doc
End If


bCreateLocalServer32 = false
Sh.RegRead(classes & "\CLSID\" & clsid & "\LocalServer32\")
If Err <> 0 Then
  bCreateLocalServer32 = true
End if
If bCreateLocalServer32 Then
	' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\LocalServer32]
	' @="<curdir>..\AutoRelease\WPhast.exe
	'
	Sh.RegWrite classes & "\CLSID\" & clsid & "\LocalServer32\", exepath
End If


bCreateProgID = false
Sh.RegRead(classes & "\CLSID\" & clsid & "\ProgID\")
If Err <> 0 Then
  bCreateProgID = true
End if
If bCreateProgID Then
	' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\ProgID]
	' @="WPhast.Document"
	'
	Sh.RegWrite classes & "\CLSID\" & clsid & "\ProgID\", doc
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

If bCreateProgID Then
	' [-HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\ProgID]
	'
	Sh.RegDelete classes & "\CLSID\" & clsid & "\ProgID\"
End If

If bCreateLocalServer32 Then
	' [-HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\LocalServer32]
	'
	Sh.RegDelete classes & "\CLSID\" & clsid & "\LocalServer32\"
End If


If bCreateClassCLSID Then
	' [-HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}]
	'
	Sh.RegDelete classes & "\CLSID\" & clsid & "\"
End If

If bCreateDocCLSID Then
	' [-HKEY_CURRENT_USER\Software\Classes\WPhast.Document\CLSID]
	'
	Sh.RegDelete classes & doc & "\CLSID\"
End If

If bCreateDoc Then
	' [-HKEY_CURRENT_USER\Software\Classes\WPhast.Document]
	'
	Sh.RegDelete classes & doc & "\"
End If
