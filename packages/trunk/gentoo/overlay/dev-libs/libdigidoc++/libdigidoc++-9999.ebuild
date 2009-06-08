# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

ESVN_REPO_URI="https://id.eesti.ee/svn/libdigidoc/cpp/trunk/"

inherit cmake-utils subversion

DESCRIPTION="C++ library for handling BDoc and DDoc digital signature containers"
HOMEPAGE="https://id.eesti.ee/trac/"
LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="dev-libs/libp11
	dev-libs/libdigidoc2
	dev-libs/opensc
	dev-libs/openssl
	dev-libs/xerces-c
	dev-libs/xml-security-c
	dev-util/cppunit
	sys-libs/zlib"
DEPEND="${RDEPEND}
	>=dev-cpp/xsd-3.2.0"

DOCS="README.linux"

src_unpack() {
	subversion_src_unpack
	cd "${S}"
	sed -e "s#DESTINATION lib#DESTINATION $(get_libdir)#" -i src/CMakeLists.txt
}
