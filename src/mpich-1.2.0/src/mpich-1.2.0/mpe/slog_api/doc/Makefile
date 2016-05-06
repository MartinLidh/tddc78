#  Output Directory

MAN_DIR  = ./man/man3
HTML_DIR = ./html

#  Input Directory

SRC_DIR  = ../src

.SUFFIXES:	.c .txt

#  Name of the sources
API_SRC = slog_header.c slog_pstat.c slog_preview.c slog_profile.c \
          slog_ttable.c slog_recdefs.c \
          fbuf.c slog_impl.c \
          slog_irec_common.c slog_irec_write.c slog_irec_read.c
INC_TXT = slog_return_status.txt slog_recdefs_extra.txt slog_profile_extra.txt

#  The following statements require GNU make.
REAL_SRC = $(API_SRC:%.c=$(SRC_DIR)/%.c)
REAL_TXT = $(INC_TXT:%.txt=$(SRC_DIR)/%.txt)

#  The utilities property
DOCTEXT = ./doctext
DOC_OPT = -heading SLOG_API

all:	man html

man:	FORCE 
	if [ ! -d $(MAN_DIR) ] ; then \
		mkdir -p $(MAN_DIR) ; fi
	$(DOCTEXT) -mpath $(MAN_DIR) $(DOC_OPT) $(REAL_TXT) $(REAL_SRC)
FORCE:

html:	FORCE
	if [ ! -d $(HTML_DIR) ] ; then \
		mkdir -p $(HTML_DIR) ; fi
	$(DOCTEXT) -html -mpath $(HTML_DIR) $(DOC_OPT) \
	           -index $(HTML_DIR)/libslog.cit -indexdir $(HTML_DIR) \
	           $(REAL_TXT) $(REAL_SRC)
FORCE:

clean:
	rm -f $(MAN_DIR)/* $(HTML_DIR)/*
