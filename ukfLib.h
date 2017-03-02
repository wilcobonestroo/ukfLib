#include "mtxLib.h"

typedef void (* tPredictFcn) (tMatrix * pu_p, tMatrix * px_p, tMatrix * pX_m,int sigmaIdx);
typedef void (* tObservFcn) (tMatrix * pu, tMatrix * pX_m, tMatrix * pY_m,int sigmaIdx);

typedef struct tUkfMatrix
{
    tMatrix * Wm_weight_vector;
    tMatrix * Wc_weight_vector;
    tMatrix * x_system_states;
    tMatrix * u_system_input;
    tMatrix * u_prev_system_input;
    tMatrix * X_sigma_points;
    tMatrix * Y_sigma_points;
    tMatrix * y_predicted_mean;
    tMatrix * y_meas;
    tMatrix * Pyy_out_covariance;
    tMatrix * Pxy_cross_covariance;
    tMatrix * P_error_covariance;
    tMatrix * K_kalman_gain;
    tMatrix * K_kalman_gain_transp;
    tMatrix * I_identity_matrix;

}tUkfMatrix;



#define alphaIdx   (int)0
#define bethaIdx   (int)1
#define kappaIdx   (int)2
#define scalingLen  (int)3


typedef struct tUKFpar
{
    int xLen;//length of state vector
    int yLen;//length of measurement vector
    int sLen;//length of sigma point
    double alpha;//Range:[10e-4 : 1].Smaller alpha leads to a tighter (closer) selection of sigma-points,
    double betha;//Contain information about the prior distribution (for Gaussian, beta = 2 is optimal).
    double kappa; //tertiary scaling parameter, usual value 0.
    double lambda;
    tMatrix * pWm;
    tMatrix * pWc;
    tMatrix * pQ;
    tMatrix * pR;

}tUKFpar;

typedef struct tUKFin
{
    tMatrix * pu;    // u(k)   Current inputs
    tMatrix * py;    // y(k)   Current measurement
}tUKFin;

typedef struct tUKFprev
{
    tMatrix * pu_p;    // u(k-1)   Previous inputs
    tMatrix * px_p;    // x(k-1)   Previous states
    tMatrix * pX_p;    // X(k-1)   Calculate the sigma-points
    tMatrix * pP_p;    // P(k-1)    Previous error covariance 
}tUKFprev;

typedef struct tUKFpredict //p(previous)==k-1, m(minus)=(k|k-1)
{
    tMatrix * pX_m;    //X(k|k-1) Propagate each sigma-point through prediction f(Chi)
    tMatrix * px_m;    //x(k|k-1) Calculate mean of predicted state
    tMatrix * pP_m;    //P(k|k-1) Calculate covariance of predicted state  
    tMatrix * pY_m;    //Y(k|k-1) Propagate each sigma-point through observation
    tMatrix * py_m;    //y(k|k-1) Calculate mean of predicted output
    tPredictFcn * pFcnPredict;
    tObservFcn * pFcnObserv;
}tUKFpredict;



typedef struct tUKFupdate
{
    tMatrix * pPyy;   //Calculate covariance of predicted output
    tMatrix * pPxy;   //Calculate cross-covariance of state and output
    tMatrix * pK;     //K(k) Calculate gain
    tMatrix * pKt;     //Kt(k) Kalman gain transponce
    tMatrix * px;     //x(k) Update state estimate   
    tMatrix * pP;     //P(k) Update error covariance
    tMatrix * pIxx;     //tmp buffer initialized as identity matrix stor result from inversion and other operation  
}tUKFupdate;


typedef struct tUKF
{
    tUKFpar     par;
    tUKFprev    prev;
    tUKFin      input;
    tUKFpredict predict;
    tUKFupdate  update;

}tUKF;

typedef void (* tPredictFcn) (tMatrix * pu_p, tMatrix * px_p, tMatrix * pX_m,int sigmaIdx);
typedef void (* tObservFcn) (tMatrix * pu, tMatrix * pX_m, tMatrix * pY_m,int sigmaIdx);

extern void ukf_init(tUKF * const pUkf,double scaling[scalingLen],int xLen,int yLen, tUkfMatrix * pUkfMatrix);
extern void ukf_step(tUKF * const pUkf);