import numpy as np

def scale(n_nodes=(2**8), n_edges=(2**8 * 16), A=57, B=19, C=19, D=5):
  # To simplify SKGs, n_nodes must be 2^x, where x is some even positive integer.
  # We round n_nodes up.
  scale = int(np.ceil(np.log2(n_nodes)/2)*2)
  n_nodes = 2**scale
  # FIXME: set bounds on edge density
  # Lower: n_edges>n_nodes*C
  # Upper: n_edges<n_nodes*n_nodes*C

  
  # Correct and normalize SKG parameters
  if( B!=C ): # Generate symmetric matrices
    bc = (B+C)/2.
    (B,C) = (bc,bc)
  s = float(A) + float(B) + float(C) + float(D)
  prng_rand_max = (2**64)-1
  (a,b,c,d) = [float(x)/s for x in [A,B,C,D]]
  assert sum([a,b,c,d])<=1.0, 'Invalid SKG parameters'
  (iA,iB,iC,iD) = [int(prng_rand_max*x) for x in [a,b,c,d]]

  # NSKGs (Seshadhri et al., ICDM 2011) smooth out the degree distribution.
  # The noise floor needs to be chosen between bounds to both preserve the
  # distribution shape and to smooth sufficiently to remove SKG artifacts.
  # variables and math are from the paper.
  noise = 0.1 # FIXME: pick better than this?
  t = 0 # FIXME
  mu = 0 # FIXME
  tau = 0 # FIXME
  sig = 0 # FIXME
  lower_bound = 0 # FIXME
  upper_bound = min( (a+d)/2, b )
  #print [lower_bound, upper_bound]
  noise = max(lower_bound, min(upper_bound, noise))

  translation_dictionary = {
    'scale': scale,
    'n_nodes': n_nodes,
    'n_edges': n_edges,
    'noise': noise,
    'A': iA,
    'B': iB,
    'C': iC,
    'D': iD,
  }
  return translation_dictionary

#print scale()
#print ''
#print scale(A=1,B=1,C=1,D=1)
