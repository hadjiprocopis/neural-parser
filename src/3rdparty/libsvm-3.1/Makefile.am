##              ARTIFICIAL NEURAL NETWORKS SOFTWARE
##   Neural Parser (np) :
##   environment for training and testing neural networks
##
##                        by Andreas Hadjiprocopis
##                  (livantes@soi.city.ac.uk)
##              Copyright Andreas Hadjiprocopis, 1994,2007

OPTIONS_AUTOMAKE=gnu
ACLOCAL_AMFLAGS=-I m4
CC=g++
CXX=g++
AM_CFLAGS=-fPIC

bin_PROGRAMS = svm-predict\
		svm-scale\
		svm-train

svm_predict_SOURCES = svm-predict.c svm-extensions.h svm.h
svm_train_SOURCES = svm-train.c svm-extensions.h svm.h
svm_scale_SOURCES = svm-scale.c svm.h

noinst_LIBRARIES = libsvmextensionsprivate.a
libsvmextensionsprivate_a_SOURCES=svm.cpp svm-extensions.h svm.h
libsvmextensionsprivate_a_LIBADD = svm.o svm-extensions.o

LDADD = libsvmextensionsprivate.a -lm 
#svm-predict_LDADD = -lm libsvmextensionsprivate.a
