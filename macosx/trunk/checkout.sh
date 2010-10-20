#!/bin/sh

PROJECTS="esteid-browser-plugin libdigidoc libdigidocpp qdigidoc"
PROJECTS="$PROJECTS qesteidutil smartcardpp"

firebreath_URL="https://firebreath.googlecode.com/hg/"
firebreath_REV="1f54d03c6e25"
HG=/opt/local/bin/hg

tokend_URL="git://github.com/martinpaljak/OpenSC.tokend.git"
GIT=/usr/local/git/bin/git

WGET=/opt/local/bin/wget

URL=http://esteid.googlecode.com/svn
BRANCH="trunk"
VER="3191"

for project in $PROJECTS
do
    mkdir -p $project
    pushd $project
    svn checkout -r$VER $URL/$project/$BRANCH trunk
    popd
done

svn checkout -r$VER $URL/externals

$HG clone -r $firebreath_REV $firebreath_URL firebreath
$GIT clone $tokend_URL OpenSC.tokend

$WGET http://martinpaljak.net/download/build-10.5.tar.gz
$WGET http://martinpaljak.net/download/build-10.6.tar.gz
