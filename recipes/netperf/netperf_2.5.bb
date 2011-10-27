DESCRIPTION = "Network performance benchmark including tests for TCP, UDP, sockets, ATM and more."
SECTION = "console/network"
HOMEPAGE = "http://www.netperf.org/"
LICENSE = "netperf"
LIC_FILES_CHKSUM = "file://COPYING;md5=a0ab17253e7a3f318da85382c7d5d5d6"
PR = "r3"

SRC_URI="ftp://ftp.netperf.org/netperf/netperf-2.5.0.tar.gz"

inherit update-rc.d autotools

S = "${WORKDIR}/netperf-${PV}"

# cpu_set.patch plus _GNU_SOURCE makes src/netlib.c compile with CPU_ macros
CFLAGS_append = " -DDO_UNIX -DDO_IPV6 -D_GNU_SOURCE"

do_install() {
        install -d ${D}${sbindir} ${D}${bindir} ${D}${sysconfdir}/init.d
        install -m 4755 src/netperf ${D}${bindir}
        install -m 4755 src/netserver ${D}${sbindir}
        install -m 0755 ${WORKDIR}/init ${D}${sysconfdir}/init.d/netperf

        # man
        install -d ${D}${mandir}/man1/
        install -m 0644 doc/netserver.man ${D}${mandir}/man1/netserver.1
        install -m 0644 doc/netperf.man ${D}${mandir}/man1/netperf.1

        # move scripts to examples directory
        install -d ${D}${docdir}/netperf/examples
        install -m 0644 doc/examples/*_script ${D}${docdir}/netperf/examples/

        # docs ..
        install -m 0644 COPYING ${D}${docdir}/netperf
        install -m 0644 Release_Notes ${D}${docdir}/netperf
        install -m 0644 README ${D}${docdir}/netperf
        install -m 0644 doc/netperf_old.ps ${D}${docdir}/netperf
}

INITSCRIPT_NAME = "netperf"
INITSCRIPT_PARAMS = "defaults"

SRC_URI[md5sum] = "249af118c891688afdc8a3e6b7257ff5"
SRC_URI[sha256sum] = "2d8d1388a16c5c5704c1821d7bf35cb60c4928a4b0f92f62888b025885b9c4f1"
