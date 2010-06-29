# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

ESVN_REPO_URI="https://esteid.googlecode.com/svn/libdigidocpp/trunk"

EAPI=2
inherit cmake-utils subversion

DESCRIPTION="C++ library for handling BDoc and DDoc digital signature containers"
HOMEPAGE="https://id.eesti.ee/trac/"
LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="debug"

RDEPEND="dev-libs/libp11
	dev-libs/libdigidoc
	dev-libs/opensc
	dev-libs/openssl
	dev-libs/pkcs11-helper
	dev-libs/xerces-c
	dev-libs/xml-security-c
	dev-util/cppunit
	sys-libs/zlib"
DEPEND="${RDEPEND}
	>=dev-cpp/xsd-3.2.0"

DOCS="README.linux"

src_configure() {
	# If prefix is /usr, sysconf needs to be /etc, not /usr/etc
	local mycmakeargs="${mycmakeargs}
		-DSYSCONF_INSTALL_DIR=${ROOT}etc"

	use debug && mycmakeargs+=" -DCMAKE_BUILD_TYPE=Debug"

	cmake-utils_src_configure
}
