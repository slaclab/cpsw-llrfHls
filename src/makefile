CPSW_DIR=/afs/slac/g/lcls/package/cpsw/framework/R4.4.1/src

CCACHE_DISABLE=1

SRCDIR=.
-include $(SRCDIR)/release.mak

include $(CPSW_DIR)/defs.mak

INSTALL_DIR=../

HEADERS += llrfFw.h

llrfHls_SRCS  = llrfFw.cc
llrfHls_LIBS  = $(CPSW_LIBS)

SHARED_LIBRARIES_YES+=llrfHls
STATIC_LIBRARIES_YES+=llrfHls

llrfHls_tst_SRCS = llrfFw_tst.cc
llrfHls_tst_LIBS = llrfHls
llrfHls_tst_LIBS += $(CPSW_LIBS)

PROGRAMS=llrfHls_tst

include $(CPSW_DIR)/rules.mak
