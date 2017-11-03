##              ARTIFICIAL NEURAL NETWORKS SOFTWARE
##   Neural Parser (np) :
##   environment for training and testing neural networks
##
##                        by Andreas Hadjiprocopis
##                  (livantes@soi.city.ac.uk)
##              Copyright Andreas Hadjiprocopis, 1994,2007

OPTIONS_AUTOMAKE=gnu
ACLOCAL_AMFLAGS=-I m4
SUBDIRS=GridSearch

bin_SCRIPTS = svm-select-parameters.pl

all-recursive: GridSearch
GridSearch: make-perl-makefile
make-perl-makefile:
	cd GridSearch && $(PERLPATH) Makefile.PL
