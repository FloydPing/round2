#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_sf_log.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_hyperg.h>

#define ERROR_ABORT(MESSAGE) { \
  fprintf(stderr, "runtime error: %s in file: %s, line: %d.\n",MESSAGE,__FILE__,__LINE__); \
  abort(); \
}

static double ln_I(const double x, const double a, const double b){
  double r =
      +     gsl_sf_log(gsl_sf_hyperg_2F1(a+b,1,a+1,x))
      + a * gsl_sf_log(x)
      + b * gsl_sf_log(1-x)
      -     gsl_sf_log(a)
      -     gsl_sf_lnbeta(a, b) ;
  return r ;
}

static double minus_log_2_P(const double n, const double t, const double p){
  return - ln_I(1-p, t - n + 1, n) / M_LN2 ;
}

// find tau s.t. lambda = minus_log_2_P(n, tau, p) ; 

double tau(const double q, const double lambda, const double n){
  double r = 1   ;
  for(;r<q;r+=r) ;
  double p = q/r ;

  double d1 = 0 ;
  double m ;

  int i ;

  for(i = 0 ; i < lambda ; i++){
    d1 += n ;
    m = minus_log_2_P(n, n+d1, p) ;
    if(m >= lambda) break ;
  }

  if(m < lambda) ERROR_ABORT("cannot found start point.")  ;

  double d0 = 0 ;
  for( ; d1-d0 > 1.0/(1ULL<<20) ; ){
    double d = (d0 + d1)/2 ; 
    m = minus_log_2_P(n, n+d, p) ; 
    if(m<lambda) d0 = d ;
    else         d1 = d ;
  }
  return ceil(n+d1) ;
}
