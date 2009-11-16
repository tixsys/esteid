# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvsroot/gentoo-x86/dev-libs/opensc/opensc-0.11.9.ebuild,v 1.1 2009/07/29 14:00:10 arfrever Exp $

EAPI="2"

if [[ ${PV} = 9999* ]]; then
	SVN_ECLASS="subversion"
	AUTOTOOLS_ECLASS="autotools"
	ESVN_REPO_URI="http://www.opensc-project.org/svn/opensc/trunk"
	EXPERIMENTAL="true"
else
	SRC_URI="http://www.opensc-project.org/files/${PN}/${P}.tar.gz"
fi

inherit ${AUTOTOOLS_ECLASS} multilib ${SVN_ECLASS}

DESCRIPTION="SmartCard library and applications"
HOMEPAGE="http://www.opensc-project.org/opensc/"

KEYWORDS="~alpha ~amd64 ~arm ~hppa ~ia64 ~m68k ~ppc ~ppc64 ~s390 ~sh ~sparc ~x86"

LICENSE="LGPL-2.1"
SLOT="0"
IUSE="pcsc-lite openct nsplugin doc"

RDEPEND="dev-libs/openssl
	sys-libs/zlib
	openct? ( >=dev-libs/openct-0.5.0 )
	pcsc-lite? ( >=sys-apps/pcsc-lite-1.3.0 )
	nsplugin? (
		app-crypt/pinentry
		x11-libs/libXt
	)"
DEPEND="${RDEPEND}
	app-text/docbook-xsl-stylesheets
	dev-util/pkgconfig
	nsplugin? ( dev-libs/libassuan )"

if [[ ${PV} = 9999* ]]; then
	src_unpack() {
		subversion_src_unpack
		eautoreconf || die "eautoreconf failed"
		cd "${S}"
	}
fi

src_configure() {
	econf \
		--docdir="/usr/share/doc/${PF}" \
		--htmldir="/usr/share/doc/${PF}/html" \
		$(use_enable openct) \
		$(use_enable pcsc-lite pcsc) \
		$(use_enable nsplugin) \
		$(use_enable doc) \
		--with-pinentry="/usr/bin/pinentry"
}

src_install() {
	emake DESTDIR="${D}" install || die "emake install failed"
}
