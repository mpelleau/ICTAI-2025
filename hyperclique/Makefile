#************************************************************************************
# Copyright (c) 2020
# Last edit, March 2, 2020
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
# associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute,
# sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
# OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# **************************************************************************************************/

# compiler :
COMPILER = g++

COPTIONS  =  -std=c++11 #-Ihtd-git/include
COPTIONS +=  -Wall -Wextra -Wno-unused-parameter -fpermissive -O3

LPROFILAGE = -fprofile-arcs -ftest-coverage -fPIC -O0 #-Lhtd-git/lib -lpthread
COPTIONS_DEBUG = -pg -g -Wall -Wextra -Wno-unused-parameter -std=c++11 #-Ihtd-git/include $(LPROFILAGE)

LOPTIONS += #-lpthread #-Lhtd-git/lib 

UNAME_S := $(shell uname -s)

EXECUTABLE = ./hyperclique

EXEC_TEST = ./test

# path:
CODE = $(shell pwd)

# folder for the executable
EXEDIR = $(CODE)/bin

# adress of the source code, objects and include files:
SRCDIR = $(CODE)/src
TESTDIR = $(CODE)/test
OBJDIR = $(CODE)/obj

# create the list of file sources:
SRC = $(wildcard $(SRCDIR)/*.cc)
TEST = $(wildcard $(TESTDIR)/*.cc)

# list the files:
NOM = $(basename $(notdir $(SRC)))
TEST_NOM = $(basename $(notdir $(TEST)))

OBJ_DEBUG = $(addprefix $(OBJDIR)/, $(addsuffix .od, $(NOM)))

OBJ = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(NOM)))

OBJ_TEST = $(addprefix $(OBJDIR)/, $(addsuffix .ot, $(TEST_NOM)))

.PHONY: help

# link edition
all: faire_dossier $(OBJ) $(OBJDIR)/Main.o
	$(COMPILER) -o $(EXEDIR)/$(EXECUTABLE) -lz $(OBJ) $(OBJDIR)/Main.o $(LOPTIONS) -lhtd

debug: faire_dossier $(OBJ_DEBUG) $(OBJDIR)/Main.od
	$(COMPILER) -o $(EXEDIR)/$(EXECUTABLE) -lz $(OBJ_DEBUG) $(OBJDIR)/Main.od $(LPROFILAGE) -lhtd

test: faire_dossier $(OBJ_TEST) $(OBJ_DEBUG)
	$(COMPILER) -o $(EXEDIR)/$(EXEC_TEST) -fprofile-arcs -ftest-coverage -lz $(OBJ_DEBUG) $(OBJ_TEST) $(LOPTIONS) -lgtest -lgtest_main -lhtd

# rules to do the folders
faire_dossier: makedir
	@echo "folders created"

# compilation rules

$(OBJDIR)/Main.o: $(SRCDIR)/Main.cpp $(SRCDIR)/Main.h
	$(COMPILER) -c $(COPTIONS) -o $@ $< $(LOPTIONS)

$(OBJDIR)/Main.od: $(SRCDIR)/Main.cpp $(SRCDIR)/Main.h
	$(COMPILER) -c $(COPTIONS_DEBUG) -o $@ $<


$(OBJDIR)/%.o: $(SRCDIR)/%.cc $(SRCDIR)/%.h
	$(COMPILER) -c $(COPTIONS) -o $@ $< $(LOPTIONS)

$(OBJDIR)/%.od: $(SRCDIR)/%.cc $(SRCDIR)/%.h
		$(COMPILER) -c $(COPTIONS_DEBUG) -o $@ $<

$(OBJDIR)/%.ot: $(TESTDIR)/%.cc $(TESTDIR)/%.h
		$(COMPILER) -c $(COPTIONS) -fprofile-arcs -ftest-coverage -o $@ $<		

# -------------------------------------------------------------------
#  to create the objects folder
# -------------------------------------------------------------------

# creation of the folder $(OBJDIR) if it is necessary:
makedir:
	mkdir -p $(OBJDIR)
	mkdir -p $(EXEDIR)

# -------------------------------------------------------------------
#  cleaning rules
# -------------------------------------------------------------------

.PHONY: clean purge

clean:
	@rm -rf $(OBJDIR)
	@rm -rf $(EXEDIR)
	@rm -rf *~
	@rm -rf coverage


help:
	@echo "--------------------------------------------------------------------------"
	@echo "Rules available"
	@echo
	@echo " install : compilation and executable creation"
	@echo " clean   : remove the object files"
	@echo " purge   : remove the object files and the executable"
	@echo " help    : print this comments (defautl)"
	@echo "--------------------------------------------------------------------------"
	@echo
