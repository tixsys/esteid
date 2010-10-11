# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=3
inherit cmake-utils

DESCRIPTION="Library for handling digitally signed documents"
HOMEPAGE="http://code.google.com/p/esteid/"
SRC_URI="http://esteid.googlecode.com/files/${P}.tar.bz2"

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="dev-libs/libxml2
	dev-libs/opensc
	dev-libs/openssl
	sys-libs/zlib"
DEPEND="${RDEPEND}"

DOCS="AUTHORS ChangeLog NEWS README"

src_configure() {
	# If prefix is /usr, sysconf needs to be /etc, not /usr/etc
	local mycmakeargs="${mycmakeargs}
		-DSYSCONF_INSTALL_DIR=${EROOT}etc"

	cmake-utils_src_configure
}
