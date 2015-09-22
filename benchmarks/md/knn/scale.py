import numpy as np

def scale(n):
  # MD represents the inner loop of a molecular dynamics sim, computing the
  # Lennard-Jones potential:
  #   LJ(r) = 4*epsilon*( (sigma/r)^12 - (sigma/r)^6 )
  # This potential technically applies to all O(n^2) pairs of atoms, but due
  # to the high exponents, it decays to zero quickly. All MD algorithms exploit
  # this to reduce computation.
  #
  # The KNN algorithm uses a periodic spatial sorting to choose the k nearest
  # atoms to include in the calculation, where k is chosen to conservatively
  # capture all atoms close enough to have nonzero LJ potentials. Typically,
  # the vanishing threshold is about 2.5*sigma (the energy minimum is at
  # 2^(1/6)*sigma), and k is chosen such that it encompasses a slightly larger
  # distance on average (LAMMPS uses 2.8*sigma).
  #
  # While we will actually do a spatial sort during input generation, we can
  # choose k here crudely based on a mathematical estimate of the inter-atom
  # distance.

  # Chosen to match memory constraints. Let's let the rest fall out.
  n_atoms = 864

  # Constants, based on Aneesur Rahman's seminal 1964 paper on MD sims of Argon
  sigma = 3.4 # angstroms
  epsilon = 120 # normalized to 1/kB (Boltzmann's constant)
  domain_edge = 10.229*sigma # angstroms

  # simplifying the LJ-equation, we can isolate and pre-compute the numerators:
  #   LJ(r) = (4*e*s^12)/r^12 - (4*e*s^6)/r^6
  #           |---lj1--|        |--lj2--|
  lj1 = 4*epsilon*sigma**12
  lj2 = 4*epsilon*sigma**6

  # In order to avoid craziness (numerical instability), we don't place points
  # too close together. We choose the minimum distance to be the balance point
  # (i.e.- r==sigma, where the fractions are 1 and LJ(r)==0)
  min_distance = sigma

  vanishing_threshold = 2.5*sigma
  cutoff_threshold = 2.8*sigma

  # Now let's estimate the expected number of neighbors.
  # Given a uniform distribution, this is the ratio of the volume of a sphere
  # of radius=cutoff_threshold inside a cube of side=domain_edge.
  Ek = n_atoms * ((4*np.pi*cutoff_threshold)/3.) / (domain_edge**3)

  max_neighbors = int(np.ceil(Ek))

  translation_dictionary = {
    'n_atoms': n_atoms,
    'max_neighbors': max_neighbors,
    'domain_edge': domain_edge,
    'min_distance': min_distance,
    'lj1': lj1,
    'lj2': lj2,
  }
  return translation_dictionary
