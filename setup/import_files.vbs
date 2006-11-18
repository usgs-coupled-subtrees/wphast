' VBScript source code

scriptdir = Mid(WScript.ScriptFullName, 1, Len(Wscript.ScriptFullName) - Len(Wscript.ScriptName ))

' set up registry
'
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

clsid = "{36353903-2137-43FD-9AD6-40B65A96A839}"
classes = "HKEY_CURRENT_USER\Software\Classes\"
doc = "Testing.WPhast.Document"
exepath = scriptdir & "..\src\AutoRelease\WPhast.exe"

Set Sh = CreateObject("WScript.Shell")

' [HKEY_CURRENT_USER\Software\Classes\Testing.WPhast.Document]
' @="Testing.WPhast.Document"
'
Sh.RegWrite classes & doc & "\", doc

' [HKEY_CURRENT_USER\Software\Classes\Testing.WPhast.Document\CLSID]
' @="{36353903-2137-43FD-9AD6-40B65A96A839}"
'
Sh.RegWrite classes & doc & "\CLSID\", clsid

' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}]
' @="Testing.WPhast.Document"
'
Sh.RegWrite classes & "\CLSID\" & clsid & "\", doc

' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\LocalServer32]
' @="<scriptdir>..\AutoRelease\WPhast.exe
'
Sh.RegWrite classes & "\CLSID\" & clsid & "\LocalServer32\", exepath

' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\ProgID]
' @="Testing.WPhast.Document"
'
Sh.RegWrite classes & "\CLSID\" & clsid & "\ProgID\", doc

' 
'
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")

' If visible takes ~5x longer
' visible = true

delay = 0 ' milliseconds

examples = scriptdir & "phast\examples\"


' decay
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "decay\decay.trans.dat")
WPhast.SaveAs(examples & "decay\decay.wphast")
WScript.Sleep(delay)
'''WPhast.Run
'''WScript.Quit

' disp2d
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "disp2d\disp2d.trans.dat")
WPhast.SaveAs(examples & "disp2d\disp2d.wphast")
WScript.Sleep(delay)

' ex1
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "ex1\ex1.trans.dat")
WPhast.SaveAs(examples & "ex1\ex1.wphast")
WScript.Sleep(delay)

' ex2
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "ex2\ex2.trans.dat")
WPhast.SaveAs(examples & "ex2\ex2.wphast")
WScript.Sleep(delay)

' ex3
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "ex3\ex3.trans.dat")
WPhast.SaveAs(examples & "ex3\ex3.wphast")
WScript.Sleep(delay)

' ex4
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "ex4\ex4.trans.dat")
WPhast.SaveAs(examples & "ex4\ex4.wphast")
WScript.Sleep(delay)

' free
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "free\free.trans.dat")
WPhast.SaveAs(examples & "free\free.wphast")
WScript.Sleep(delay)

' kindred4.4
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "kindred4.4\kindred4.4.trans.dat")
WPhast.SaveAs(examples & "kindred4.4\kindred4.4.wphast")
WScript.Sleep(delay)

' leaky
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "leaky\leaky.trans.dat")
WPhast.SaveAs(examples & "leaky\leaky.wphast")
WScript.Sleep(delay)

' leakyx
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "leakyx\leakyx.trans.dat")
WPhast.SaveAs(examples & "leakyx\leakyx.wphast")
WScript.Sleep(delay)

' leakyz
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "leakyz\leakyz.trans.dat")
WPhast.SaveAs(examples & "leakyz\leakyz.wphast")
WScript.Sleep(delay)

' linear_bc
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "linear_bc\linear_bc.trans.dat")
WPhast.SaveAs(examples & "linear_bc\linear_bc.wphast")
WScript.Sleep(delay)

' linear_ic
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "linear_ic\linear_ic.trans.dat")
WPhast.SaveAs(examples & "linear_ic\linear_ic.wphast")
WScript.Sleep(delay)

' phrqex11
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "phrqex11\phrqex11.trans.dat")
WPhast.SaveAs(examples & "phrqex11\phrqex11.wphast")
WScript.Sleep(delay)

' radial
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
'''WPhast.Import(examples & "radial\radial.trans.dat")
'''WPhast.SaveAs(examples & "radial\radial.wphast")
WScript.Sleep(delay)

' river
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "river\river.trans.dat")
WPhast.SaveAs(examples & "river\river.wphast")
WScript.Sleep(delay)

' unconf
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "unconf\unconf.trans.dat")
WPhast.SaveAs(examples & "unconf\unconf.wphast")
WScript.Sleep(delay)

' well
WPhast = Null
Set WPhast = WScript.CreateObject("Testing.WPhast.Document")
WPhast.Visible = visible
WPhast.Import(examples & "well\well.trans.dat")
WPhast.SaveAs(examples & "well\well.wphast")
WScript.Sleep(delay)


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

' [-HKEY_CURRENT_USER\Software\Classes\Testing.WPhast.Document\CLSID]
'
Sh.RegDelete classes & doc & "\CLSID\"

' [-HKEY_CURRENT_USER\Software\Classes\Testing.WPhast.Document]
'
Sh.RegDelete classes & doc & "\"
