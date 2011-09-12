# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

ESVN_REPO_URI="https://esteid.googlecode.com/svn/libdigidocpp/trunk"

EAPI=3
inherit subversion

MY_P="libdigidocpp-${PV}"

DESCRIPTION="Estonian root certificates for PKI"
HOMEPAGE="http://code.google.com/p/esteid/"

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

S=${WORKDIR}/${MY_P}

src_install() {
	insinto /etc/digidocpp
	doins -r etc/certs
}
