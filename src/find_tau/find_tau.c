#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tau.h"

#define QRUOV_q 127
#define QRUOV_L 3
#define QRUOV_v 156
#define QRUOV_m 54

int main(int argc, char * argv[]){
  int lambda = 256 ;
  int q      = QRUOV_q ;
  int L      = QRUOV_L ;
  int v      = QRUOV_v ;
  int m      = QRUOV_m ;
  char * directive = "elif" ;

  if(argc > 1) lambda    = atol(argv[1]) ;
  if(argc > 2) q         = atol(argv[2]) ;
  if(argc > 3) L         = atol(argv[3]) ;
  if(argc > 4) v         = atol(argv[4]) ;
  if(argc > 5) m         = atol(argv[5]) ;
  if(argc > 6) directive = argv[6] ;

  const int V      = v/L ;
  const int M      = m/L ;

  const int n1     = ((V*(V+1))/2) * L ;
  const int n2     = (V*M) * L ;
  const int n3     = V * L ;
  const int n4     = M * L ;

  const int tau1  = (int)ceil(tau(q, lambda, n1)) ;
  const int tau2  = (int)ceil(tau(q, lambda, n2)) ;
  const int tau3  = (int)ceil(tau(q, lambda, n3)) ;
  const int tau4  = (int)ceil(tau(q, lambda, n4)) ;

  printf("#%s (QRUOV_q == %d)"
         " && (QRUOV_L == %d)"
         " && (QRUOV_v == %d)"
         " && (QRUOV_m == %d)\n",directive,q,L,v,m) ;
  printf("#  define QRUOV_tau1 %7d // n1 = L*V*(V+1)/2 : %7d\n", tau1, n1) ;
  printf("#  define QRUOV_tau2 %7d // n2 = L*V*M       : %7d\n", tau2, n2) ;
  printf("#  define QRUOV_tau3 %7d // n3 = L*V = v     : %7d\n", tau3, n3) ;
  printf("#  define QRUOV_tau4 %7d // n4 = L*M = m     : %7d\n", tau4, n4) ;

  return 0 ;
}
