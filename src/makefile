
ifeq ($(PACKAGE_TOP),)
ifneq ($(EPICS_PACKAGE_TOP),)
	PACKAGE_TOP=$(EPICS_PACKAGE_TOP)
else
	$(error PACKAGE_TOP or EPICS_PACKAGE_TOP must be provided by the environment or on the command line)
endif
endif

CPSW_DIR=$(PACKAGE_TOP)/cpsw/framework/R4.4.1/src

CCACHE_DISABLE=1

SRCDIR=.
-include $(SRCDIR)/release.mak

include $(CPSW_DIR)/defs.mak

INSTALL_DIR=../

HEADERS += llrfFw.h
HEADERS += llrfDestnTrig.h
HEADERS += interlockRtmFw.h
HEADERS += dacSigGenFw.h

llrfHls_SRCS  += llrfFw.cc
llrfHls_SRCS  += llrfDestnTrig.cc
llrfHls_SRCS  += interlockRtmFw.cc
llrfHls_SRCS  += dacSigGenFw.cc
llrfHls_LIBS  += $(CPSW_LIBS)

SHARED_LIBRARIES_YES+=llrfHls
STATIC_LIBRARIES_YES+=llrfHls

llrfHls_tst_SRCS = llrfFw_tst.cc
llrfHls_tst_LIBS = llrfHls
llrfHls_tst_LIBS += $(CPSW_LIBS)

PROGRAMS=llrfHls_tst

include $(CPSW_DIR)/rules.mak
