# Overall Dos9 suite Makefile, The Dos9 Project (c) Darkbatcher
# 2010-2016
#
# This is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
#
# This files generates translations while compiling
#

PO_BUILD=$$(ls po/)
SHARE_DIR=./share/locale

MSGFMT = msgfmt
XGETTEXT = xgettext
MSGMERGE = msgmerge

all: m-po

m-po: po-tree
	mkdir -p $(SHARE_DIR)
	for i in $(PO_BUILD); do \
		echo "Building GETTEXT translation for $$i"; \
		echo " - Dos9-errors.po"; \
		$(MSGFMT) -c -v -o "$(SHARE_DIR)/$$i/LC_MESSAGES/Dos9-errors.mo" po/$$i/Dos9-errors.po ;\
		echo " - Dos9-msg.po"; \
		$(MSGFMT) -c -v -o "$(SHARE_DIR)/$$i/LC_MESSAGES/Dos9-msg.mo" po/$$i/Dos9-msg.po ;\
		echo " - Dos9-hlp.po"; \
		$(MSGFMT) -c -v -o "$(SHARE_DIR)/$$i/LC_MESSAGES/Dos9-hlp.mo" po/$$i/Dos9-hlp.po ;\
	done;

po-tree:
	for i in $(PO_BUILD); do \
		mkdir -p $(SHARE_DIR)/$$i ; \
		mkdir -p $(SHARE_DIR)/$$i/LC_MESSAGES ; \
	done;


# this target is for developers that want to merge po when they updated sources
po-merge: m-pot
	for i  in $(PO_BUILD); do \
		$(MSGMERGE) -U "po/$$i/Dos9-msg.po" "pot/Dos9-msg.pot" ; \
		$(MSGMERGE) -U "po/$$i/Dos9-errors.po" "pot/Dos9-errors.pot" ; \
		$(MSGMERGE) -U "po/$$i/Dos9-hlp.po" "pot/Dos9-hlp.pot" ; \
	done;

m-pot: ../dos9/lang/Dos9_Lang.c ../dos9/lang/Dos9_ShowHelp.c ../dos9/errors/Dos9_Errors.c
	mkdir -p pot
	$(XGETTEXT) -d Dos9-errors -o pot/Dos9-errors.pot "../dos9/errors/Dos9_Errors.c" -c="//"
	$(XGETTEXT) -d Dos9-msg -o pot/Dos9-msg.pot "../dos9/lang/Dos9_Lang.c" -c="//"
	$(XGETTEXT) -d Dos9-hlp -o pot/Dos9-hlp.pot "../dos9/lang/Dos9_ShowHelp.c" -c="//"

clean: