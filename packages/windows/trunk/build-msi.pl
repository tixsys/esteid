#! /usr/bin/perl -w

$wixDir = "$ENV{'WIX'}\\bin";
$candle = "\"$wixDir\\candle.exe\"";
$light = "\"$wixDir\\light.exe\"";
$lit = "\"$wixDir\\lit.exe\"";
$inf2cat = "C:/WinDDK/7600.16385.1/bin/selfsign/inf2cat.exe";

$wixUI = "WixUI_IDCard";
@wixModules = (
	$wixUI,
	"RemoveOldSoftDlg",
	"MyInstallDirDlg",
	"commonlibs",
	"esteid-browser-plugin",
	"csp",
	"libdigidoc",
	"opensc",
	"qdigidoc",
	"qesteidutil",
	"qt",
	"updater"
);
@certs = ("certs", "testcerts");
@drivers = ("minidriver");
$tempFiles = "*.wixlib *.wixobj *.wixpdb minidriver";
$outDir = "C:/build/msi";

$_ = `svn info http://esteid.googlecode.com/svn`;
if ( /Revision: (.*)/ ) {
	$riaSvnRev = $1;
} else {
	exit 1;
}

$riaSvnRev=~ s/[^1-9]*//g;

$msiVer = "2.8.$riaSvnRev";
#$msiVer = "2.8.0";
$ENV{'MSI_VERSION'} = "$msiVer.0";
$ENV{'MergeModules'} = "C:\\Program Files (x86)\\Common Files\\Merge Modules";

$prefix32 = "C:/esteid/nightly/win32/build/install";
$ENV{'PREFIX32'} = "$prefix32";
$ENV{'LIBDIR32'} = "$prefix32/lib";
$ENV{'BINDIR32'} = "$prefix32/bin";

$prefix64 = "C:/esteid/nightly/x64/build/install";
$ENV{'PREFIX64'} = "$prefix64";
$ENV{'LIBDIR64'} = "$prefix64/lib";
$ENV{'BINDIR64'} = "$prefix64/bin";

$platform = $ENV{'MSI_PLATFORM'};
if ($platform eq "x64") {
	$msiName = "Eesti_ID_kaart_x64";
	$msiDir = "$outDir/$msiVer" . "_x64";
} else {
	$msiName = "Eesti_ID_kaart";
	$msiDir = "$outDir/$msiVer";
}

if (-e $msiDir) {
	print "ERROR: $msiDir already exists.\n";
	exit 1;
}

system "rm -rf $tempFiles $msiName.msi";

system( "mkdir -p minidriver" ) == 0
	or die "Error running mkdir";
system( "cp -f $prefix32/bin/esteidcm.dll minidriver" ) == 0
	or die "Error running cp";
system( "cp -f $prefix32/bin/esteidcm.inf minidriver" ) == 0
	or die "Error running cp";

if ($platform eq "x64") {
	system( "cp -f $prefix64/bin/esteidcm64.dll minidriver" ) == 0
		or die "Error running cp";
	system( "cp -f $prefix64/bin/esteidcm.inf minidriver" ) == 0
		or die "Error running cp";
}

if ($platform eq "x64") {
	$infOS = "7_X64";
} else {
	$infOS = "7_X86";
}

system("$inf2cat /driver:minidriver /os:$infOS") == 0
	or die "Error running inf2cat";

@files = <minidriver/*.dll minidriver/*.cat>;
foreach my $f (@files) {
	$cmd = "signtool.exe sign -a -t http://timestamp.verisign.com/scripts/timstamp.dll -v $f";
	system( $cmd ) == 0
		or die "Error signing $f";
}

foreach my $f (@wixModules) {
	system( "$candle $f.wxs -dPlatform=$platform -ext WixUtilExtension" ) == 0
		or die "Error running $candle";
	system( "$lit -out $f.wixlib $f.wixobj -loc IDCard_en-us.wxl -ext WixUtilExtension" ) == 0
		or die "Error running $lit";
} 

foreach my $f (@certs) {
	system( "$candle $f.wxs -ext WixIIsExtension" ) == 0
		or die "Error running $candle";
	system( "$lit -out $f.wixlib $f.wixobj -ext WixIIsExtension" ) == 0
		or die "Error running $lit";
} 

foreach my $f (@drivers) {
	system( "$candle $f.wxs -dPlatform=$platform -ext WixDifxAppExtension" ) == 0
		or die "Error running $candle";
	system( "$lit -out $f.wixlib $f.wixobj -ext WixDifxAppExtension" ) == 0
		or die "Error running $lit";
} 

system( "$candle Eesti_ID_kaart.wxs -dPlatform=$platform -ext WixUtilExtension -ext WiXNetFxExtension" ) == 0
	or die "Error running $candle";

$silenceVCRedist = "-sice:ICE03 -sice:ICE82 -sice:ICE83 -sw1055";
$difxapp = "\"$wixDir/difxapp_$platform.wixlib\"";
$cmd = "$light -out $msiName.msi Eesti_ID_kaart.wixobj -loc IDCard_en-us.wxl -ext WixUtilExtension -ext WixUIExtension -ext WixIIsExtension -ext WixDifxAppExtension -ext WiXNetFxExtension -sice:ICE60 $silenceVCRedist $difxapp";
foreach my $f (@wixModules, @certs, @drivers) {
	$cmd .= " $f.wixlib";
}
system $cmd;

unless (-e "$msiName.msi") {
	print "ERROR: Couldn't create $msiName.msi\n";
	exit 1;
}

$cmd = "signtool.exe sign -a -t http://timestamp.verisign.com/scripts/timstamp.dll -v -d $msiName.msi $msiName.msi";
system( $cmd ) == 0
	or die "Error signing msi";

system( "cscript query.wsf" ) == 0
	or die "Error running query.wsf";

system "mkdir -p $msiDir/";
system "mv $msiName.msi $msiDir/";
system "mv products.xml $msiDir/";

system "rm -rf $tempFiles";
