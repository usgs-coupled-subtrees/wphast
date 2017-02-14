# use gnuwin32 wget NOT alias (wget -> Invoke-WebRequest)
While (Test-Path Alias:wget) {
  Remove-Item Alias:wget
}

# export msi/phast/RELEASE
${config_dir}="C:\Users\jenkins\svn-jenkins"
svn --config-dir ${config_dir} export --force `
    "http://internalbrr.cr.usgs.gov/svn_GW/phast3/trunk/RELEASE@${REL}" "msi/phast/RELEASE"

# set date
if ([string]::IsNullOrEmpty($Env:DATE)) {
  $Env:DATE = date +%x
}
Write-Output "0 LastExitCode=$LastExitCode"
${Env:RELEASE_DATE} = date -d ${Env:DATE} "+%B %e, %G"

# set ver
${PPK}="${Env:PUBLIC}\rsa-key-20151119.ppk"
if ([string]::IsNullOrEmpty($Env:VER)) {
  $SRC_FILE=(plink -i ${PPK} charlton@parkplace `
            "cd ftp/p4w; ls -t phast4windows-*-*.exe | awk '{if (NR == 1) {print}}'")
  $v = ($SRC_FILE -replace "^phast4windows-", "" -replace "-.*exe$", "") -split "\."
  if ([string]::IsNullOrEmpty($v[2])) {
    $v[2] = 0
  }
  $v[2] = 1 + $v[2]
  $Env:ver_major = $v[0]
  $Env:ver_minor = $v[1]
  $Env:ver_patch = $v[2]
  $Env:VER = $v -join "."
}
else {
  $v = ($Env:VER) -split "\."  
  $Env:ver_major = $v[0]
  $Env:ver_minor = $v[1]
  $Env:ver_patch = $v[2]
}
Write-Output "1 LastExitCode=$LastExitCode"
# set HEAD
[string]$HEAD=(-split (svn --config-dir C:\Users\jenkins\svn-jenkins st -v WPhast.sln))[0]
if ([string]::IsNullOrEmpty($Env:REL) -or $Env:REL.CompareTo('HEAD') -eq 0) {
  $Env:REL = $HEAD
}

${Env:VER_TAG}="r$Env:REL"
${Env:VER_NUMTAG}="-$Env:REL"
${Env:VERSION_LONG}="$Env:ver_major.$Env:ver_minor.$Env:ver_patch.$Env:REL"
${Env:VER_UC}="$Env:ver_major.$Env:ver_minor.$Env:ver_patch.$Env:REL"
${Env:MSI_VERSION}="$Env:ver_major.$Env:ver_minor.$Env:REL"
${Env:FULLPKG}="$Env:NAME-$Env:VER-$Env:REL"
${Env:DOWNLOADS}="${Env:PUBLIC}\Downloads"

Write-Output "HEAD=$HEAD"
Write-Output "Env:DATE=$Env:DATE"
Write-Output "Env:RELEASE_DATE=$Env:RELEASE_DATE"
Write-Output "Env:ver_major=$Env:ver_major"
Write-Output "Env:ver_major=$Env:ver_minor"
Write-Output "Env:ver_major=$Env:ver_patch"
Write-Output "Env:VER=$Env:VER"
Write-Output "Env:REL=$Env:REL"
Write-Output "Env:VER_TAG=$Env:VER_TAG"
Write-Output "Env:VER_NUMTAG=$Env:VER_NUMTAG"
Write-Output "Env:VERSION_LONG=$Env:VERSION_LONG"
Write-Output "Env:MSI_VERSION=$Env:MSI_VERSION"
Write-Output "Env:FULLPKG=$Env:FULLPKG"
Write-Output "Env:DOWNLOADS=${Env:DOWNLOADS}"

Write-Output "2 LastExitCode=$LastExitCode"

# create phast-dist-windows build URL
[string]$trigger = 'http://136.177.112.8:8081/job/phast-dist-windows/buildWithParameters'
$trigger += '?DATE='
$trigger += ${Env:DATE} -replace '/','%2f'
$trigger += '&REL='
$trigger += ${Env:REL}
$trigger += '&VER='
$trigger += ${Env:VER}
$trigger += '&delay=0sec'
Write-Output "trigger=$trigger"

# trigger build
PASS8081="c798ee5a2259a5ae001f90dd8772f6c3"
AUTH="--auth-no-challenge --http-user=charlton --http-password=${PASS8081}"
wget ${AUTH} -S ${trigger} -O start.html 2> queue.out
[string]$location="$((-Split (cat .\queue.out | Select-String "Location"))[1])api/xml"
Write-Output "location=$location"

# wget until <waitingItem> changes to <leftItem>
do {
  Start-Sleep -s 2
  wget ${AUTH} $location -O leftItem.xml 2> $null
} until ((Select-Xml -Path .\leftItem.xml -XPath "/leftItem"))

[string]$build="$((Select-Xml -Path .\leftItem.xml -XPath "/leftItem/executable/url").Node.InnerText)"
if ([string]::IsNullOrEmpty($build)) {
  throw "*** phast-dist-windows cannot be built ***`n"
}
Write-Output "build=$build"

# wget until <freeStyleBuild><building>false</building></freeStyleBuild>
do {
  Start-Sleep -s 20
  wget ${AUTH} "${build}api/xml" -O freeStyleBuild.xml 2> $null
  [string]$building = (Select-Xml -Path .\freeStyleBuild.xml -XPath "/freeStyleBuild/building").Node.InnerText
  Write-Output "building=$building"
} until($building -contains 'false')

[string]$url=(Select-Xml -Path .\freeStyleBuild.xml -XPath "/freeStyleBuild/url").Node.InnerText
$artifacts=(Select-Xml -Path .\freeStyleBuild.xml -XPath "/freeStyleBuild/artifact")

foreach ($art in $artifacts) {
  $path=Split-Path ${art}.Node.relativePath
  if (!(Test-Path -Path "${path}" -PathType Container)) {
    New-Item -ItemType directory -Path "${path}"
  }
  Push-Location ${path}
  ${relPath}=${art}.Node.relativePath
  wget ${AUTH} "${url}artifact/${relPath}" 2> $null
  Pop-Location 
}

# duplicate build/dist.sh
$sed_files=@('src/Version.h', `
             'msi/phast/RELEASE', `
             'src/WPhast.rc')

foreach ($file in $sed_files) {
  (Get-Content $file) | Foreach-Object {
    $_ -replace "(#define *VER_MAJOR\s*)[0-9]*",        "`${1}$Env:ver_major" `
       -replace "(#define *VER_MINOR\s*)[0-9]*",        "`${1}$Env:ver_minor" `
       -replace "(#define *VER_PATCH\s*)[0-9]*",        "`${1}$Env:ver_patch" `
       -replace "(#define *VER_REVISION\s*)[0-9]*",     "`${1}$Env:REL" `
       -replace "@RELEASE_DATE@",                       "$Env:RELEASE_DATE" `
       -replace "@VERSION_LONG@",                       "$Env:VERSION_LONG" `
       -replace "@VER_DATE@",                           "$Env:RELEASE_DATE" `
       -replace "@VERSION@",                            "$Env:VER" `
  } | Set-Content $file
}

Write-Output "3 LastExitCode=$LastExitCode"
# build P4W.exe/spawnc.exe
[string]${MsBuild}="c:\WINDOWS\Microsoft.NET\Framework\v2.0.50727\MsBuild.exe"
[string]$opts="/p:Configuration=AutoRelease /p:Platform=x64"
Invoke-Expression "$MsBuild $opts"

Write-Output "4 LastExitCode=$LastExitCode"
$LastExitCode=0

# create .p4w examples
${examples_dir}=".\msi\phast\examples"

${examples}=@("${examples_dir}\diffusion1d\diffusion1d.trans.dat", `
              "${examples_dir}\diffusion2d\diffusion2d.trans.dat", `
              "${examples_dir}\disp2d\disp2d.trans.dat", `
              "${examples_dir}\ex1\ex1.trans.dat", `
              "${examples_dir}\ex2\ex2.trans.dat", `
              "${examples_dir}\ex3\ex3.trans.dat", `
              "${examples_dir}\ex4\ex4.trans.dat", `
              "${examples_dir}\ex4_ddl\ex4_ddl.trans.dat", `
              "${examples_dir}\ex4_noedl\ex4_noedl.trans.dat", `
              "${examples_dir}\ex4restart\ex4restart.trans.dat", `
              "${examples_dir}\ex4_start_time\ex4_start_time.trans.dat", `
              "${examples_dir}\ex4_transient\ex4_transient.trans.dat", `
              "${examples_dir}\ex5\ex5.trans.dat", `
              "${examples_dir}\ex6\ex6.trans.dat", `
              "${examples_dir}\kindred4.4\kindred4.4.trans.dat", `
              "${examples_dir}\leaky\leaky.trans.dat", `
              "${examples_dir}\leakysurface\leakysurface.trans.dat", `
              "${examples_dir}\leakyx\leakyx.trans.dat", `
              "${examples_dir}\leakyz\leakyz.trans.dat", `
              "${examples_dir}\linear_bc\linear_bc.trans.dat", `
              "${examples_dir}\linear_ic\linear_ic.trans.dat", `
              "${examples_dir}\phrqex11\phrqex11.trans.dat", `
              "${examples_dir}\property\property.trans.dat", `
              "${examples_dir}\radial\radial.trans.dat", `
              "${examples_dir}\river\river.trans.dat", `
              "${examples_dir}\shell\shell.trans.dat", `
              "${examples_dir}\simple\simple.trans.dat", `
              "${examples_dir}\unconf\unconf.trans.dat", `
              "${examples_dir}\well\well.trans.dat", `
              "${examples_dir}\zf\zf.trans.dat")

foreach ($ex in $examples) {
  Write-Output "importing $ex"
  cscript /nologo .\msi\import.vbs $ex
}

# build msi
[string]${msi_opts}=".\msi\msi.sln /t:msi /p:Configuration=Release /p:Platform=x64 /p:Major=${Env:ver_major} /p:Minor=${Env:ver_minor} /p:Patch=${Env:ver_patch} /p:Build=${Env:REL}"
Invoke-Expression "${MsBuild} ${msi_opts}"

# build bootstrap
[string]$boot_opts=".\Bootstrapper\Bootstrapper.sln /t:Bootstrapper /p:Configuration=Release /p:Platform=x64 /p:TargetName=${Env:FULLPKG}-x64 /p:Major=${Env:ver_major} /p:Minor=${Env:ver_minor} /p:Patch=${Env:ver_patch} /p:Build=${Env:REL} /verbosity:detailed"
Invoke-Expression "${MsBuild} ${boot_opts}"
