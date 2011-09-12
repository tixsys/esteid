# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

ESVN_REPO_URI="https://esteid.googlecode.com/svn/libdigidocpp/trunk"

EAPI=3
inherit cmake-utils subversion

DESCRIPTION="Library for creating and validating BDoc and DDoc containers"
HOMEPAGE="http://code.google.com/p/esteid/"
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
	>=dev-cpp/xsd-3.2.0"

DOCS="AUTHORS NEWS README"

src_prepare() {
	cd ${S}
	rm -r etc/certs/
}

src_configure() {
	# If prefix is /usr, sysconf needs to be /etc, not /usr/etc
	local mycmakeargs="${mycmakeargs}
		-DSYSCONF_INSTALL_DIR=${EROOT}etc"

	cmake-utils_src_configure
}
