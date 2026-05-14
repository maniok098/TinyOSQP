
#include "osqp.h"
#include "util.h"
#include "algebra_vector.h"
#include "version.h"
#include "printing.h"
#include "lin_alg.h"
#define new new_

/************************************
* Printing Constants to set Layout *
************************************/

/**********************
* Utility Functions  *
**********************/
void c_strcpy(char dest[], const char source[]) {
  int i = 0;

  while (1) {
    dest[i] = source[i];

    if (dest[i] == '\0') break;
    i++;
  }
}


#ifndef OSQP_EMBEDDED_MODE

OSQPSettings* copy_settings(const OSQPSettings *settings) {

  OSQPSettings *new = (OSQPSettings*)c_malloc(sizeof(OSQPSettings));
  if (!new) return OSQP_NULL;

  /* Copy settings
   * NB: Copying them explicitly because memcpy is not
   * defined when OSQP_ENABLE_PRINTING is disabled (appears in string.h)
   */
  new->device        = settings->device;
  new->linsys_solver = settings->linsys_solver;
  new->verbose       = settings->verbose;
  new->warm_starting = settings->warm_starting;
  new->scaling       = settings->scaling;
  new->polishing     = settings->polishing;

  new->rho        = settings->rho;
  new->rho_is_vec = settings->rho_is_vec;
  new->sigma      = settings->sigma;
  new->alpha      = settings->alpha;

  new->cg_max_iter      = settings->cg_max_iter;
  new->cg_tol_reduction = settings->cg_tol_reduction;
  new->cg_tol_fraction  = settings->cg_tol_fraction;
  new->cg_precond       = settings->cg_precond;

  new->adaptive_rho           = settings->adaptive_rho;
  new->adaptive_rho_interval  = settings->adaptive_rho_interval;
  new->adaptive_rho_fraction  = settings->adaptive_rho_fraction;
  new->adaptive_rho_tolerance = settings->adaptive_rho_tolerance;

  new->max_iter           = settings->max_iter;
  new->eps_abs            = settings->eps_abs;
  new->eps_rel            = settings->eps_rel;
  new->eps_prim_inf       = settings->eps_prim_inf;
  new->eps_dual_inf       = settings->eps_dual_inf;
  new->scaled_termination = settings->scaled_termination;
  new->check_termination  = settings->check_termination;
  new->time_limit         = settings->time_limit;

  new->delta              = settings->delta;
  new->polish_refine_iter = settings->polish_refine_iter;

  return new;
}

#endif /* ifndef OSQP_EMBEDDED_MODE */
