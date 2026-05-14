#include <iostream>
#include "osqp.h"

int main() {

    // Step1 : Init workspace and problem data
    //  This part requires CSC format matrices for P and A, 
	// note that you mus take only the upper triangular part of P -> csc_matrix(np.triu(P))

	// number of optimization variables 
	static const OSQPInt m_nVars = 2;
	// number of constraints 
	static const OSQPInt m_nCons = 3;
	// CSC representation of P matrix 
	OSQPCscMatrix* m_P;
	// Notice: Only take the upper triangular representation, i.e.  P = numpy.triu(P) 
	OSQPFloat m_P_x[3] = {4.0, 1.0, 2.0, };
	OSQPInt m_P_nnz = 3;
	OSQPInt m_P_i[3] = {0, 0, 1, };
	OSQPInt m_P_p[3] = {0, 1, 3, };

	// CSC representation of A matrix 
	OSQPCscMatrix* m_A;
	// Notice: All elements of A are stored 
	OSQPFloat m_A_x[4] = {1.0, 1.0, 1.0, 1.0, };
	OSQPInt m_A_nnz = 4;
	OSQPInt m_A_i[4] = {0, 1, 0, 2, };
	OSQPInt m_A_p[3] = {0, 2, 4, };
	// dense vector q 
	OSQPFloat m_q[2] = {1.0, 1.0, };
	// dense lower bound vector lb 
	OSQPFloat m_lb[3] = {1.0, 0.0, 0.0, };
	// dense upper bound vector ub 
	OSQPFloat m_ub[3] = {1.0, 0.7, 0.7, };
	// Exitflag  
	OSQPInt m_exitflag;
	// Solver, settings, matrices  
	OSQPSolver* m_solver;
	OSQPSettings* m_osqp_settings;


    // Step 2: Init memory for solver
    // // This part should be done for example from preop to safeop, and requires malloc 
	// set up OSQP matrices
	m_P = (OSQPCscMatrix*)malloc(sizeof(OSQPCscMatrix));
	m_A = (OSQPCscMatrix*)malloc(sizeof(OSQPCscMatrix));
	// set matrices with CSC format
	csc_set_data(m_P, m_nVars, m_nVars, m_P_nnz, m_P_x, m_P_i, m_P_p);
	csc_set_data(m_A, m_nCons, m_nVars, m_A_nnz, m_A_x, m_A_i, m_A_p);

	/* Set default settings */
	m_osqp_settings = (OSQPSettings*)malloc(sizeof(OSQPSettings));
	/* // see https://osqp.org/docs/interfaces/solver_settings.html // */
	if (m_osqp_settings) {
		osqp_set_default_settings(m_osqp_settings);
		m_osqp_settings->polishing = 1;
		m_osqp_settings->polish_refine_iter = 3;
		m_osqp_settings->scaled_termination = 0;
		m_osqp_settings->scaling = 10;
		m_osqp_settings->max_iter = 50;
	}

	/* Setup solver */
	m_exitflag = osqp_setup(&m_solver, m_P, m_q, m_A, m_lb, m_ub, m_nCons, m_nVars, m_osqp_settings);


	// Step 3: Solve problem
	// init solve as warm start
	if (!m_exitflag) {
		m_exitflag = osqp_solve(m_solver);
	}

	// Print solution
	std::cout << " First run "  << std::endl;
	for (OSQPInt i = 0; i < m_nVars; i++) {
		std::cout << "x[" << i << "] = " << m_solver->solution->x[i] << std::endl;
	}

	// Example: Update vector
	m_q[0] = 1.0;
	m_q[1] = 1.5;
	m_exitflag = osqp_update_data_vec(m_solver, m_q, m_lb, m_ub);
	// Solve problem again
	m_exitflag = osqp_solve(m_solver);
	// Print solution
	std::cout << " Update vector "  << std::endl;
	for (OSQPInt i = 0; i < m_nVars; i++) {
		std::cout << "x[" << i << "] = " << m_solver->solution->x[i] << std::endl;
	}

	// Example: Update matrix
	// See https://github.com/orgs/osqp/discussions/704#discussioncomment-12217356
	// Using OSQP_NULL to force full update
	// Do not use original indices, this will lead to segfaults
	m_P_x[0] = 1.5;
	m_P_x[1] = 1.0;
	m_P_x[2] = 5.5;
	osqp_update_data_mat(m_solver, m_P_x, OSQP_NULL, m_P_nnz, m_A_x,
									OSQP_NULL, m_A_nnz);

	osqp_cold_start(m_solver);
	osqp_solve(m_solver);

	std::cout << " Update matrix P "  << std::endl;
	for (OSQPInt i = 0; i < m_nVars; i++) {
		std::cout << "x[" << i << "] = " << m_solver->solution->x[i] << std::endl;
	}


	m_P_x[0] = 4.0;
	m_P_x[1] = 1.0;
	m_P_x[2] = 2.0;
	osqp_update_data_mat(m_solver, m_P_x, OSQP_NULL, m_P_nnz, m_A_x,
									OSQP_NULL, m_A_nnz);
	m_q[0] = 1.0;
	m_q[1] = 1.0;
	m_exitflag = osqp_update_data_vec(m_solver, m_q, m_lb, m_ub);
						
	osqp_solve(m_solver);

	std::cout << " Update matrix P "  << std::endl;
	for (OSQPInt i = 0; i < m_nVars; i++) {
		std::cout << "x[" << i << "] = " << m_solver->solution->x[i] << std::endl;
	}



	m_A_x[0] = 1.2;
	osqp_update_data_mat(m_solver, m_P_x, OSQP_NULL, m_P_nnz, m_A_x,
									OSQP_NULL, m_A_nnz);
	osqp_solve(m_solver);

	std::cout << " Update matrix A "  << std::endl;
	for (OSQPInt i = 0; i < m_nVars; i++) {
		std::cout << "x[" << i << "] = " << m_solver->solution->x[i] << std::endl;
	}

    /* Cleanup (important!) */
    osqp_cleanup(m_solver);
    free(m_osqp_settings);
    free(m_P);
    free(m_A);

    return 0;
}
