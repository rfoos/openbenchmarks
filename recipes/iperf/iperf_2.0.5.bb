DESCRIPTION = "Iperf is a tool to measure maximum TCP bandwidth, allowing the tuning of various parameters and UDP characteristics"
HOMEPAGE = "http://dast.nlanr.net/Projects/Iperf/"
SECTION = "console/network"
LICENSE = "NewBSD"
LIC_FILES_CHKSUM = "file://COPYING;md5=e8478eae9f479e39bc34975193360298"

SRC_URI = " \
	${SOURCEFORGE_MIRROR}/iperf/iperf-${PV}.tar.gz \
	"


SRC_URI[md5sum] = "44b5536b67719f4250faed632a3cd016"
SRC_URI[sha256sum] = "636b4eff0431cea80667ea85a67ce4c68698760a9837e1e9d13096d20362265b"

inherit autotools

S="${WORKDIR}/iperf-${PV}"

EXTRA_OECONF = "--exec-prefix=${STAGING_DIR_HOST}${layout_exec_prefix}"

do_configure() {
	export ac_cv_func_malloc_0_nonnull=yes
	gnu-configize
	oe_runconf
}

do_compile() {
	cd ${WORKDIR}/iperf-${PV}
	oe_runmake
}

do_install() {
	cd ${WORKDIR}/iperf-${PV}/src
	oe_runmake DESTDIR=${D} install
}


