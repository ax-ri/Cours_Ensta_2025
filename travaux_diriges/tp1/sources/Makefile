include Make_linux.inc
#include Make_msys2.inc
#include Make_osx.inc

CXXFLAGS = -std=c++14
ifdef DEBUG
CXXFLAGS += -g -O0 -Wall -fbounds-check -pedantic -D_GLIBCXX_DEBUG
else
CXXFLAGS += -O3 -march=native -Wall
endif

ALL= calcul_pi.exe TestProductMatrix.exe test_product_matrice_blas.exe hypercube.exe jeton.exe

default:	help

all: $(ALL)

clean:
	@rm -fr *.o *.exe *~

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $^ -o $@

calcul_pi.exe: calcul_pi.cpp
	$(MPICXX) $(CXXFLAGS) $^ -o $@

jeton.exe: jeton.cpp
	$(MPICXX) $(CXXFLAGS) $^ -o $@

hypercube.exe: hypercube.cpp
	$(MPICXX) $(CXXFLAGS) $^ -o $@

TestProductMatrix.exe : TestProductMatrix.o Matrix.hpp Matrix.o ProdMatMat.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIB)

test_product_matrice_blas.exe : test_product_matrice_blas.o Matrix.hpp Matrix.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIB)	$(BLAS)

help:
	@echo "Available targets : "
	@echo "    all            : compile all executables"
	@echo "Add DEBUG=yes to compile in debug"
	@echo "Configuration :"
	@echo "    CXX      :    $(CXX)"
	@echo "    CXXFLAGS :    $(CXXFLAGS)"

%.html: %.md
	pandoc -s --toc $< --css=./github-pandoc.css --metadata pagetitle="OS202 - TD1" -o $@
