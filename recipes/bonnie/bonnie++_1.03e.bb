DESCRIPTION = "Tests large file IO and creation/deletion of small files."
HOMEPAGE = "http://www.coker.com.au/bonnie++/"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://copyright.txt;md5=cd4dde95a6b9d122f0a9150ae9cc3ee0"

SRC_URI = "http://www.coker.com.au/bonnie++/bonnie++-${PV}.tgz \
           file://gcc-4.3-fixes.patch \
           "
SRC_URI[md5sum] = "750aa5b5051263a99c6c195888c74968"
SRC_URI[sha256sum] = "cb3866116634bf65760b6806be4afa7e24a1cad6f145c876df8721f01ba2e2cb"

inherit autotools

SCRIPTS = "bon_csv2html bon_csv2txt"
EXES = "bonnie++ zcav"

TARGET_CC_ARCH += "${LDFLAGS}"

do_install () {
        install -d ${D}/bin
        install -d ${D}/sbin
        install -m 0755 ${EXES} ${D}/sbin
        install -m 0755 ${SCRIPTS} ${D}/bin
}

PACKAGES =+ "bonnie-scripts"

FILES_${PN} = "${base_sbindir}"
FILES_bonnie-scripts = "${base_bindir}"



