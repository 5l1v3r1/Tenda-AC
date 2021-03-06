## Process this file with automake to produce Makefile.in
## =====================================================================
##
##     Makefile.am
##
##     Build support for the host-side synthetic target support,
##     the arch package.
##     
##
## =====================================================================
# ####ECOSHOSTGPLCOPYRIGHTBEGIN####                                         
# -------------------------------------------                               
# This file is part of the eCos host tools.                                 
# Copyright (C) 2002, 2003 Free Software Foundation, Inc.                   
#
# This program is free software; you can redistribute it and/or modify      
# it under the terms of the GNU General Public License as published by      
# the Free Software Foundation; either version 2 or (at your option) any    
# later version.                                                            
#
# This program is distributed in the hope that it will be useful, but       
# WITHOUT ANY WARRANTY; without even the implied warranty of                
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
# General Public License for more details.                                  
#
# You should have received a copy of the GNU General Public License         
# along with this program; if not, write to the                             
# Free Software Foundation, Inc., 51 Franklin Street,                       
# Fifth Floor, Boston, MA  02110-1301, USA.                                 
# -------------------------------------------                               
# ####ECOSHOSTGPLCOPYRIGHTEND####                                           
## =====================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):	bartv
## Contact(s):	bartv
## Date:	2002/08/06
## Version:	0.01
##
######DESCRIPTIONEND####
## =====================================================================

AUTOMAKE_OPTIONS = 1.10 foreign

## Only some platforms are supported. Having the configure script throw
## an error when attempting to configure on an unsupported platform
## would be a mistake, since that would prevent any configury from
## the toplevel on unsupported platforms. Instead an automake conditional
## is used, leading to null makefiles on unsupported platforms.


AM_CFLAGS       = @ecos_CFLAGS@ -DECOSYNTH_VERSION=\"@VERSION@\" \
 -DECOS_REPOSITORY=\"@ECOS_REPOSITORY@\" \
 -DLIBEXECDIR=\"$(libexecdir)\" \
 -DPKG_DIR=\"@PACKAGE_DIR@\" \
 -DPKG_VERSION=\"@PACKAGE_VERSION@\" \
 -DPKG_INSTALL=\"@PACKAGE_INSTALL@\"
AM_CXXFLAGS     = @ecos_CXXFLAGS@
INCLUDES        = @ecos_INCLUDES@ @ecos_tk_includes@
LIBS            = @ecos_LIBS@ @ecos_LDADD@

if SUPPORTED
## The synthetic target support consists of a single program ecosynth,
## a number of Tcl scripts, and some additional data files. These are
## all installed in a single directory $(libexec)/ecos/<package>_<version>/
## Neither the ecosynth executable nor any of the scripts are directly
## executable, instead ecosynth gets fork()'d/execve()'d by the eCos
## application so $(libexec) is appropriate. Strictly speaking the
## Tcl scripts and data files are architecture-independent so should
## probably be installed in an analogous directory below $(datadir),
## but that would add more directories for little real gain. The scripts
## are treated as data files since they should not be executed directly,
## i.e. they should not be installed with the execute bit set.

synthdir 		= $(libexecdir)/ecos/@PACKAGE_INSTALL@
synth_PROGRAMS		= ecosynth
synth_DATA		= ecosynth.tcl default.tdf user_initrc.tcl user_mainrc.tcl \
                          console.tcl \
                          ecosicon.xbm ecosiconmask.xbm tick_yes.xbm tick_no.xbm \
                          save.xbm cut.xbm copy.xbm paste.xbm help.xbm running1.ppm \
                          saveall.xbm

ecosynth_SOURCES	= ecosynth.c
ecosynth_LDADD		= @ecos_tk_libs@

## Manual dependencies
ecosynth.$(OBJEXT)	: Makefile ../src/synth_protocol.h

endif
