DESCRIPTION = "DHRYSTONE Benchmark Program"
PR = "r0"
PRIORITY = "optional"
SECTION = "console/utils"
DEPENDS = ""
LICENSE = "GPL"
LIC_FILES_CHKSUM = "file:///README_C;md5=2d45e81a2a82cecb15f8152178348883"
AUTHOR = "Reinhold P. Weicker, Rick Richardson"
# No original home page. ACM/IEEE article.
# Original shar file: http://www.netlib.org/benchmark/dhry-c
HOMEPAGE = "http://en.wikipedia.org/wiki/Dhrystone"

# Original wrapped with automake builder
SRC_URI = "file://dhrystone-${PV}.tar.gz"
#SRC_URI_native = "file://dhrystone-${PV}.tar.gz"

inherit autotools

do_configure_prepend() {
    cd ${S}
    ./autogen.sh
}    

do_install() {
	install -d ${D}/home/root/dhrystone
	cp ${WORKDIR}/${PN}-${PV}/dhry ${D}/home/root/dhrystone
}

FILES_${PN}-dbg = "/home/root/${PN}/.debug"
FILES_${PN} = "/home/root/${PN}/*"


# modified, no clean drop space with README_C
# SRC_URI = "http://ftp.unicamp.br/pub/unix-c/benchmark/system/dhrystone-2.1.tar.gz;name=dhrystone"
# SRC_URI[dhrystone.md5sum] = "f17bf168821901c29ccb074fcfd2dfe9"
# SRC_URI[dhrystone.sha256sum] = "7e002c721b106690c63eb07c0de906d2d7bd383b6e69db4bbfa3586027e5556a"
NATIVE_INSTALL_WORKS = "0"
BBCLASSEXTEND = "native"
