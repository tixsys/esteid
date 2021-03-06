# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=3

ESVN_REPO_URI="https://esteid.googlecode.com/svn/smartcardpp/trunk"
inherit cmake-utils subversion

DESCRIPTION="Library for accessing smart cards"
HOMEPAGE="http://code.google.com/p/esteid/"
LICENSE="BSD"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="sys-apps/pcsc-lite"
DEPEND="${RDEPEND}
	dev-util/pkgconfig"

DOCS="NEWS"
