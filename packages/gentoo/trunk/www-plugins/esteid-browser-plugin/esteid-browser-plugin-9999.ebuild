# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit cmake-utils mercurial mozextension multilib nsplugins subversion

ESVN_REPO_URI="https://esteid.googlecode.com/svn/${PN}/trunk"
EHG_REPO_URI="https://firebreath.googlecode.com/hg/"

MY_PN="esteid"
MY_PV="1.0.0"
MY_P="${MY_PN}-${MY_PV}"

DESCRIPTION="Estonian ID card digital signing browser plugin"
HOMEPAGE="https://id.eesti.ee/trac/"
LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="debug"

RDEPEND="app-arch/zip
	dev-cpp/gtkmm
	dev-libs/boost
	dev-libs/openssl
	dev-libs/smartcardpp
	|| (
		>=www-client/firefox-3.0
		>=www-client/firefox-bin-3.0
		>=www-client/seamonkey-2.0
		>=www-client/seamonkey-bin-2.0
		>=mail-client/thunderbird-2.0
		>=mail-client/thunderbird-bin-2.0
	)
	!www-plugins/esteid-firefox
	!www-plugins/esteid-mozilla"

DEPEND="${RDEPEND}"

S="${WORKDIR}/hg"

src_unpack() {
	# unpack firebreath
	mercurial_src_unpack
	# unpack esteid-browser-plugin to projects/ subdir
	S="${S}/projects/${PN}" subversion_src_unpack

	cd "${S}"
}

src_configure() {
	local mycmakeargs="
		-DWITH_SYSTEM_BOOST:BOOL=YES
		-DPROJECTS_DIR=projects/"
	use debug && mycmakeargs+=" -DCMAKE_BUILD_TYPE=Debug"

	cmake-utils_src_configure
}

src_install() {
	cmake-utils_src_install

	inst_plugin "/usr/$(get_libdir)/mozilla/plugins/npesteid.so"


	local MOZILLA_FIVE_HOME xpiname
	mozillas=""

	xpiname="${MY_P}"
	xpi_unpack "${CMAKE_BUILD_DIR}/${xpiname}.xpi"

	# FIXME: Hard-coded MOZILLA_FIVE_HOME dirs
	if has_version '>=www-client/firefox-3.0'; then
		MOZILLA_FIVE_HOME="/usr/$(get_libdir)/mozilla-firefox"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version www-client/firefox) ${mozillas}"
	fi
	if has_version '>=www-client/firefox-bin-3.0'; then
		MOZILLA_FIVE_HOME="/opt/firefox"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version www-client/firefox-bin) ${mozillas}"
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
	if has_version '>=mail-client/thunderbird-2.0'; then
		MOZILLA_FIVE_HOME="/usr/$(get_libdir)/thunderbird"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version mail-client/thunderbird) ${mozillas}"
	fi
	if has_version '>=mail-client/thunderbird-bin-2.0'; then
		MOZILLA_FIVE_HOME="/opt/thunderbird"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version mail-client/thunderbird-bin) ${mozillas}"
	fi
}

pkg_postinst() {
	elog "${PN} has been installed for the following packages:"
	for i in ${mozillas}; do
		elog "  $i"
	done
}
