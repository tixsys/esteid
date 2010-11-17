#!/bin/sh

# http://www.sk.ee/pages.php/0203040502,423

TEST="${TEST:-no}"

if [ x$TEST = x"yes" ] ; then
	OUTFILE=../testcerts.wxs
	GROUP=TestCertificates
else
	OUTFILE=../certs.wxs
	GROUP=SKCertificates
fi

TMP1=$(mktemp wixgen.XXXXXXXXXX)
TMP2=$(mktemp wixgen.XXXXXXXXXX)
TMP3=$(mktemp wixgen.XXXXXXXXXX)

rm -f $OUTFILE
rm -f $TMP1 $TMP2 $TMP3

test -f guids.inc && . ./guids.inc

getGuid()
{
	local KEY=$1
	local NEWGUID=$(eval echo \$GUID_$KEY)

	if [ -z "$NEWGUID" ] ; then
		# generate new GUID
		NEWGUID=$(uuidgen | tr a-z A-Z)
		echo "GUID_$KEY=$NEWGUID" >> guids.inc
	fi

	echo $NEWGUID
}

getHash()
{
	local FILE="$1"
	local HASH=$(openssl x509 -fingerprint -noout -in "$FILE" | sed -e "s/SHA1 Fingerprint=/0x/" -e "s/:\(..\)/,0x\1/g")

	echo $HASH
}

saveHash()
{
	local HASH=$1
	local TMP=$(mktemp wixgen.XXXXXXXXXX)

	cat hashes.inc > $TMP
	echo $HASH >> $TMP
	cat $TMP | sort | uniq > hashes.inc
	rm -f $TMP
}

genHashHeader()
{
	local OUTFILE=../../EsteidWixCustomAction/hashes.h
cat > $OUTFILE << EOF
/*
 * EsteidWixCustomAction - custom actions for Estonian EID card WiX installer
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

EOF
	echo "#define HASH_LEN 20" >> $OUTFILE
	echo "#define HASH_COUNT $(wc -l hashes.inc | cut -f 1 -d " ")" >> $OUTFILE
	echo "unsigned char hashes[][HASH_LEN] = {" >> $OUTFILE

	for HASH in `cat hashes.inc` ; do
		echo "    { $HASH }," >> $OUTFILE
	done
	echo "    0" >> $OUTFILE
	echo "};" >> $OUTFILE
}

for FILE in *.crt ; do
	# Skip if the glob doesn't match
	test -f "$FILE" || continue

	# Output test certs to different file.
	if [[ $FILE == TEST* ]] && [ x"$TEST" != x"yes" ] ; then
		continue
	fi
	if [[ $FILE != TEST* ]] && [ x"$TEST" = x"yes" ] ; then
		continue
	fi

	CERTNAME=$(echo $FILE | sed -e "s/\(.*\)\.crt/\1/g" -e "s/[- \.]/_/g")
	SOURCE="\$(var.CertSourceDir)\\$FILE"
	GUID=$(getGuid $CERTNAME)
	if [ x"$CERTNAME" = x"JUUR_SK" ] ; then
		STORENAME=root
		GUID=""
	else
		STORENAME=ca

		# Save hash to file
		HASH=$(getHash "$FILE")
		saveHash $HASH
	fi

cat >> $TMP1 << EOF
            <Component Id="${CERTNAME}_file" Guid="*">
                <File Id="$CERTNAME" Name="$FILE" Source="$SOURCE" />
            </Component>

EOF


	#if grep "^${FILE}\$" ca_certs.lst > /dev/null ; then
cat >> $TMP1 << EOF
            <Component Id="$CERTNAME" Guid="$GUID" KeyPath="yes">
EOF

	if [ x"$CERTNAME" = x"JUUR_SK" ] ; then
cat >> $TMP1 << EOF
                <Condition>NOT JUUR_CERTIFICATE_INSTALLED</Condition>
EOF
	fi

cat >> $TMP1 << EOF
                <iis:Certificate Id="$CERTNAME"
                                 Name="$CERTNAME"
                                 Request="no"
                                 StoreLocation="localMachine"
                                 StoreName="$STORENAME"
                                 Overwrite="yes"
                                 BinaryKey="$CERTNAME"
                                 />
            </Component>

EOF

cat >> $TMP2 << EOF
        <Binary Id="$CERTNAME" SourceFile="$SOURCE" />
EOF

cat >> $TMP3 << EOF
            <ComponentRef Id="$CERTNAME" />
EOF
	#fi


cat >> $TMP3 << EOF
            <ComponentRef Id="${CERTNAME}_file" />
EOF

done



cat >> $OUTFILE << EOF
<?xml version="1.0" encoding="utf-8"?>

<?define CertSourceDir = "certs" ?>

<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi"
     xmlns:iis="http://schemas.microsoft.com/wix/IIsExtension">

    <Fragment>
        <DirectoryRef Id="CertificatesFolder">
EOF

cat $TMP1 >> $OUTFILE

cat >> $OUTFILE << EOF
        </DirectoryRef>

EOF

cat $TMP2 >> $OUTFILE

cat >> $OUTFILE << EOF
    </Fragment>

    <Fragment>
        <ComponentGroup Id="$GROUP">
EOF

cat $TMP3 >> $OUTFILE

cat >> $OUTFILE << EOF
        </ComponentGroup>
    </Fragment>
</Wix>
EOF

genHashHeader

rm -f $TMP1 $TMP2 $TMP3
