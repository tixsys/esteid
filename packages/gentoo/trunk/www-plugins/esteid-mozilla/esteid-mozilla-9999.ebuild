# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit cmake-utils mozextension subversion

ESVN_REPO_URI="https://id.eesti.ee/svn/plugins/firefox/trunk/"

MY_PN="esteid"
MY_PV="0.4.0"
MY_P="${MY_PN}-${MY_PV}"

DESCRIPTION="Estonian ID Card Mozilla signing extension"
HOMEPAGE="https://id.eesti.ee/trac/"
LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="debug"

RDEPEND="dev-libs/smartcardpp
	app-arch/zip
	|| (
		>=www-client/mozilla-firefox-3.0
		>=www-client/mozilla-firefox-bin-3.0
		>=www-client/seamonkey-2.0
		>=www-client/seamonkey-bin-2.0
		>=mail-client/mozilla-thunderbird-2.0
		>=mail-client/mozilla-thunderbird-bin-2.0
	)
	>=net-libs/xulrunner-1.9
	>=dev-libs/nspr-4.7.1
	!www-plugins/esteid-firefox"

DEPEND="${RDEPEND}"

src_configure() {
	use debug && mycmakeargs+=" -DCMAKE_BUILD_TYPE=Debug"

	cmake-utils_src_configure
}

src_install() {
	local MOZILLA_FIVE_HOME xpiname
	mozillas=""

	xpiname="${MY_P}"
	xpi_unpack "${CMAKE_BUILD_DIR}/${xpiname}.xpi"

	# FIXME: Hard-coded MOZILLA_FIVE_HOME dirs
	if has_version '>=www-client/mozilla-firefox-3.0'; then
		MOZILLA_FIVE_HOME="/usr/$(get_libdir)/mozilla-firefox"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version www-client/mozilla-firefox) ${mozillas}"
	fi
	if has_version '>=www-client/mozilla-firefox-bin-3.0'; then
		MOZILLA_FIVE_HOME="/opt/firefox"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version www-client/mozilla-firefox-bin) ${mozillas}"
	fi
	if has_version '>=www-client/seamonkey-2.0'; then
		MOZILLA_FIVE_HOME="/usr/$(get_libdir)/seamonkey"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version www-client/seamonkey) ${mozillas}"
	fi
	if has_version '>=www-client/seamonkey-bin-2.0'; then
		MOZILLA_FIVE_HOME="/opt/seamonkey"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version www-client/seamonkey-bin) ${mozillas}"
	fi
	if has_version '>=mail-client/mozilla-thunderbird-2.0'; then
		MOZILLA_FIVE_HOME="/usr/$(get_libdir)/mozilla-thunderbird"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version mail-client/mozilla-thunderbird) ${mozillas}"
	fi
	if has_version '>=mail-client/mozilla-thunderbird-bin-2.0'; then
		MOZILLA_FIVE_HOME="/opt/thunderbird"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version mail-client/mozilla-thunderbird-bin) ${mozillas}"
	fi
}

pkg_postinst() {
	elog "${PN} has been installed for the following packages:"
	for i in ${mozillas}; do
		elog "  $i"
	done
}
