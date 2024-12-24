#! /bin/sh

kani(){
  ./find_tau $*
}

echo "#pragma once"

kani 128 127 3 156 54 "if"
kani 128 31 3 165 60
kani 128 31 10 600 70
kani 128 7 10 740 100

kani 192 127 3 228 78
kani 192 31 3 246 87
kani 192 31 10 890 100
kani 192 7 10 1100 140

kani 256 127 3 306 105
kani 256 31 3 324 114
kani 256 31 10 1120 120
kani 256 7 10 1490 190

cat <<__EOS__
#else
#  error "unknown (q,L,v,m)"
#endif
__EOS__
