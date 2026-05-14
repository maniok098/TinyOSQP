# TinyOSQP
Minimal OSQP integration for MATLAB/Simulink with CMake-based build system.


## HowTos

1. Setup & Memory Allocation
    - `osqp_setup()` need allocated memory
    - This can be done in initialization (e.g. Pre-Op -> Safe-Op) 
2. CSC Matrix Format
    - `P` matrix must be symmetric and only upper triangular part is allowed
        ```cpp
        P_sparse = csc_matrix(np.triu(P)) # correct
        P_sparse = csc_matrix(P) # wrong
        ```
3. Solver settings: Enable polishing and set maxIter to 50 or 100
    ```cpp
    	if (m_osqp_settings) {
		osqp_set_default_settings(m_osqp_settings);
		m_osqp_settings->polishing = 1;
		m_osqp_settings->polish_refine_iter = 3;
		m_osqp_settings->scaled_termination = 0;
		m_osqp_settings->scaling = 10;
		m_osqp_settings->max_iter = 50;
	    }
    ```
    - See `section 4 solution polishing` of OSQP paper

4. Prefer Vector Updates only (Matrix update needs inversion of new KKT matrix)
    - See `section 6 Parametric programs` of OSQP paper
    - If system dynamics and sparsity do not change, update `q`,`l`,`u` only in realtime

5. Matrix Updates
    - Allowed only if sparsity structure is unchanged
    - See example, use `OSQP_NULL` as index to update values

6. Constraints
    - Avoid unnecessary box constraints that are never active
    - E.g. Maximum position/velocity/acceleration
    - NO need if the controller stays stable around set points
