import os
import os.path
import json
import string
import imp

### Scaling ####################################################################
def scale(scalefile, value):
  (path,name) = os.path.split(scalefile)
  if name[-3:]=='.py':
    name = name[:-3]
  mod = imp.load_source('scale',scalefile)
  scalefunc = mod.scale
  return scalefunc(value)

def scale_files(scalefile, value, outfile):
  store_translation_dictionary( scale(scalefile, value), outfile)

def store_translation_dictionary(tdict, outfile):
  with open(outfile,'w') as outf:
    json.dump(tdict, outf, indent=2)

def load_translation_dictionary(infile):
  with open(infile,'r') as transfile:
    transdict = json.load(transfile)
    return transdict

### Translation ################################################################

# Simple, but leaves room for more complicated things later.
def translate(translation_dict, in_string):
  out_string = string.Template(in_string).substitute(translation_dict)
  return out_string

def translate_files(tdict_file, infile, outfile):
  translation_dict = load_translation_dictionary(tdict_file)
  with open(infile,'r') as inf:
    with open(outfile,'w') as outf:
      outf.write(translate(translation_dict, inf.read()))


if __name__=='__main__':
  import sys
  assert len(sys.argv) in (2,3), 'Usage: '+sys.argv[0]+' <translation_dictionary> [template_file]'
  transfile = sys.argv[1]
  if len(sys.argv)>1:
    infile = sys.argv[2]
  else:
    infile = sys.stdin

  translate_files(transfile, infile, sys.stdout)
