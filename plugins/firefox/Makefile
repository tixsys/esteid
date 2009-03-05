SUBDIRS := chrome components
XPI_FILENAME := esteid.xpi

.PHONY: all clean subdirs ${SUBDIRS}

all: subdirs ${XPI_FILENAME}

${XPI_FILENAME}: subdirs install.rdf
	rm -f $@
	zip -p $@ chrome/*.jar components/esteid.{so,xpt} install.rdf

subdirs: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@

clean:
	for DIR in ${SUBDIRS}; do \
		${MAKE} -C $$DIR $@; \
	done
