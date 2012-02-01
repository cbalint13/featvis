#include "featvis.h"

#include "gsl/gsl_matrix.h"
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>


gsl_matrix *cov(gsl_matrix *X) {

  int i, j, j1, j2;

  int n = X->size1;    // rows
  int m = X->size2;    // collumn
  double mean[m];

  // output covariance matrix
  gsl_matrix * C = gsl_matrix_alloc(m, m);

  // workspace a copy matrix
  gsl_matrix * W  = gsl_matrix_alloc(n, m);
  gsl_matrix_memcpy(W, X);


/* Determine mean of column vectors of input data matrix */

for (j = 0; j < m; j++)
    {
    mean[j] = 0.0;
    for (i = 0; i < n; i++)
        {
          mean[j] += gsl_matrix_get(X,i,j); // X.dm[i][j];
        }
      mean[j] /= (double) n;
    }

   if (debug) {
       printf("\nMeans of column vectors:\n");
       for (j = 0; j < m; j++)  {
       printf("%7.4f ",mean[j]);  }   
       printf("\n\n");
   }

/* Center the column vectors. */

for (i = 0; i < n; i++)
    {
    for (j = 0; j < m; j++)
        {
          W->data[i*(W->tda)+j] -= mean[j];
        }
    }
/* Calculate the m * m covariance matrix. */
for (j1 = 0; j1 < m; j1++)
    {
    for (j2 = j1; j2 < m; j2++)
        {
        C->data[j1*(C->tda)+j2] = 0.0;
        for (i = 0; i < n; i++)
            {
            C->data[j1*(W->tda)+j2] += (W->data[i*(W->tda)+j1] * W->data[i*(W->tda)+j2]) / (n-1);
            }
        C->data[j2*(W->tda)+j1] = C->data[j1*(W->tda)+j2];
        }
    }

    if (debug) {
    printf("Covariance matrix [%i][%i]:\n",m,m);

    // Output variance-covariance matrix.
    for (i = 0; i < m; i++) {
      for (j = 0; j < m; j++)  {
        printf("%5.4f ", C->data[i*(C->tda)+j]);  }
        printf("\n");  }
    }


    gsl_matrix_free (W);

  return C;

}

gsl_matrix *eigens(gsl_matrix *X, int npca) {

  //int n = X->size1;    // rows
  int m = X->size2;    // collumn

  // alloc eigen vectors matrix
  gsl_matrix * E = gsl_matrix_alloc (m, npca);

  // alloc vector space
  gsl_vector *eval = gsl_vector_alloc (m);
  // alloc eigen vectors space
  gsl_matrix *evec = gsl_matrix_alloc (m, m);
  // allocate workspace
  gsl_eigen_symmv_workspace * W = gsl_eigen_symmv_alloc (m);
  // compute eigen vectors
  gsl_eigen_symmv (X, eval, evec, W);
  // free workspace
  gsl_eigen_symmv_free (W);
  // sort bigest eigen vectors
  gsl_eigen_symmv_sort (eval, evec, GSL_EIGEN_SORT_ABS_DESC);
  {
    
    int i;
    // keep only npca eigen vectors
    for (i = 0; i < npca; i++) {

        double eval_i = gsl_vector_get (eval, i);
        // copy eigen vector
        gsl_vector_view evec_i = gsl_matrix_column (evec, i);
        // write eigen vector into eigen matrix [m*ndim]
	gsl_matrix_set_col(E, i, &evec_i.vector);
	
        if (debug) {
          printf ("eigenvalue = %g\n", eval_i);
          printf ("eigenvector = \n");
          gsl_vector_fprintf (stdout, &evec_i.vector, "%g");
	}
    }
  }
     
    gsl_vector_free (eval);
    gsl_matrix_free (evec);
     
    return E;

}

gsl_matrix *min(gsl_matrix *X) {

  int i;
  double min;

  //int n = X->size1;    // rows
  int m = X->size2;    // collumn

  // alloc output min matrix
  gsl_matrix * M = gsl_matrix_alloc(1, m);

  // iterate through column, get min value.
  for (i = 0; i < m; i++) {
    // get matrix column into vector
    gsl_vector_view col = gsl_matrix_column (X, i);
    // store min value in matrix
    min = gsl_vector_min (&col.vector) - EPS;
    gsl_matrix_set (M, 0, i, min);
  }

  return M;

}

gsl_matrix *max(gsl_matrix *X) {

  int i;
  double max;

  //int n = X->size1;    // rows
  int m = X->size2;    // collumn

  // alloc output min matrix
  gsl_matrix * M = gsl_matrix_alloc (1, m);

  // iterate through column, get min value.
  for (i = 0; i < m; i++) {
    // get matrix column into vector
    gsl_vector_view col = gsl_matrix_column (X, i);
    // store max value in matrix
    max = gsl_vector_max (&col.vector) + EPS;
    gsl_matrix_set (M, 0, i, max);
  }

  return M;

}

eig *eigen(gsl_matrix *X, int npca) {

  eig *E;

  //int n = X->size1;    // rows
  int m = X->size2;    // collumn

  // alloc eigen vectors matrix
  gsl_matrix * Ve = gsl_matrix_alloc (m, npca);
  gsl_vector * Vl = gsl_vector_alloc (m);

  // alloc vector space
  gsl_vector *eval = gsl_vector_alloc (m);
  // alloc eigen vectors space
  gsl_matrix *evec = gsl_matrix_alloc (m, m);
  // allocate workspace
  gsl_eigen_symmv_workspace * W = gsl_eigen_symmv_alloc (m);
  // compute eigen vectors
  gsl_eigen_symmv (X, eval, evec, W);
  // free workspace
  gsl_eigen_symmv_free (W);
  // sort bigest eigen vectors
  gsl_eigen_symmv_sort (eval, evec, GSL_EIGEN_SORT_VAL_DESC);
  {
    
    int i;
    // keep only npca eigen vectors
    for (i = 0; i < npca; i++) {

        double eval_i = gsl_vector_get (eval, i);
        gsl_vector_set (Vl, i, eval_i);
        // copy eigen vector
        gsl_vector_view evec_i = gsl_matrix_column (evec, i);
        // write eigen vector into eigen matrix [m*ndim]
	gsl_matrix_set_col(Ve, i, &evec_i.vector);
	
        if (debug) {
          printf ("eigenvalue = %g\n", eval_i);
          printf ("eigenvector = \n");
          gsl_vector_fprintf (stdout, &evec_i.vector, "%g");
	}
    }
  }
     
    gsl_vector_free (eval);
    gsl_matrix_free (evec);

    // alocate storage for eig pointer
    E = malloc(sizeof(eig));

    E->eigval = Vl;
    E->eigvec = Ve;
    
    return E;

}
