# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

ESVN_REPO_URI="https://esteid.googlecode.com/svn/smartcardpp/trunk"

EAPI=2
inherit cmake-utils subversion

DESCRIPTION="A library for accessing smart cards"
HOMEPAGE="http://code.google.com/p/esteid/"
LICENSE="BSD"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="debug"

RDEPEND="sys-apps/pcsc-lite"
DEPEND="${RDEPEND}"

src_configure() {
	# If prefix is /usr, sysconf needs to be /etc, not /usr/etc
	local mycmakeargs="${mycmakeargs}
		-DSYSCONF_INSTALL_DIR=${ROOT}etc"

	use debug && mycmakeargs+=" -DCMAKE_BUILD_TYPE=Debug"

	cmake-utils_src_configure
}
