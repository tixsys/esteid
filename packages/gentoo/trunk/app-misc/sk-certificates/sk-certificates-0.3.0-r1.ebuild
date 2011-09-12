# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=3

MY_P="libdigidocpp-${PV}"

DESCRIPTION="Estonian root certificates for PKI"
HOMEPAGE="http://code.google.com/p/esteid/"
SRC_URI="http://esteid.googlecode.com/files/${MY_P}.tar.bz2"

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

S=${WORKDIR}/${MY_P}

DEPEND="!<dev-libs/libdigidocpp-0.3.0-r1"

src_install() {
	insinto /etc/digidocpp
	doins -r etc/certs
}
