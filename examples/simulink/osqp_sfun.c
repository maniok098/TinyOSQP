#define S_FUNCTION_NAME  osqp_sfun
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include "osqp.h"
#include <stdlib.h>

/* =========================
   Problem dimensions
   ========================= */
static const OSQPInt N_VARS = 2;
static const OSQPInt N_CONS = 3;

/* =========================
   Solver context structure
   ========================= */
typedef struct {

    /* OSQP objects */
    OSQPSolver* solver;
    OSQPSettings* settings;
    OSQPCscMatrix* P;
    OSQPCscMatrix* A;

    /* Data buffers */
    OSQPFloat P_x[3];
    OSQPInt   P_i[3];
    OSQPInt   P_p[3];

    OSQPFloat A_x[4];
    OSQPInt   A_i[4];
    OSQPInt   A_p[3];

    OSQPFloat q[2];
    OSQPFloat lb[3];
    OSQPFloat ub[3];

} OsqpContext;


/* ============================================================
   mdlInitializeSizes
   ============================================================ */
static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 0);

    /* One input port: q (size 2) */
    ssSetNumInputPorts(S, 1);
    ssSetInputPortWidth(S, 0, N_VARS);
    ssSetInputPortDirectFeedThrough(S, 0, 1);

    /* One output port: x (size 2) */
    ssSetNumOutputPorts(S, 1);
    ssSetOutputPortWidth(S, 0, N_VARS);

    ssSetNumSampleTimes(S, 1);

    /* One persistent pointer (solver context) */
    ssSetNumPWork(S, 1);

    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
}


/* ============================================================
   mdlInitializeSampleTimes
   ============================================================ */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}


/* ============================================================
   mdlStart
   Called once at simulation start
   ============================================================ */
#define MDL_START
static void mdlStart(SimStruct *S)
{
    OsqpContext* ctx = (OsqpContext*)malloc(sizeof(OsqpContext));

    /* ---------- P matrix (upper triangular only) ---------- */
    ctx->P_x[0] = 4.0;
    ctx->P_x[1] = 1.0;
    ctx->P_x[2] = 2.0;

    ctx->P_i[0] = 0;
    ctx->P_i[1] = 0;
    ctx->P_i[2] = 1;

    ctx->P_p[0] = 0;
    ctx->P_p[1] = 1;
    ctx->P_p[2] = 3;

    /* ---------- A matrix ---------- */
    ctx->A_x[0] = 1.0;
    ctx->A_x[1] = 1.0;
    ctx->A_x[2] = 1.0;
    ctx->A_x[3] = 1.0;

    ctx->A_i[0] = 0;
    ctx->A_i[1] = 1;
    ctx->A_i[2] = 0;
    ctx->A_i[3] = 2;

    ctx->A_p[0] = 0;
    ctx->A_p[1] = 2;
    ctx->A_p[2] = 4;

    /* ---------- Vectors ---------- */
    ctx->q[0]  = 1.0;
    ctx->q[1]  = 1.0;

    ctx->lb[0] = 1.0;
    ctx->lb[1] = 0.0;
    ctx->lb[2] = 0.0;

    ctx->ub[0] = 1.0;
    ctx->ub[1] = 0.7;
    ctx->ub[2] = 0.7;

    /* ---------- Allocate CSC matrices ---------- */
    ctx->P = (OSQPCscMatrix*)malloc(sizeof(OSQPCscMatrix));
    ctx->A = (OSQPCscMatrix*)malloc(sizeof(OSQPCscMatrix));

    csc_set_data(ctx->P, N_VARS, N_VARS, 3,
                 ctx->P_x, ctx->P_i, ctx->P_p);

    csc_set_data(ctx->A, N_CONS, N_VARS, 4,
                 ctx->A_x, ctx->A_i, ctx->A_p);

    /* ---------- Settings ---------- */
    ctx->settings = (OSQPSettings*)malloc(sizeof(OSQPSettings));
    osqp_set_default_settings(ctx->settings);

    ctx->settings->polishing = 1;
    ctx->settings->polish_refine_iter = 3;
    ctx->settings->scaled_termination = 0;
    ctx->settings->scaling = 10;
    ctx->settings->max_iter = 50;

    /* ---------- Setup solver ---------- */
    if (osqp_setup(&ctx->solver,
                   ctx->P,
                   ctx->q,
                   ctx->A,
                   ctx->lb,
                   ctx->ub,
                   N_CONS,
                   N_VARS,
                   ctx->settings) != 0)
    {
        ssSetErrorStatus(S, "OSQP setup failed");
        return;
    }

    ssSetPWorkValue(S, 0, ctx);
}


/* ============================================================
   mdlOutputs
   Called at every simulation step
   ============================================================ */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    OsqpContext* ctx = (OsqpContext*)ssGetPWorkValue(S, 0);

    InputRealPtrsType u = ssGetInputPortRealSignalPtrs(S, 0);
    real_T* y = ssGetOutputPortRealSignal(S, 0);

    /* Update q from Simulink input */
    ctx->q[0] = *u[0];
    ctx->q[1] = *u[1];

    osqp_update_data_vec(ctx->solver,
                         ctx->q,
                         ctx->lb,
                         ctx->ub);

    osqp_solve(ctx->solver);

    /* Output solution */
    for (int i = 0; i < N_VARS; ++i) {
        y[i] = ctx->solver->solution->x[i];
    }
}


/* ============================================================
   mdlTerminate
   ============================================================ */
static void mdlTerminate(SimStruct *S)
{
    OsqpContext* ctx = (OsqpContext*)ssGetPWorkValue(S, 0);

    if (!ctx) return;

    osqp_cleanup(ctx->solver);

    free(ctx->P);
    free(ctx->A);
    free(ctx->settings);
    free(ctx);
}


/* ============================================================
   Required S-function trailer
   ============================================================ */
#ifdef MATLAB_MEX_FILE
#include "simulink.c"
#else
#include "cg_sfun.h"
#endif
