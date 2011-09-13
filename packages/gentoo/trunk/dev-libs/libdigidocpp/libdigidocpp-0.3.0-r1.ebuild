# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=3
inherit cmake-utils

DESCRIPTION="Library for creating and validating BDoc and DDoc containers"
HOMEPAGE="http://code.google.com/p/esteid/"
SRC_URI="http://esteid.googlecode.com/files/${P}.tar.bz2"

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="dev-libs/libp11
	dev-libs/libdigidoc
	dev-libs/opensc
	dev-libs/openssl
	dev-libs/xerces-c
	dev-libs/xml-security-c
	dev-util/cppunit
	sys-libs/zlib"
DEPEND="${RDEPEND}
	=app-misc/sk-certificates-${PV}
	>=dev-cpp/xsd-3.2.0"

DOCS="AUTHORS NEWS README"

src_prepare() {
	# We use another package (app-misc/sk-certificates) to install root certs
	cd "${S}"
	rm etc/certs/*
}

src_configure() {
	# If prefix is /usr, sysconf needs to be /etc, not /usr/etc
	local mycmakeargs="${mycmakeargs}
		-DSYSCONF_INSTALL_DIR=${EROOT}etc"

	cmake-utils_src_configure
}
