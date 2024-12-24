#include "qruov.h"
#include <x86intrin.h>

typedef __m256i Fq_vec ;

inline static __m512i Fq_vec_add_512(__m512i a, __m512i b){
  __m512i   q = _mm512_set1_epi8(QRUOV_q)      ; // Fq_vec_PRINT(q) ;
  __m512i   c = _mm512_adds_epu8(a, b)         ; // Fq_vec_PRINT(c) ;
  __mmask64 m = _mm512_cmple_epu8_mask(q, c)   ; // printf("m = %016llx\n",m);
  __m512i   d = _mm512_mask_subs_epu8(c,m,c,q) ; // Fq_vec_PRINT(d) ;
  return d ;
}

inline static __m512i Fq_vec_sub_512(__m512i a, __m512i b){
  __m512i   q = _mm512_set1_epi8(QRUOV_q)      ; // Fq_vec_PRINT(q) ;
  __mmask64 m = _mm512_cmplt_epu8_mask(a, b)   ; // printf("m = %016llx\n",m);
  __m512i   c = _mm512_mask_adds_epu8(a,m,a,q) ; // Fq_vec_PRINT(c) ;
  __m512i   d = _mm512_subs_epu8(c, b)         ; // Fq_vec_PRINT(d) ;
  return d ;
}

inline static __m512i Fq_vec_dot_accumulate_512(__m512i a, __m512i b, __m512i c){
  return _mm512_dpbusd_epi32(c,a,b) ;
}

inline static uint64_t Fq_vec_dot_accumulate_final_512(__m512i c){
  __m256i hi0 = _mm512_extracti64x4_epi64(c, 1) ;
  __m256i lo0 = _mm512_castsi512_si256(c)       ;
  __m256i f   = _mm256_add_epi32(lo0, hi0)      ;
// --
  __m128i hi = _mm256_extracti128_si256(f, 1)   ;
  __m128i lo = _mm256_castsi256_si128(f)        ;
  __m128i s1 = _mm_hadd_epi32(lo, hi)           ;
  __m128i s2 = _mm_hadd_epi32(s1, s1)           ;
  __m128i s3 = _mm_hadd_epi32(s2, s2)           ;
  return ((uint64_t)_mm_extract_epi32(s3, 0))   ;
}

inline static Fq_vec Fq_vec_add(Fq_vec a0, Fq_vec b0){
  __m512i a = _mm512_castsi256_si512(a0) ;
  __m512i b = _mm512_castsi256_si512(b0) ;
  __m512i c = Fq_vec_add_512(a, b)       ;
  return _mm512_castsi512_si256(c)       ;
}

inline static void Fq_vec_add_2(Fq_vec a0, Fq_vec a1, Fq_vec b0, Fq_vec b1, Fq_vec c_[2]){
  __m512i a = _mm512_inserti64x4(_mm512_castsi256_si512(a0),a1,1) ;
  __m512i b = _mm512_inserti64x4(_mm512_castsi256_si512(b0),b1,1) ;
  __m512i c = Fq_vec_add_512(a, b) ;
      c_[1] = _mm512_extracti64x4_epi64(c, 1) ;
      c_[0] = _mm512_castsi512_si256(c) ;
}

inline static Fq_vec Fq_vec_sub(Fq_vec a0, Fq_vec b0){
  __m512i a = _mm512_castsi256_si512(a0) ;
  __m512i b = _mm512_castsi256_si512(b0) ;
  __m512i c = Fq_vec_sub_512(a, b)       ;
  return _mm512_castsi512_si256(c)       ;
}

inline static void Fq_vec_sub_2(Fq_vec a0, Fq_vec a1, Fq_vec b0, Fq_vec b1, Fq_vec c_[2]){
  __m512i a = _mm512_inserti64x4(_mm512_castsi256_si512(a0),a1,1) ;
  __m512i b = _mm512_inserti64x4(_mm512_castsi256_si512(b0),b1,1) ;
  __m512i c = Fq_vec_sub_512(a, b) ;
      c_[1] = _mm512_extracti64x4_epi64(c, 1) ;
      c_[0] = _mm512_castsi512_si256(c) ;
}

inline static Fq_vec Fq_vec_dot_accumulate(Fq_vec a0, Fq_vec b0, Fq_vec c0){
  __m512i a = _mm512_castsi256_si512(a0) ;
  __m512i b = _mm512_castsi256_si512(b0) ;
  __m512i c = _mm512_castsi256_si512(c0) ;
  __m512i d = Fq_vec_dot_accumulate_512(a, b, c) ;
  return _mm512_castsi512_si256(d) ;
}

inline static __m512i Fq_vec_dot_accumulate_2(Fq_vec a0, Fq_vec a1, Fq_vec b0, Fq_vec b1, __m512i c){
  __m512i a = _mm512_inserti64x4(_mm512_castsi256_si512(a0),a1,1) ;
  __m512i b = _mm512_inserti64x4(_mm512_castsi256_si512(b0),b1,1) ;
  return Fq_vec_dot_accumulate_512(a, b, c) ;
}

inline static uint64_t Fq_vec_dot_accumulate_final(Fq_vec c){
  __m128i hi = _mm256_extracti128_si256(c, 1) ;
  __m128i lo = _mm256_castsi256_si128(c) ;
  __m128i s1 = _mm_hadd_epi32(lo, hi) ;
  __m128i s2 = _mm_hadd_epi32(s1, s1) ;
  __m128i s3 = _mm_hadd_epi32(s2, s2) ;
  return ((uint64_t)_mm_extract_epi32(s3, 0)) ;
}

#if 0
inline static void Fq_vec_dot_accumulate_final_2(__m512i c, uint64_t * c0, uint64_t * c1){
  __m256i hi   = _mm512_extracti64x4_epi64(d, 1)          ;
  __m256i lo   = _mm512_castsi512_si256(d)                ;
  *c1 += Fq_vec_dot_accumulate_final(hi)                  ;
  *c0 += Fq_vec_dot_accumulate_final(lo)                  ;
}
#else
inline static void Fq_vec_dot_accumulate_final_2(__m512i c, uint64_t * c0, uint64_t * c1){
  __m512i addr = _mm512_set_epi64(7, 6, 3, 2, 5, 4, 1, 0) ;
  __m512i d    = _mm512_permutexvar_epi64(addr, c)        ;
  __m256i hi   = _mm512_extracti64x4_epi64(d, 1)          ;
  __m256i lo   = _mm512_castsi512_si256(d)                ;
  __m256i f    = _mm256_add_epi32(lo, hi)                 ;
// --
  __m256i g    = _mm256_hadd_epi32(f, f)                  ;
  __m256i h    = _mm256_hadd_epi32(g, g)                  ;
  *c0 += ((uint64_t)_mm256_extract_epi32(h, 0))           ;
  *c1 += ((uint64_t)_mm256_extract_epi32(h, 4))           ;
}
#endif

// =============================================================================
//
// =============================================================================

inline static void vector_M_add(const vector_M A, const vector_M B, vector_M C){
  Fq_vec * a = (Fq_vec *) A ;
  Fq_vec * b = (Fq_vec *) B ;
  Fq_vec * c = (Fq_vec *) C ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_M) ; j++) *c++ = Fq_vec_add(*a++, *b++) ;
}

inline static void vector_M_add_2(
  const vector_M A0, const vector_M A1, 
  const vector_M B0, const vector_M B1, 
        vector_M C0,       vector_M C1
  ){
  vector_M_add(A0, B0, C0) ;
  vector_M_add(A1, B1, C1) ;
}

inline static void vector_M_sub(const vector_M A, const vector_M B, vector_M C){
  Fq_vec * a = (Fq_vec *) A ;
  Fq_vec * b = (Fq_vec *) B ;
  Fq_vec * c = (Fq_vec *) C ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_M) ; j++) *c++ = Fq_vec_sub(*a++, *b++) ;
}

inline static void vector_M_sub_2(
  const vector_M A0, const vector_M A1, 
  const vector_M B0, const vector_M B1, 
        vector_M C0,       vector_M C1
){
  vector_M_sub(A0, B0, C0) ;
  vector_M_sub(A1, B1, C1) ;
}

inline static void vector_V_sub(const vector_V A, const vector_V B, vector_V C){
  Fq_vec * a = (Fq_vec *) A ;
  Fq_vec * b = (Fq_vec *) B ;
  Fq_vec * c = (Fq_vec *) C ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_V) ; j++) *c++ = Fq_vec_sub(*a++, *b++) ;
}

inline static void vector_V_sub_2(
  const vector_V A0, const vector_V A1, 
  const vector_V B0, const vector_V B1, 
        vector_V C0,       vector_V C1
){
  vector_V_sub(A0, B0, C0) ;
  vector_V_sub(A1, B1, C1) ;
}

inline static uint64_t vector_V_dot_vector_V(const vector_V A, const vector_V B){
  Fq_vec * a = (Fq_vec *) A ;
  Fq_vec * b = (Fq_vec *) B ;
  Fq_vec   c ; c = _mm256_set1_epi32(0) ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_V) ; j++) c = Fq_vec_dot_accumulate(*a++, *b++, c) ;
  uint64_t C = Fq_vec_dot_accumulate_final(c) ;
  return C ;
}

#if 0
inline static void vector_V_dot_vector_V_2(
  const vector_V A0, const vector_V A1, 
  const vector_V B0, const vector_V B1, 
  uint64_t * C0    , uint64_t * C1
){
  *C0 += vector_V_dot_vector_V(A0, B0);
  *C1 += vector_V_dot_vector_V(A1, B1);
}
#else
inline static void vector_V_dot_vector_V_2(
  const vector_V A0, const vector_V A1, 
  const vector_V B0, const vector_V B1, 
  uint64_t * C0    , uint64_t * C1
){
  Fq_vec * a0 = (Fq_vec *) A0 ;
  Fq_vec * a1 = (Fq_vec *) A1 ;
  Fq_vec * b0 = (Fq_vec *) B0 ;
  Fq_vec * b1 = (Fq_vec *) B1 ;

  __m512i c ; c = _mm512_xor_epi32(c,c) ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_V) ; j++) c = Fq_vec_dot_accumulate_2(*a0++, *a1++, *b0++, *b1++, c) ;
  Fq_vec_dot_accumulate_final_2(c, C0, C1) ;
}
#endif

inline static uint64_t vector_M_dot_vector_M(const vector_M A, const vector_M B){
  Fq_vec * a = (Fq_vec *) A ;
  Fq_vec * b = (Fq_vec *) B ;
  Fq_vec   c ; c = _mm256_set1_epi32(0) ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_M) ; j++) c = Fq_vec_dot_accumulate(*a++, *b++, c) ;
  uint64_t C = Fq_vec_dot_accumulate_final(c) ;
  return C ;
}

#if 0
inline static void vector_M_dot_vector_M_2(
  const vector_M A0, const vector_M A1, 
  const vector_M B0, const vector_M B1, 
  uint64_t * C0    , uint64_t * C1
){
  *C0 += vector_M_dot_vector_M(A0, B0);
  *C1 += vector_M_dot_vector_M(A1, B1);
}
#else
inline static void vector_M_dot_vector_M_2(
  const vector_M A0, const vector_M A1, 
  const vector_M B0, const vector_M B1, 
  uint64_t * C0    , uint64_t * C1
){
  Fq_vec * a0 = (Fq_vec *) A0 ;
  Fq_vec * a1 = (Fq_vec *) A1 ;
  Fq_vec * b0 = (Fq_vec *) B0 ;
  Fq_vec * b1 = (Fq_vec *) B1 ;

  __m512i c ; c = _mm512_xor_epi32(c,c) ;
  for(int j = 0 ; j < BYTES2BLOCKS(QRUOV_M) ; j++) c = Fq_vec_dot_accumulate_2(*a0++, *a1++, *b0++, *b1++, c) ;
  Fq_vec_dot_accumulate_final_2(c, C0, C1) ;
}
#endif

// =============================================================================
//
// =============================================================================

void VECTOR_M_ADD(const VECTOR_M A, const VECTOR_M B, VECTOR_M C){
//  for(int i = 0 ; i < QRUOV_L ; i++) vector_M_add(A[i], B[i], C[i]) ;
#  if (QRUOV_L & 1)
  {
    int i ;
    for(i = 0 ; i < QRUOV_L - 1 ; i+=2){
      vector_M_add_2(
        A[i], A[i+1], 
        B[i], B[i+1], 
        C[i], C[i+1]
      ) ;
    }
    vector_M_add(A[i], B[i], C[i]) ;
  }
#  else
  for(int i = 0 ; i < QRUOV_L ; i+=2){
    vector_M_add_2(
      A[i], A[i+1], 
      B[i], B[i+1], 
      C[i], C[i+1]
    ) ;
  }
#  endif
}

void VECTOR_M_SUB(const VECTOR_M A, const VECTOR_M B, VECTOR_M C){
//  for(int i = 0 ; i < QRUOV_L ; i++) vector_M_sub(A[i], B[i], C[i]) ;
#  if (QRUOV_L & 1)
  {
    int i ;
    for(i = 0 ; i < QRUOV_L - 1 ; i+=2){
      vector_M_sub_2(
        A[i], A[i+1], 
        B[i], B[i+1], 
        C[i], C[i+1]
      ) ;
    }
    vector_M_sub(A[i], B[i], C[i]) ;
  }
#  else
  for(int i = 0 ; i < QRUOV_L ; i+=2){
    vector_M_sub_2(
      A[i], A[i+1], 
      B[i], B[i+1], 
      C[i], C[i+1]
    ) ;
  }
#  endif
}

void VECTOR_V_SUB(const VECTOR_V A, const VECTOR_V B, VECTOR_V C){
//  for(int i = 0 ; i < QRUOV_L ; i++) vector_V_sub(A[i], B[i], C[i]) ;
#  if (QRUOV_L & 1)
  {
    int i ;
    for(i = 0 ; i < QRUOV_L - 1 ; i+=2){
      vector_V_sub_2(
        A[i], A[i+1], 
        B[i], B[i+1], 
        C[i], C[i+1]
      ) ;
    }
    vector_V_sub(A[i], B[i], C[i]) ;
  }
#  else
  for(int i = 0 ; i < QRUOV_L ; i+=2){
    vector_V_sub_2(
      A[i], A[i+1], 
      B[i], B[i+1], 
      C[i], C[i+1]
    ) ;
  }
#  endif
}

void VECTOR_V_dot_VECTOR_V (
  const VECTOR_V A,         // input
  const VECTOR_V B,         // input
        Fq       C[QRUOV_L] // output
){
  uint64_t T[2*QRUOV_L - 1] ;
  memset(T, 0, sizeof(T)) ;

#if 0
  for(int i = 0 ; i < QRUOV_L ; i++)
    for(int j = 0 ; j < QRUOV_L ; j++)
      T[i+j] += vector_V_dot_vector_V (A[i], B[j]) ;
#else
/*
  for(int k = 0 ; k < QRUOV_L * QRUOV_L ; k++){
    int i = k / QRUOV_L ;
    int j = k % QRUOV_L ;
    T[i+j] += vector_V_dot_vector_V (A[i], B[j]) ;
  }
*/

#  if (QRUOV_L & 1)

  {
    int k ;
    for(k = 0 ; k < QRUOV_L * QRUOV_L - 1 ; k+=2){
      int i0 = k / QRUOV_L ;
      int j0 = k % QRUOV_L ;
      int i1 = (k+1) / QRUOV_L ;
      int j1 = (k+1) % QRUOV_L ;
      vector_V_dot_vector_V_2(A[i0], A[i1], B[j0], B[j1], T+i0+j0, T+i1+j1) ;
    }
    int i = k / QRUOV_L ;
    int j = k % QRUOV_L ;
    T[i+j] += vector_V_dot_vector_V (A[i], B[j]) ;
  }

#  else

  for(int k = 0 ; k < QRUOV_L * QRUOV_L ; k+=2){
    int i0 = k / QRUOV_L ;
    int j0 = k % QRUOV_L ;
    int i1 = (k+1) / QRUOV_L ;
    int j1 = (k+1) % QRUOV_L ;
    vector_V_dot_vector_V_2(A[i0], A[i1], B[j0], B[j1], T+i0+j0, T+i1+j1) ;
  }

#  endif

#endif

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

#if 0
  for(int i = 0 ; i < QRUOV_L ; i++)
    for(int j = 0 ; j < QRUOV_L ; j++)
      T[i+j] += vector_M_dot_vector_M (A[i], B[j]) ;
#else
/*
  for(int k = 0 ; k < QRUOV_L * QRUOV_L ; k++){
    int i = k / QRUOV_L ;
    int j = k % QRUOV_L ;
    T[i+j] += vector_M_dot_vector_M (A[i], B[j]) ;
  }
*/

#  if (QRUOV_L & 1)

  {
    int k ;
    for(k = 0 ; k < QRUOV_L * QRUOV_L - 1 ; k+=2){
      int i0 = k / QRUOV_L ;
      int j0 = k % QRUOV_L ;
      int i1 = (k+1) / QRUOV_L ;
      int j1 = (k+1) % QRUOV_L ;
      vector_M_dot_vector_M_2(A[i0], A[i1], B[j0], B[j1], T+i0+j0, T+i1+j1) ;
    }
    int i = k / QRUOV_L ;
    int j = k % QRUOV_L ;
    T[i+j] += vector_M_dot_vector_M (A[i], B[j]) ;
  }

#  else

  for(int k = 0 ; k < QRUOV_L * QRUOV_L ; k+=2){
    int i0 = k / QRUOV_L ;
    int j0 = k % QRUOV_L ;
    int i1 = (k+1) / QRUOV_L ;
    int j1 = (k+1) % QRUOV_L ;
    vector_M_dot_vector_M_2(A[i0], A[i1], B[j0], B[j1], T+i0+j0, T+i1+j1) ;
  }

#  endif

#endif

  for(int i = 2*QRUOV_L-2; i >= QRUOV_L; i--){
    T[i-QRUOV_L]          += QRUOV_fc0 * T[i] ;
    T[i-QRUOV_L+QRUOV_fe] += QRUOV_fc  * T[i] ;
  }

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
