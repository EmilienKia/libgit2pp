## Process this file with automake to produce Makefile.in
## Created by Anjuta

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

libgit2ppdocdir = ${prefix}/doc/libgit2pp
libgit2ppdoc_DATA = \
	README\
	README.md\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(libgit2ppdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(libgit2ppdocdir)
