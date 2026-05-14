import numpy as np
import osqp
import scipy.sparse as sp
import json

# -----------------------
# Parameters
# -----------------------
A_eq = np.array([
    [-0.618537,  0.618537,  0.618537, -0.618537, -0.618537,  0.618537,  0.618537, -0.618537],
    [ 0.459121,  0.459121, -0.459121, -0.459121,  0.459121,  0.459121, -0.459121, -0.459121],
    [ 0.637667,  0.637667,  0.637667,  0.637667, -0.637667, -0.637667, -0.637667, -0.637667],
    [ 0.03826,   0.03826,  -0.03826,  -0.03826,  -0.03826,  -0.03826,   0.03826,   0.03826],
    [ 0.03826,  -0.03826,  -0.03826,   0.03826,  -0.03826,   0.03826,   0.03826,  -0.03826],
    [ 0.00956501,-0.00956501, 0.00956501, -0.00956501, 0.00956501, -0.00956501, 0.00956501, -0.00956501]
])

w = np.array([5, -5, -10, 0, 0, 0])

f_min = np.full(8, 50)
f_max = np.full(8, 500)
f_pref = np.full(8, 200)

n = len(f_pref)

# -----------------------
# Quadratic problem
# minimize (1/2)xᵀPx + qᵀx
# -----------------------
P =  np.eye(n)
q = - f_pref

obj_scaling_factor = 1/2000
P = P*obj_scaling_factor
q = q*obj_scaling_factor

# -----------------------
# Box constraints
# -----------------------
A_box = np.vstack([np.eye(n), -np.eye(n)])
l_box = np.hstack([f_min, -f_max])
u_box = np.hstack([f_max, -f_min])

# -----------------------
# Combine all constraints
# -----------------------
A = np.vstack([A_eq, A_box])
lb = np.hstack([w, l_box])    # lower bound
ub = np.hstack([w, u_box])    # upper bound

# -----------------------
# OSQP Setup
# -----------------------

# Convert matrix to csc form
P_sp = sp.csc_matrix(np.triu(P))  # OSQP requires upper triangular part
A_sp = sp.csc_matrix(A)

prob = osqp.OSQP()
prob.setup(sp.csc_matrix(np.triu(P)), q, sp.csc_matrix(A), lb, ub, verbose=False)

# Solve
res = prob.solve()

# -----------------------
# Output
# -----------------------
print("Optimal var:")
print(res.x)

print("Optimal obj:")
print(res.info.obj_val)


filename = 'configs.h'

# generate header file directly
cpp_data =  "// Custom member variables for QSQP solver\n\n"

cpp_data += "// number of optimization variables \n"
cpp_data += f"static const OSQPInt m_nVars = {A.shape[1]};\n"

cpp_data += "// number of constraints \n"
cpp_data += f"static const OSQPInt m_nCons = {A.shape[0]};\n"

cpp_data += "// CSC representation of P matrix \n"
cpp_data += "OSQPCscMatrix* m_P;\n"

cpp_data += "// Notice: Only take the upper triangular representation, i.e.  P = numpy.triu(P) \n"
cpp_data += f"OSQPFloat m_P_x[{P_sp.nnz}] = {{{', '.join(map(str, P_sp.data.tolist()))}}};\n"
cpp_data += f"OSQPInt m_P_nnz = {P_sp.nnz};\n"
cpp_data += f"OSQPInt m_P_i[{P_sp.nnz}] = {{{', '.join(map(str, P_sp.indices.tolist()))}}};\n"
cpp_data += f"OSQPInt m_P_p[{P_sp.indptr.size}] = {{{', '.join(map(str, P_sp.indptr.astype(int).tolist()))}}};\n\n"

cpp_data += "// CSC representation of A matrix \n"
cpp_data += f"OSQPCscMatrix* m_A;\n"
cpp_data += "// Notice: All elements of A are stored \n"
cpp_data += f"OSQPFloat m_A_x[{A_sp.nnz}] = {{{', '.join(map(str, A_sp.data.tolist()))}}};\n"
cpp_data += f"OSQPInt m_A_nnz = {A_sp.nnz};\n"
cpp_data += f"OSQPInt m_A_i[{A_sp.nnz}] = {{{', '.join(map(str, A_sp.indices.tolist()))}}};\n"
cpp_data += f"OSQPInt m_A_p[{A_sp.indptr.size}] = {{{', '.join(map(str, A_sp.indptr.astype(int).tolist()))}}};\n"

cpp_data += "// dense vector q \n"
cpp_data += f"OSQPFloat m_q[{q.size}] = {{{', '.join(map(str, q.flatten().tolist()))}}};\n"
cpp_data += "// dense lower bound vector lb \n"
cpp_data += f"OSQPFloat m_lb[{lb.size}] = {{{', '.join(map(str, lb.flatten().tolist()))}}};\n"
cpp_data += "// dense upper bound vector ub \n"
cpp_data += f"OSQPFloat m_ub[{ub.size}] = {{{', '.join(map(str, ub.flatten().tolist()))}}};\n"

cpp_data += "// Exitflag  \n"
cpp_data += "OSQPInt m_exitflag;  \n"
cpp_data += "// Solver, settings, matrices  \n"
cpp_data += "OSQPSolver* m_solver;   \n"
cpp_data += "OSQPSettings* m_osqp_settings;  \n"

# write to file
with open(filename, 'w') as f:
    f.write(cpp_data)

