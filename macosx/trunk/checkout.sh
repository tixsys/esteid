#!/bin/sh

cd ../..

PROJECTS="esteid-browser-plugin libdigidoc libdigidocpp qdigidoc"
PROJECTS="$PROJECTS qesteidutil smartcardpp"

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
