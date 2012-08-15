# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="3"
inherit cmake-utils

DESCRIPTION="Digidoc client"
HOMEPAGE="http://code.google.com/p/esteid/"
SRC_URI="http://esteid.googlecode.com/files/${P}.tar.bz2"

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="gnome kde"

RDEPEND="dev-libs/libdigidoc
	dev-libs/libdigidocpp
	dev-libs/openssl
	gnome? ( dev-python/nautilus-python )
	net-nds/openldap
	x11-libs/qt-core:4[ssl]
	x11-libs/qt-gui:4
	x11-libs/qt-webkit:4"
DEPEND="${RDEPEND}"

DOCS="AUTHORS NEWS README"

src_configure() {
	local mycmakeargs="
		$(cmake-utils_use_enable gnome NAUTILUS_EXTENSION)
		$(cmake-utils_use_enable kde KDE)
	"

	cmake-utils_src_configure
}
