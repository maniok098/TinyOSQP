#ifndef ALGEBRA_IMPL_H
#define ALGEBRA_IMPL_H

#include "csc_math.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************
*   Internal definition of OSQPVector types
*   and supporting definitions
*********************************************/

typedef struct OSQPVectori_ {
  OSQPInt* values;
  OSQPInt  length;
}OSQPVectori_;

typedef struct OSQPVectorf_ {
  OSQPFloat* values;
  OSQPInt    length;
}OSQPVectorf_;


/*********************************************
*   Internal definition of OSQPMatrix type
*   and supporting definitions
*********************************************/

/**
 *  An enum used to indicate whether a matrix is symmetric.   Options
 *  NONE : matrix is fully populated
 *  TRIU : matrix is symmetric and only upper triangle is stored
 */
typedef enum OSQPMatrix_symmetry_type {NONE,TRIU} OSQPMatrix_symmetry_type;

typedef struct OSQPMatrix_ {
  OSQPCscMatrix*           csc;
  OSQPMatrix_symmetry_type symmetry;
}OSQPMatrix_;

#ifdef __cplusplus
}
#endif

#endif /* ifndef ALGEBRA_IMPL_H */
