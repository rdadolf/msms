import numpy as np

def scale(n_epochs=1, n_inputs=163, input_layer_sz=13, hidden_layer_sz=64):
  # FIXME: Backprop needs new input data.

  translation_dictionary = {
    'input_dimension': input_layer_sz,
    'training_sets': n_inputs,
    'nodes_per_layer': hidden_layer_sz,
    'epochs': n_epochs,
  }
  return translation_dictionary
