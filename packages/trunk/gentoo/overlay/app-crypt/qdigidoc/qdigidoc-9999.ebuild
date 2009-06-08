# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

ESVN_REPO_URI="https://id.eesti.ee/svn/qdigidocclient/trunk/"

inherit cmake-utils subversion

DESCRIPTION="Digidoc client"
HOMEPAGE="https://id.eesti.ee/trac/"
LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="dev-libs/libdigidoc++
	dev-libs/openssl
	sys-apps/pcsc-lite
	x11-libs/qt-core:4
	x11-libs/qt-gui:4"
DEPEND="${RDEPEND}"

DOCS=""
