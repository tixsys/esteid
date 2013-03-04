# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="3"
inherit cmake-utils

DESCRIPTION="Estonian ID card utility"
HOMEPAGE="http://code.google.com/p/esteid/"
SRC_URI="http://esteid.googlecode.com/files/${P}.tar.bz2"

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="dev-libs/smartcardpp
	dev-libs/libp11
	dev-qt/qtcore:4[ssl]
	dev-qt/qtgui:4
	dev-qt/qtwebkit:4"
DEPEND="${RDEPEND}"

DOCS="AUTHORS NEWS README"
