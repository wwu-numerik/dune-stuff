# $Id: duneproject 5104 2008-03-06 16:24:04Z robertk $

# we need the module file to be able to build via dunecontrol
EXTRA_DIST=dune.module

# possible options
#LDADD = $(UG_LDFLAGS) $(AMIRAMESH_LDFLAGS) $(UG_LIBS) $(AMIRAMESH_LIBS)
#AM_CPPFLAGS = $(UG_CPPFLAGS) $(AMIRAMESH_CPPFLAGS)
SUBDIRS = dune src

# don't follow the full GNU-standard
# we need automake 1.5
AUTOMAKE_OPTIONS = foreign 1.5
# pass most important options when "make distcheck" is used
DISTCHECK_CONFIGURE_FLAGS = --with-dune=$(DUNEROOT) CXX="$(CXX)" CC="$(CC)"

include $(top_srcdir)/am/global-rules
include $(top_srcdir)/am/top-rules
