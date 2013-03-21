# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=3
inherit eutils cmake-utils

DESCRIPTION="Library for accessing smart cards"
HOMEPAGE="http://code.google.com/p/esteid/"
SRC_URI="http://esteid.googlecode.com/files/${P}.tar.bz2"

LICENSE="BSD"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="sys-apps/pcsc-lite"
DEPEND="${RDEPEND}
	dev-util/pkgconfig"

DOCS="NEWS"

src_prepare() {
	cmake-utils_src_prepare
	epatch "$FILESDIR"/sleep-header.patch
}

src_configure() {
	append-cppflags -DHAVE_GETOPT_H
	cmake-utils_src_configure
}
