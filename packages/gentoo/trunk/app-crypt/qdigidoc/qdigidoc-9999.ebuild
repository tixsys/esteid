# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

ESVN_REPO_URI="https://esteid.googlecode.com/svn/qdigidoc/trunk"

inherit cmake-utils subversion

DESCRIPTION="Digidoc client"
HOMEPAGE="https://id.eesti.ee/trac/"
LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="dev-libs/libdigidoc
	dev-libs/libdigidocpp
	dev-libs/openssl
	net-nds/openldap
	x11-libs/qt-core:4[ssl]
	x11-libs/qt-gui:4
	x11-libs/qt-webkit:4"
DEPEND="${RDEPEND}"

DOCS=""
