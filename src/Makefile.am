## Process this file with automake to produce Makefile.in

## Created by Anjuta

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(LIBGIT2PP_CFLAGS) \
	$(libgit2_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g


lib_LTLIBRARIES =  \
	libgit2pp.la

libgit2pp_la_SOURCES =  \
	common.cpp \
	common.hpp \
	blob.cpp \
	blob.hpp \
	branch.cpp \
	branch.hpp \
	commit.cpp \
	commit.hpp \
	config.cpp \
	config.hpp \
	database.cpp \
	database.hpp \
	diff.cpp \
	diff.hpp \
	exception.cpp \
	exception.hpp \
	index.cpp \
	index.hpp \
	object.cpp \
	object.hpp \
	oid.cpp \
	oid.hpp \
	ref.cpp \
	ref.hpp \
	repository.cpp \
	repository.hpp \
	revwalk.hpp \
	revwalk.cpp \
	signature.cpp \
	signature.hpp \
	status.hpp \
	status.cpp \
	tag.cpp \
	tag.hpp \
	tree.cpp \
	tree.hpp \
	remote.hpp \
	remote.cpp


libgit2pp_la_LIBADD = $(libgit2_LIBS)

include_HEADERS = git2pp.hpp

git2ppincludedir = $(includedir)/git2pp
git2ppinclude_HEADERS = \
	common.hpp \
	blob.hpp \
	branch.hpp \
	commit.hpp \
	config.hpp \
	database.hpp \
	diff.hpp \
	exception.hpp \
	index.hpp \
	object.hpp \
	oid.hpp \
	ref.hpp \
	repository.hpp \
	revwalk.hpp \
	signature.hpp \
	status.hpp \
	tag.hpp \
	tree.hpp \
	remote.hpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgit2pp.pc


