CPSW_DIR=/afs/slac/g/lcls/package/cpsw/framework/R4.4.1/src

CCACHE_DISABLE=1

SRCDIR=.
-include $(SRCDIR)/release.mak

include $(CPSW_DIR)/defs.mak

INSTALL_DIR=../

HEADERS += llrfFw.h

llrf_SRCS  = llrfFw.cc
llrf_LIBS  = $(CPSW_LIBS)

SHARED_LIBRARIES_YES+=llrf
STATIC_LIBRARIES_YES+=llrf

llrf_tst_SRCS = llrfFw_tst.cc
llrf_tst_LIBS = llrf
llrf_tst_LIBS += $(CPSW_LIBS)

PROGRAMS=llrf_tst

include $(CPSW_DIR)/rules.mak
