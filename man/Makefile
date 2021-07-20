#
# Build all files in this directory that have a .md suffix
#
# Note: uses pandoc
#
# type "make" to build pdf
# type "make clean" to remove preliminary files and such
#

PANDOC= pandoc
SRC := $(wildcard *.md)
SRC := $(filter-out README.md, $(SRC))
DST= $(SRC:.md=.1)

all : $(DST)

%.1 : %.md
	echo $(SRC)
	$(PANDOC) --standalone --self-contained -t man -o $@ $<

clean:
	/bin/rm -f $(DST) 2> /dev/null *.log

# end of Makefile
