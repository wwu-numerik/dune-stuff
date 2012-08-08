# Automake file

ACLOCAL_AMFLAGS = -I m4

# Nonstandard package files for distribution
EXTRA_DIST = \
  CHANGES \
  CONTRIBUTORS \
  include/gtest/gtest-param-test.h.pump \
  include/gtest/internal/gtest-param-util-generated.h.pump \
  include/gtest/internal/gtest-tuple.h.pump \
  include/gtest/internal/gtest-type-util.h.pump \
  make/Makefile \
  scripts/fuse_gtest_files.py \
  scripts/gen_gtest_pred_impl.py \
  scripts/pump.py \
  scripts/test/Makefile

# gtest source files that we don't compile directly.  They are
# #included by gtest-all.cc.
GTEST_SRC = \
  src/gtest-death-test.cc \
  src/gtest-filepath.cc \
  src/gtest-internal-inl.h \
  src/gtest-port.cc \
  src/gtest-printers.cc \
  src/gtest-test-part.cc \
  src/gtest-typed-test.cc \
  src/gtest.cc

EXTRA_DIST += $(GTEST_SRC)

# CMake script
EXTRA_DIST += \
  CMakeLists.txt \
  cmake/internal_utils.cmake


# Distribute and install M4 macro
m4datadir = $(datadir)/aclocal
m4data_DATA = m4/gtest.m4
EXTRA_DIST += $(m4data_DATA)

# We define the global AM_CPPFLAGS as everything we compile includes from these
# directories.
AM_CPPFLAGS = -I$(srcdir) -I$(srcdir)/include

# Modifies compiler and linker flags for pthreads compatibility.
if HAVE_PTHREADS
  AM_CXXFLAGS = @PTHREAD_CFLAGS@ -DGTEST_HAS_PTHREAD=1
  AM_LIBS = @PTHREAD_LIBS@
else
  AM_CXXFLAGS = -DGTEST_HAS_PTHREAD=0
endif

# Build rules for libraries.
lib_LTLIBRARIES = lib/libgtest.la lib/libgtest_main.la

lib_libgtest_la_SOURCES = src/gtest-all.cc

pkginclude_HEADERS = \
  include/gtest/gtest-death-test.h \
  include/gtest/gtest-message.h \
  include/gtest/gtest-param-test.h \
  include/gtest/gtest-printers.h \
  include/gtest/gtest-spi.h \
  include/gtest/gtest-test-part.h \
  include/gtest/gtest-typed-test.h \
  include/gtest/gtest.h \
  include/gtest/gtest_pred_impl.h \
  include/gtest/gtest_prod.h

pkginclude_internaldir = $(pkgincludedir)/internal
pkginclude_internal_HEADERS = \
  include/gtest/internal/gtest-death-test-internal.h \
  include/gtest/internal/gtest-filepath.h \
  include/gtest/internal/gtest-internal.h \
  include/gtest/internal/gtest-linked_ptr.h \
  include/gtest/internal/gtest-param-util-generated.h \
  include/gtest/internal/gtest-param-util.h \
  include/gtest/internal/gtest-port.h \
  include/gtest/internal/gtest-string.h \
  include/gtest/internal/gtest-tuple.h \
  include/gtest/internal/gtest-type-util.h

lib_libgtest_main_la_SOURCES = src/gtest_main.cc
lib_libgtest_main_la_LIBADD = lib/libgtest.la


maintainer-clean-local:
	rm -rf "$(srcdir)/fused-src"

# Death tests may produce core dumps in the build directory. In case
# this happens, clean them to keep distcleancheck happy.
CLEANFILES = core

# Disables 'make install' as installing a compiled version of Google
# Test can lead to undefined behavior due to violation of the
# One-Definition Rule.

install-exec-local:
	echo "'make install' is dangerous and not supported. Instead, see README for how to integrate Google Test into your build system."
	false

install-data-local:
	echo "'make install' is dangerous and not supported. Instead, see README for how to integrate Google Test into your build system."
	false
