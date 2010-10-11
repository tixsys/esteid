# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="3"
inherit cmake-utils mozextension multilib nsplugins

MY_PN="esteid"
MY_P="${MY_PN}-${PV}"

FB_PN="firebreath"
FB_PV="1.2.2"
FB_P="${FB_PN}-${FB_PV}"

DESCRIPTION="Estonian ID card digital signing browser plugin"
HOMEPAGE="http://code.google.com/p/esteid/"
SRC_URI="http://esteid.googlecode.com/files/${P}.tar.bz2
	http://firebreath.googlecode.com/files/${FB_P}.tar.bz2"

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="debug"

RDEPEND="app-arch/zip
	dev-cpp/gtkmm
	dev-libs/boost
	dev-libs/openssl
	dev-libs/smartcardpp"

DEPEND="${RDEPEND}"

S="${WORKDIR}/${FB_P}"

src_unpack() {
	# unpack firebreath
	unpack "${FB_P}.tar.bz2"

	# unpack esteid-browser-plugin to projects/ subdir
	mkdir -p "${S}/projects"
	pushd "${S}/projects"
	unpack "${P}.tar.bz2"
	popd

	cd "${S}"
}

src_configure() {
	local mycmakeargs="-DWITH_SYSTEM_BOOST:BOOL=YES"

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
	# Install the extension for each of these Mozilla browsers;
	# if none is found just silently skip extension install.
	if has_version '>=www-client/firefox-3.5'; then
		MOZILLA_FIVE_HOME="/usr/$(get_libdir)/firefox"
		xpi_install "${WORKDIR}/${xpiname}"
		mozillas="$(best_version www-client/firefox) ${mozillas}"
	fi
	if has_version '>=www-client/firefox-bin-3.5'; then
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
