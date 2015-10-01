import numpy as np

def scale(n_elements=2048, radix=4):
  # If we want less than 32 bits, this can be adjusted.
  # It's not really a scaling parameter, though.
  sort_type = 'uint32_t'
  bitwidth = 32
  bit_mask = (2**bitwidth)-1

  # Round radix up to the nearest power of 2
  radix_bits = int(np.ceil(np.log2(radix)))
  radix = 2**radix_bits
  radix_mask = radix-1

  # Which buffer depends on the number of partial sorts
  buffer_parity = int(np.ceil(float(bitwidth)/radix_bits))%2
  which_output_buffer = ['a','b'][buffer_parity]
  #print 'Iterations:',float(bitwidth)/radix_bits
  #print 'Buffer Parity:',buffer_parity
  #print 'Output Buffer:',which_output_buffer

  translation_dictionary = {
    'sort_type': sort_type,
    'bitwidth': bitwidth,
    'bit_mask': bit_mask,
    'which_output_buffer': which_output_buffer,
    'n_elements': n_elements,
    'radix': radix,
    'radix_bits': radix_bits,
    'radix_mask': radix_mask,
  }
  return translation_dictionary
