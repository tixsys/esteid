# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="3"

ESVN_REPO_URI="https://esteid.googlecode.com/svn/qdigidoc/trunk"

inherit cmake-utils subversion

DESCRIPTION="Digidoc client"
HOMEPAGE="http://code.google.com/p/esteid/"
LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="gnome kde"

RDEPEND="dev-libs/libdigidoc
	dev-libs/libdigidocpp
	dev-libs/openssl
	dev-qt/qtcore:4[ssl]
	dev-qt/qtgui:4
	dev-qt/qtwebkit:4
	gnome? ( dev-python/nautilus-python )
	net-nds/openldap"
DEPEND="${RDEPEND}"

DOCS="AUTHORS NEWS README"

src_configure() {
	local mycmakeargs="
		$(cmake-utils_use_enable gnome NAUTILUS_EXTENSION)
		$(cmake-utils_use_enable kde KDE)
	"

	cmake-utils_src_configure
}
