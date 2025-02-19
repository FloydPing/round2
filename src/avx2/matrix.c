#include "qruov.h"
#include <x86intrin.h>

typedef __m256i Fq_vec ;

#if (QRUOV_q == 127)
inline static Fq_vec Fq_vec_add(Fq_vec a, Fq_vec b){
  __m256i q = _mm256_set1_epi8(QRUOV_q) ; // Fq_vec_PRINT(q) ;
  __m256i z = _mm256_xor_si256(q, q)    ; // Fq_vec_PRINT(z) ;
  __m256i c = _mm256_add_epi8(a, b)     ; // Fq_vec_PRINT(c) ;
  __m256i d = _mm256_cmpgt_epi8(q, c)   ; // Fq_vec_PRINT(d) ;
  __m256i e = _mm256_cmpgt_epi8(z, c)   ; // Fq_vec_PRINT(e) ;
  __m256i f = _mm256_xor_si256(d, e)    ; // Fq_vec_PRINT(f) ;
  __m256i g = _mm256_andnot_si256(f, q) ; // Fq_vec_PRINT(g) ;
  __m256i h = _mm256_sub_epi8(c, g)     ; // Fq_vec_PRINT(h) ;
  return h ;
}
#elif (QRUOV_q == 31) || (QRUOV_q == 7)
inline static Fq_vec Fq_vec_add(Fq_vec a, Fq_vec b){
  __m256i q = _mm256_set1_epi8(QRUOV_q) ; // Fq_vec_PRINT(q) ;
  __m256i c = _mm256_add_epi8(a, b)     ; // Fq_vec_PRINT(c) ;
  __m256i d = _mm256_cmpgt_epi8(q, c)   ; // Fq_vec_PRINT(d) ;
  __m256i e = _mm256_andnot_si256(d, q) ; // Fq_vec_PRINT(e) ;
  __m256i f = _mm256_sub_epi8(c, e)     ; // Fq_vec_PRINT(f) ;
  return f ;
}
#else
#  error "unsupported QRUOV_q."
#endif

inline static Fq_vec Fq_vec_sub(Fq_vec a, Fq_vec b){
  __m256i q = _mm256_set1_epi8(QRUOV_q) ; // Fq_vec_PRINT(q) ;
  __m256i c = _mm256_cmpgt_epi8(b, a)   ; // Fq_vec_PRINT(c) ;
  __m256i d = _mm256_and_si256(c, q)    ; // Fq_vec_PRINT(d) ;
  __m256i e = _mm256_sub_epi8(a, b)     ; // Fq_vec_PRINT(e) ;
  __m256i f = _mm256_add_epi8(d, e)     ; // Fq_vec_PRINT(f) ;
  return f ;
}

inline static Fq_vec Fq_vec_dot_accumulate(Fq_vec a, Fq_vec b, Fq_vec c){
  __m256i d = _mm256_maddubs_epi16(a,b) ; // Fq_vec_PRINT(d) ;
  __m256i e = _mm256_set1_epi16(1)      ; // Fq_vec_PRINT(e) ;
  __m256i f = _mm256_madd_epi16 (d,e)   ; // Fq_vec_PRINT(f) ;
  __m256i g = _mm256_add_epi32 (f,c)    ; // Fq_vec_PRINT(g) ;
  return g ;
}

inline static uint64_t Fq_vec_dot_accumulate_final(Fq_vec c){
  __m128i lo = _mm256_castsi256_si128(c) ;
  __m128i hi = _mm256_extracti128_si256(c, 1) ;
  __m128i s1 = _mm_hadd_epi32(lo, hi) ;
  __m128i s2 = _mm_hadd_epi32(s1, s1) ;
  __m128i s3 = _mm_hadd_epi32(s2, s2) ;
  return ((uint64_t)_mm_extract_epi32(s3, 0)) ;
}

// =============================================================================
//
// =============================================================================

inline static void vector_M_add(const vector_M A, const vector_M B, vector_M C){
  Fq_vec * a = (Fq_vec *) A ;
  Fq_vec * b = (Fq_vec *) B ;
  Fq_vec * c = (Fq_vec *) C ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_M) ; j++) *c++ = Fq_vec_add(*a++, *b++) ;
}

inline static void vector_M_sub(const vector_M A, const vector_M B, vector_M C){
  Fq_vec * a = (Fq_vec *) A ;
  Fq_vec * b = (Fq_vec *) B ;
  Fq_vec * c = (Fq_vec *) C ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_M) ; j++) *c++ = Fq_vec_sub(*a++, *b++) ;
}

inline static void vector_V_sub(const vector_V A, const vector_V B, vector_V C){
  Fq_vec * a = (Fq_vec *) A ;
  Fq_vec * b = (Fq_vec *) B ;
  Fq_vec * c = (Fq_vec *) C ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_V) ; j++) *c++ = Fq_vec_sub(*a++, *b++) ;
}

inline static uint64_t vector_V_dot_vector_V(const vector_V A, const vector_V B){
  Fq_vec * a = (Fq_vec *) A ;
  Fq_vec * b = (Fq_vec *) B ;
  Fq_vec   c ; c = _mm256_set1_epi32(0) ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_V) ; j++) c = Fq_vec_dot_accumulate(*a++, *b++, c) ;
  uint64_t C = Fq_vec_dot_accumulate_final(c) ;
  return C ;
}

inline static uint64_t vector_M_dot_vector_M(const vector_M A, const vector_M B){
  Fq_vec * a = (Fq_vec *) A ;
  Fq_vec * b = (Fq_vec *) B ;
  Fq_vec   c ; c = _mm256_set1_epi32(0) ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_M) ; j++) c = Fq_vec_dot_accumulate(*a++, *b++, c) ;
  uint64_t C = Fq_vec_dot_accumulate_final(c) ;
  return C ;
}

// =============================================================================
//
// =============================================================================

void VECTOR_M_ADD(const VECTOR_M A, const VECTOR_M B, VECTOR_M C){
  for(int i = 0 ; i < QRUOV_L ; i++) vector_M_add(A[i], B[i], C[i]) ;
}

void VECTOR_M_SUB(const VECTOR_M A, const VECTOR_M B, VECTOR_M C){
  for(int i = 0 ; i < QRUOV_L ; i++) vector_M_sub(A[i], B[i], C[i]) ;
}

void VECTOR_V_SUB(const VECTOR_V A, const VECTOR_V B, VECTOR_V C){
  for(int i = 0 ; i < QRUOV_L ; i++) vector_V_sub(A[i], B[i], C[i]) ;
}

void VECTOR_V_dot_VECTOR_V (
  const VECTOR_V A,         // input
  const VECTOR_V B,         // input
        Fq       C[QRUOV_L] // output
){
  uint64_t T[2*QRUOV_L - 1] ;
  memset(T, 0, sizeof(T)) ;

  for(int i = 0 ; i < QRUOV_L ; i++)
    for(int j = 0 ; j < QRUOV_L ; j++)
      T[i+j] += vector_V_dot_vector_V (A[i], B[j]) ;

  for(int i = 2*QRUOV_L-2; i >= QRUOV_L; i--){
    T[i-QRUOV_L]          += QRUOV_fc0 * T[i] ;
    T[i-QRUOV_L+QRUOV_fe] += QRUOV_fc  * T[i] ;
  }

  for(int i = 0; i < QRUOV_L; i++) C[i] = (Fq)(T[i] % QRUOV_q) ;
}

void VECTOR_M_dot_VECTOR_M (
  const VECTOR_M A,         // input
  const VECTOR_M B,         // input
        Fq       C[QRUOV_L] // output
){
  uint64_t T[2*QRUOV_L - 1] ;
  memset(T, 0, sizeof(T)) ;

  for(int i = 0 ; i < QRUOV_L ; i++)
    for(int j = 0 ; j < QRUOV_L ; j++)
      T[i+j] += vector_M_dot_vector_M (A[i], B[j]) ;

  for(int i = 2*QRUOV_L-2; i >= QRUOV_L; i--){
    T[i-QRUOV_L]          += QRUOV_fc0 * T[i] ;
    T[i-QRUOV_L+QRUOV_fe] += QRUOV_fc  * T[i] ;
  }

  // TODO: this can be optimized
  for(int i = 0; i < QRUOV_L; i++) C[i] = (Fq)(T[i] % QRUOV_q) ;
}

void VECTOR_V_MUL_SYMMETRIC_MATRIX_VxV(const VECTOR_V A, const MATRIX_VxV B, VECTOR_V C){
  Fq tmp [QRUOV_L] ;
  for(int i = 0 ; i < QRUOV_V ; i++){
    VECTOR_V_dot_VECTOR_V (A, B[i], tmp) ;
    for(int k = 0; k < QRUOV_L; k++) C[k][i] = tmp[k] ;
  }
  VECTOR_V_CLEAR_TAIL(C) ;
}

/// TODO to optimize this, we need to circument the padding in each
void MATRIX_TRANSPOSE_VxM(const MATRIX_VxM A, MATRIX_MxV C){
  for(int i=0;i<QRUOV_V;i++){
    for(int k=0;k<QRUOV_L;k++){
      for(int j=0;j<QRUOV_M;j++){
        C[j][k][i] = A[i][k][j] ;
      }
    }
  }
  for(int j=0;j<QRUOV_M;j++) VECTOR_V_CLEAR_TAIL(C[j]) ;
}

void VECTOR_V_MUL_MATRIX_VxM(const VECTOR_V A, const MATRIX_VxM B, VECTOR_M C){
  MATRIX_MxV BT ;
  MATRIX_TRANSPOSE_VxM(B, BT) ;
  Fq tmp [QRUOV_L] ;
  for(int i = 0 ; i < QRUOV_M ; i++){
    VECTOR_V_dot_VECTOR_V (A, BT[i], tmp) ;
    for(int k = 0; k < QRUOV_L; k++) C[k][i] = tmp[k] ;
  }
  VECTOR_M_CLEAR_TAIL(C) ;
}

void MATRIX_MxV_MUL_SYMMETRIC_MATRIX_VxV(const MATRIX_MxV A, const MATRIX_VxV B, MATRIX_MxV C){
  for(int i = 0 ; i < QRUOV_M ; i++){
    VECTOR_V_MUL_SYMMETRIC_MATRIX_VxV(A[i], B, C[i]) ;
  }
}

void MATRIX_MUL_MxV_VxM(const MATRIX_MxV A, const MATRIX_VxM B, MATRIX_MxM C){
  for(int i = 0 ; i < QRUOV_M ; i++){
    VECTOR_V_MUL_MATRIX_VxM(A[i], B, C[i]) ;
  }
}

void MATRIX_ADD_MxM(const MATRIX_MxM A, const MATRIX_MxM B, MATRIX_MxM C){
  for(int i = 0 ; i < QRUOV_M ; i++){
    VECTOR_M_ADD(A[i], B[i], C[i]) ;
  }
}

void MATRIX_MUL_ADD_MxV_VxM(const MATRIX_MxV A, const MATRIX_VxM B, MATRIX_MxM C){
  MATRIX_MxM T ;
  MATRIX_MUL_MxV_VxM(A, B, T) ;
  MATRIX_ADD_MxM(C, T, C) ;
}

void MATRIX_SUB_MxV(const MATRIX_MxV A, const MATRIX_MxV B, MATRIX_MxV C){
  for(int i = 0 ; i < QRUOV_M ; i++){
    VECTOR_V_SUB(A[i], B[i], C[i]) ;
  }
}
