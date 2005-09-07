' VBScript source code

Set WPhast = WScript.CreateObject("WPhast.Document")

' If visible takes ~5x longer
'''WPhast.Visible = true

delay = 0 ' milliseconds

examples = "C:\cygwin\home\charlton\programs\WPhast\setup\tree\USGS\WPhast 0.0\examples\"

' decay
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "decay\decay.trans.dat")
WPhast.SaveAs(examples & "decay\decay.wphast")
WScript.Sleep(delay)
'''WPhast.Run

' disp2d
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "disp2d\disp2d.trans.dat")
WPhast.SaveAs(examples & "disp2d\disp2d.wphast")
WScript.Sleep(delay)

' ex1
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "ex1\ex1.trans.dat")
WPhast.SaveAs(examples & "ex1\ex1.wphast")
WScript.Sleep(delay)

' ex2
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "ex2\ex2.trans.dat")
WPhast.SaveAs(examples & "ex2\ex2.wphast")
WScript.Sleep(delay)

' ex3
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "ex3\ex3.trans.dat")
WPhast.SaveAs(examples & "ex3\ex3.wphast")
WScript.Sleep(delay)

' ex4
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "ex4\ex4.trans.dat")
WPhast.SaveAs(examples & "ex4\ex4.wphast")
WScript.Sleep(delay)

' free
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "free\free.trans.dat")
WPhast.SaveAs(examples & "free\free.wphast")
WScript.Sleep(delay)

' kindred4.4
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "kindred4.4\kindred4.4.trans.dat")
WPhast.SaveAs(examples & "kindred4.4\kindred4.4.wphast")
WScript.Sleep(delay)

' leaky
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "leaky\leaky.trans.dat")
WPhast.SaveAs(examples & "leaky\leaky.wphast")
WScript.Sleep(delay)

' leakyx
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "leakyx\leakyx.trans.dat")
WPhast.SaveAs(examples & "leakyx\leakyx.wphast")
WScript.Sleep(delay)

' leakyz
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "leakyz\leakyz.trans.dat")
WPhast.SaveAs(examples & "leakyz\leakyz.wphast")
WScript.Sleep(delay)

' linear_bc
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "linear_bc\linear_bc.trans.dat")
WPhast.SaveAs(examples & "linear_bc\linear_bc.wphast")
WScript.Sleep(delay)

' linear_ic
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "linear_ic\linear_ic.trans.dat")
WPhast.SaveAs(examples & "linear_ic\linear_ic.wphast")
WScript.Sleep(delay)

' phrqex11
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "phrqex11\phrqex11.trans.dat")
WPhast.SaveAs(examples & "phrqex11\phrqex11.wphast")
WScript.Sleep(delay)

' radial
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
'''WPhast.Import(examples & "radial\radial.trans.dat")
'''WPhast.SaveAs(examples & "radial\radial.wphast")
WScript.Sleep(delay)

' river
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "river\river.trans.dat")
WPhast.SaveAs(examples & "river\river.wphast")
WScript.Sleep(delay)

' unconf
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "unconf\unconf.trans.dat")
WPhast.SaveAs(examples & "unconf\unconf.wphast")
WScript.Sleep(delay)

' well
WPhast = Null
Set WPhast = WScript.CreateObject("WPhast.Document")
WPhast.Import(examples & "well\well.trans.dat")
WPhast.SaveAs(examples & "well\well.wphast")
WScript.Sleep(delay)
