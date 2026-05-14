#ifndef OSQP_CONFIGURE_H
# define OSQP_CONFIGURE_H

// Define this flag to disable runtime rho adaption in embedded mode if matrix update is not needed
// #define OSQP_DISABLE_RHO_ADAPTION

/* Enble derivative computation in the solver */
#define OSQP_ENABLE_DERIVATIVES 1

#endif /* ifndef OSQP_CONFIGURE_H */
