AUTOMAKE_OPTIONS = 1.4 foreign
AUTOTOOL_VERSION=autoconf-2.52 automake-1.5 libtool-1.4.2

doc_FILES = README.MSVC6 README.SDL COPYING.ZZIP COPYING.LIB COPYING.ZLIB \
	zziplib.html staticlink.txt
htm_FILES = zzip-index.htm zzip-zip.htm zzip-file.htm zzip-sdl-rwops.htm \
            zzip-extio.htm zzip-xor.htm zzip-api.htm  zzip-parse.htm \
            future.htm configs.htm sfx-make.htm history.htm referentials.htm  
SDL = @top_srcdir@/SDL
SDL_RWOPS = $(SDL)/SDL_rwops_zzcat.c \
            $(SDL)/SDL_rwops_zzip.c $(SDL)/SDL_rwops_zzip.h
changelog = @top_srcdir@/ChangeLog

EXTRA_DIST = zzip-doc.pl zzip-doc.php $(doc_FILES) $(htm_FILES) $(SDL_RWOPS) \
             make-doc.pl make-dbk.pl  manpages.ar htmpages.ar      body.htm
CLEANFILES = *.pc *.omf
DISTCLEANFILES = zziplib.spec 
MAINTAINERCLEANFILES = manpages.ar htmpages.ar *.html *.xml

default : doc
clean-doc clean-docs : clean-unpack
	- rm $(DISTCLEANFILES) 
	- rm $(MAINTAINERCLEANFILES)

# -------------------------------------------------------------------
zziplib.spec : @top_srcdir@/$(PACKAGE).spec
	@ $(LN_S) $? $@ # the two zzip-doc.* will grep thru zziplib.spec
doc  : $(doc_FILES) $(htm_FILES:.htm=.html)
docs : doc manpages.ar htmpages.ar
zziplib0.html : zziplib0.xml
zziplib0.xml : zziplib.spec @srcdir@/Makefile.am \
                 $(srcdir)/make-doc.pl @top_srcdir@/zzip/*.c
	- @PERL@ $(srcdir)/make-doc.pl @top_srcdir@/zzip/*.c \
          "suffix=0" "package=$(PACKAGE)" "version=$(VERSION)" \
          "mainheader=zzip/zzip.h" "exported=<zzip/zzip.h> // or <zziplib.h>" \
	  "docbookfile=$@" 
	- test -s $@
zziplib1.html : zziplib1.xml
zziplib1.xml : zziplib.spec @srcdir@/Makefile.am \
                 $(srcdir)/zzip-doc.pl @top_srcdir@/zzip/*.c
	- @PERL@ $(srcdir)/zzip-doc.pl @top_srcdir@/zzip/*.c
	- test -f zziplib1.docbook && mv zziplib1.docbook $@ ; test -s $@
# zziplib2.html : zziplib2.xml
# zziplib2.xml : zziplib.spec @srcdir@/Makefile.am \
#                    $(srcdir)/zzip-doc.php @top_srcdir@/zzip/*.c
# 	- @PHP@ -q $(srcdir)/zzip-doc.php @top_srcdir@/zzip/*.c
# 	- test -f zziplib2.docbook && mv zziplib2.docbook $@ ; test -s $@
zziplib.html :    zziplib1.html zziplib0.html
	- test -s zziplib1.html && cp zziplib1.html zziplib.html
	- test -s zziplib0.html && cp zziplib0.html zziplib.html
	test -s $@
zziplib.xml :     zziplib1.xml zziplib0.xml
	- test -s zziplib1.xml && cp zziplib1.xml zziplib.xml
	- test -s zziplib0.xml && cp zziplib0.xml zziplib.xml
	test -s $@ || rm $@

omfdir=${datadir}/omf
pkgomfdir=${omfdir}/${PACKAGE}
SFNETDIR=/groups/z/zz/zziplib/htdocs
SFNETDOC=${datadir}${SFNETDIR}
pkgdocdir=${datadir}${SFNETDIR}
bins = @top_srcdir@/bins
DOCEXAMPLES = $(bins)/zzdir.c $(bins)/zzcat.c \
	      $(bins)/zzobfuscated.c $(bins)/zziptest.c \
              $(bins)/zzxordir.c $(bins)/zzxorcat.c \
	      $(bins)/zzxorcopy.c        $(SDL_RWOPS)

install-docu: $(doc_FILES) $(htm_FILES:.htm=.html) $(PACKAGE)-doc.omf
	$(mkinstalldirs) $(DESTDIR)$(pkgdocdir)
	for i in $(htm_FILES) ; do cat $${i}l \
	| sed -e 's:--START-->:-- :' -e 's:<!--ENDS--: --:' \
	-e "s:\\(href=\"[^/\"]*\\)[.]html\":\\1.htm\":" \
	-e "s:\\(href=\"[^/\"]*\\)[.]html\#:\\1.htm\#:" \
	> $(DESTDIR)$(pkgdocdir)/$$i ; done
	@ for i in $(doc_FILES) $(DOCEXAMPLES) $(srcdir)/README.* \
	; do $(INSTALL_DATA) `test -f $$i || echo $(srcdir)/`$$i  \
	  $(DESTDIR)$(pkgdocdir) ; done
	(cd $(DESTDIR)$(pkgdocdir) && ln -sf zziplib.html zziplib.htm)
	$(mkinstalldirs) $(DESTDIR)$(pkgomfdir)
	$(INSTALL_DATA) $(PACKAGE)-doc.omf $(DESTDIR)$(pkgomfdir)/
	- test ".$(DESTDIR)" != "." || scrollkeeper-update

install-sfnet: $(doc_FILES) $(htm_FILES:.htm=.html) htmpages.ar
	$(mkinstalldirs) $(DESTDIR)$(SFNETDOC)
	$(INSTALL_DATA) $(htm_FILES:.htm=.html) $(DESTDIR)$(SFNETDOC)
	for i in $(doc_FILES) $(DOCEXAMPLES) $(changelog) $(srcdir)/README.* \
	; do $(INSTALL_DATA) `test -f $$i || echo $(srcdir)/`$$i \
	  $(DESTDIR)$(SFNETDOC) ; done
	(cd $(DESTDIR)$(SFNETDOC) && ln -sf zzip-index.html index.html)
	$(mkinstalldirs) $(DESTDIR)$(SFNETDOC)/man
	(test -f htmpages.ar || cd "$srcdir" ; P=`pwd` \
	; cd $(DESTDIR)$(SFNETDOC)/man && ar x $$P/htmpages.ar)

install-doc : install-docu install-sfnet

# --------------------------------------------------------------------
man manpages : manpages.ar
htm htmpages : htmpages.ar

manpages.ar : zziplib.xml
	@ if test "$(XMLTO)" = ":" \
	; then echo no xmlto found - could not regenerate "$@" - skipped \
	; echo just touching the output file "$@" ; touch "$@" \
	; else echo going to regenerate "$@" in subdir "'"man"'" \
	; echo 'test ! -d man || rm man/* ; test -d man || mkdir man' \
	;       test ! -d man || rm man/* ; test -d man || mkdir man  \
	; echo '(cd man && xmlto man ../zziplib.xml)' \
	;       (cd man && xmlto man ../zziplib.xml)  \
	; echo 'test ! -f $@ || mv $@ $@.old' \
	;       test ! -f $@ || mv $@ $@.old  \
	; echo '(cd man && ar qc ../$@ *.3)' \
	;       (cd man && ar qc ../$@ *.3)  \
	; echo '(rm man/*.3 ; rmdir man)' \
	;       (rm man/*.3 ; rmdir man)  \
	; fi ; test -s $@ || mv $@.old $@

htmpages.ar : zziplib.xml
	@ if test "$(XMLTO)" = ":" \
	; then echo no xmlto found - could not regenerate "$@" - skipped \
	; echo just touching the output file "$@" ; touch "$@" \
	; else echo going to regenerate "$@" in subdir "'"man"'" \
	; echo 'test ! -d man || rm /* ; test -d man || mkdir man' \
	;       test ! -d man || rm man/* ; test -d man || mkdir man  \
	; echo '(cd man && xmlto html ../zziplib.xml | tee written.lst)' \
	;       (cd man && xmlto html ../zziplib.xml | tee written.lst)  \
	; echo 'test ! -f $@ || mv $@ $@.old' \
	;       test ! -f $@ || mv $@ $@.old  \
	; echo '(cd man && ar qc ../$@ *.*)' \
	;       (cd man && ar qc ../$@ *.*)  \
	; echo '(rm man/*.* ; rmdir man)' \
	;       (rm man/*.* ; rmdir man)  \
	; fi ; test -s $@ || mv $@.old $@

install-man3 : manpages.ar
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man3
	P=`pwd` ; cd $(DESTDIR)$(mandir)/man3 \
        && ar x $$P/manpages.ar && chmod 664 *

unpack : manpages.ar htmpages.ar
	(rm -rf _htm && mkdir _htm && cd _htm && ar x ../htmpages.ar)
	(rm -rf _man && mkdir _man && cd _man && ar x ../manpages.ar)
clean-unpack : 
	rm -rf _htm
	rm -rf _man

# -------------------------------------------------------------------
spec_file=$(top_srcdir)/$(PACKAGE).spec
DOCSERIES= 775fb73e-1874-11d7-93e9-e18cd7ea3c2e
FROMSPEC= $(spec_file) | head -1 | sed -e 's,<,\&lt;,g' -e 's,>,\&gt;,g'
DATESPEC= `date +%Y-%m-%d`

$(PACKAGE)-doc.omf : $(spec_file) Makefile
	echo '<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>' >$@
	echo '<omf><resource><creator> Guido Draheim </creator>' >>$@
	grep Packager $(FROMSPEC) | sed -e 's,Packager *: *, <maintainer>,' \
                                    -e '/<maintainer>/s,$$,</maintainer>,' >>$@
	grep Summary  $(FROMSPEC) | sed -e 's,Summary *: *, <title>,' \
		       -e '/<title>/s,$$,</title>,' >>$@
	echo ' <date>'$(DATESPEC)'</date>' >>$@
	echo ' <version identifier="$(VERSION)" date="'$(DATESPEC)'"/>' >>$@
	grep Group  $(FROMSPEC) | sed -e 's,Group *: *, <subject category=",' \
	                -e 's,/,|,g' -e '/<subject/s,$$," />,' >>$@
	echo ' <format mime="text/html"/>' >>$@
	echo ' <identifier url="file:$(pkgdocdir)/zzip-index.htm"/>' >>$@
	echo ' <language code="C"/>' >>$@
	echo ' <relation seriesid="$(DOCSERIES)"/>' >>$@
	echo ' <rights type="GNU LGPL" holder="Guido Draheim"' >>$@
	echo '         license="$(pkgdocdir)/COPYING.LIB"/>' >>$@
	echo '</resource></omf>' >>$@

DOCBOOKDTD= -//OASIS/DTD Docbook V4.1.2//EN
MANSERIES= a302c642-1888-11d7-86f6-ba4b52ef847d
$(PACKAGE)-man.omf : $(PACKAGE)-doc.omf $(PACKAGE).xml
	sed -e 's,"text/html","text/xml" dtd="$(DOCBOOKDTD)",' \
	    -e 's,</title>, (Function Reference)</title>,' \
	    -e 's,/index.html,/manpages.xml,' \
	    -e 's,$(DOCSERIES),$(MANSERIES),' $(PACKAGE)-doc.omf > $@
	test -s $@ || rm $@

omf : $(PACKAGE)-doc.omf $(PACKAGE)-man.omf $(PACKAGE).xml
install-omf : $(PACKAGE)-doc.omf $(PACKAGE)-man.omf $(PACKAGE).xml
	$(mkinstalldirs) $(DESTDIR)$(pkgomfdir)
	$(INSTALL_DATA) $(PACKAGE)-doc.omf $(DESTDIR)$(pkgomfdir)/
	$(mkinstalldirs) $(DESTDIR)$(pkgdocdir)
	$(INSTALL_DATA) $(PACKAGE).xml $(DESTDIR)$(pkgdocdir)/manpages.xml
	$(INSTALL_DATA) $(PACKAGE)-man.omf $(DESTDIR)$(pkgomfdir)/
	- test ".$(DESTDIR)" != "." || scrollkeeper-update -v

# -------------------------------------------------------------------
datehead = <hr><table border="0" width="100%"><tr><td> \
    <a href="mailto:guidod@gmx.de?subject=$(PACKAGE):">guidod@gmx.de</a> \
    </td><td align="right">
datetail = </td></tr></table>
borderhead = <table cellpadding="10"><tr><td>
bordertail = </td></tr></table>

x : $(htm_FILES:.htm=.html)
$(htm_FILES:.htm=.html) : body.htm
%.html : %.htm
	@ title=`grep '<h2>' $< | sed -e 's:</h2>.*::' -e 's:<[^<>]*>::g'` \
	; D=$(DATESPEC) ; cat body.htm \
	| sed -e "s/<!--VERSION-->/$(VERSION)/" \
	| sed -e "s/<!--DATE-->/$$D/" \
	| sed -e "s:</title>: - $$title</title>:" \
	| sed -e "/$@.>/s:\\([^<>]*\\)</a>:<b>\\1</b></a>:" \
	> $@
	@ echo $< '>>' $@; cat $< | sed -e "s/<!--VERSION-->/$(VERSION)/" \
	-e "s:</h2>\\(.*\\):</h2></td><td><i>\\1</i></td></tr></table>:" \
	-e 's:<h2>:<table width="100%"><tr valign="bottom"><td><h2>:' \
	-e 's:<td><i>:<td align="right"><br> <i>:' \
	-e 's:<P>:<p class="justify"><span class="justify">:g' \
	-e 's:</P>:</span></p>:g' \
	-e 's,<date>,<!--DATE: ,' -e 's,</date>, -->,' \
	-e 's,<!--border-->,$(borderhead)<!--BORDER-->,' >>$@
	@ if grep '<!--BORDER-->' $@ >/dev/null ; then : \
	; echo "$(bordertail)<!--/BORDER-->" >>$@ ; fi
	@ D=`grep '<!--DATE:' $@ | sed -e 's,.*<!--DATE: *,,' -e 's, *-->,,'` \
	; if test ".$$D" != "." ; then echo '$(datehead)' >>$@ \
	; echo "$$D" >> $@ ; echo "$(datetail)" >> $@ ;	fi
	@ echo "</td></tr></table></body></html>" >>$@


zzip.xml : $(htm_FILES) zziplib.xml make-dbk.pl
	@PERL@ make-dbk.pl $(htm_FILES) zziplib.xml >$@
	test -s "$@" || rm "$@"

zzip.html : zzip.xml
	xmlto html-nochunks zzip.xml
zzip.pdf  : zzip.xml
	xmlto pdf zzip.xml
