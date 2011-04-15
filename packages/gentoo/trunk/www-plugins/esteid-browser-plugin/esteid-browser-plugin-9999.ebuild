# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="3"

inherit cmake-utils git mozextension multilib nsplugins subversion

EGIT_BRANCH="firebreath-1.3"
EGIT_COMMIT="${EGIT_BRANCH}"
EGIT_REPO_URI="git://github.com/firebreath/FireBreath.git"
ESVN_REPO_URI="https://esteid.googlecode.com/svn/${PN}/trunk"

MY_PN="esteid"
MY_PV="1.2.0"
MY_P="${MY_PN}-${MY_PV}"

DESCRIPTION="Estonian ID card digital signing browser plugin"
HOMEPAGE="http://code.google.com/p/esteid/"
LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="debug"

RDEPEND="app-arch/zip
	dev-cpp/gtkmm:2.4
	dev-libs/boost
	dev-libs/openssl
	dev-libs/smartcardpp"

DEPEND="${RDEPEND}"

src_unpack() {
	# unpack firebreath
	git_src_unpack
	# unpack esteid-browser-plugin to projects/ subdir
	subversion_fetch ${ESVN_REPO_URI} projects/${MY_PN}
}

src_configure() {
	local mycmakeargs="
		-DSYSCONF_INSTALL_DIR=/etc
		-DWITH_SYSTEM_BOOST=ON
	"

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
