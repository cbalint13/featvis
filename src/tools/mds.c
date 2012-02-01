// Multi Dimensional Scaling (MDS)
//
//           C implementation by cristian.balint@gmail.com
//

#include "math.h"

#include "featvis.h"

gsl_matrix *distMDS( gsl_matrix *D, int dim ) {

  int i,j;
  double sum;

  eig *Eig;
  gsl_matrix *E;
  gsl_vector *V;

  int n = D->size1;    // rows
  int m = D->size2;    // collumn

  printf("           steps: power two matrix\n");
  // rise matrix to power 2
  for (i = 0; i < n*m; i++) {
      D->data[i] = D->data[i] * D->data[i];
  }

  printf("           steps: center over cols\n");
  // center matrix on X
  for(i=0 ; i<n ; i++) {
      sum = 0;
      for(j=0 ; j<n ; j++)
          sum += D->data[i+j*n];
      sum /= n;
      for(j=0 ; j<n ; j++)
          D->data[i+j*n] -= sum;
  }
  printf("           steps: center over rows\n");
  // center matrix on Y
  for(j=0 ; j<n ; j++) {
      sum = 0;
      for(i=0 ; i<n ; i++)
          sum += D->data[i+j*n];
      sum /= n;
      for(i=0 ; i<n ; i++)
          D->data[i+j*n] -= sum;
  }

  printf("           steps: half divide\n");
  // divide matrix with negative 2
  for (i = 0; i < n*m; i++) {
      D->data[i] = D->data[i] / -2.0f;
  }

  printf("           steps: compute eigen\n");
  // compute dim eigens
  Eig = eigen(D , dim);
  E = Eig->eigvec;
  V = Eig->eigval;

  printf("           steps: square mean back\n");
  // add square mean back
  for (i = 0; i < E->size1; i++) {
      for (j = 0; j < E->size2; j++) {
          E->data[i*E->size2+j] = E->data[i*E->size2+j] * sqrt(gsl_vector_get(V,j));
      }
  }

  return E;

}