#ifndef _ProdMatMat_hpp__
#define _ProdMatMat_hpp__
#include "Matrix.hpp"
#include <functional>

Matrix operator*(const Matrix &A, const Matrix &B);
Matrix prodBlocks(const Matrix &A, const Matrix &B, int szBlock);

enum prod_algo {
  naive,
  block,
  parallel_naive,
  parallel_block1,
  parallel_block2
};
void setProdMatMat(prod_algo algo);
void setBlockSize(int size);
void setNbThreads(int n);
#endif
