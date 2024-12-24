### `find_tau` :

Given $\lambda,q,\ell,v,m$, `find_tau` finds
$`\tau_{q,\lambda}(n):=\min{}\{t\in\mathbb{N}\mid{}P(n,t,q/2^{\lceil\log_2q\rceil})\le2^{-\lambda}\}`$ for some $n(\ell,v,m)$.
$P(n,t,p)$ is the cumulative binomial distribution,

$$ P(n,t,p) := \sum_{i=0}^{n-1} \binom{t}{i} p^i (1-p)^{t-i} = I_{1-p}(t-n+1,n), $$

which denotes the probability of less than $n$ successes in $t$ independent Bernoulli trials of success probability $p$. 
$I_{z}(a,b)$ is called the regularized incomplete beta function which is well known in statistics,
and many numerical packages provide functions to compute it.
By combining such a function with some root-finding algorithm, $\tau_{q,\lambda}(n)$ can be directly evaluated.
For simplicity, `find_tau` employs the bisection method as a root-finding algorithm.
However, more efficient methods like the Newton's one should be employed
to evaluate $\tau$ on the fly in cryptographic functions.
Or some appropriate upper bounds or pre-calculated value can be used.

How to build:

0. Install a C compiler if necessary. For example, if you are using
   ubuntu, type the following into the command line shell.
```
   sudo apt -y install build-essential
```
1. Install "GNU Scientific Library (GSL) -- development package"
   if necessary. For example, if you are using ubuntu, type the
   following into the command line shell.
```
   sudo apt -y install libgsl-dev
```
2. Make it if necessary. For example, if you are using ubuntu,
   type the following into the command line shell.
```
   make
```
3. Make sure the file qruov_tau.h has been created if necessary.
   For example, if you are using ubuntu, type the following into
   the command line shell.
```
   cat qruov_tau.h
```
