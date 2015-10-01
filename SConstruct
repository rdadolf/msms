#
import os
import os.path
import shutil

template_dir = 'benchmarks'
tools_dir = 'tools'
emit_dir = 'emit'
data_dir = 'data'

# Import python modules, so we can execute functions directly
tools = __import__( tools_dir+'.generate', ['generate'] )
# provies 'tools.generate' module

# MachSuite benchmark names
ms_names = [
  'aes/aes',
  'backprop/backprop',
  'bfs/bulk',
  'bfs/queue',
  'fft/strided',
  'fft/transpose',
  'gemm/blocked',
  'gemm/ncubed',
  'kmp/kmp',
  'md/grid',
  'md/knn',
  'nw/nw',
  'sort/merge',
  'sort/radix',
  'spmv/crs',
  'spmv/ellpack',
  'stencil/stencil2d',
  'stencil/stencil3d',
  'viterbi/viterbi',
]
ms_names = ['bfs/queue'] # FIXME: remove; debug
#ms_aliases = {kern: kern+'/'+alg for (kern,alg) in [kern_alg.split('/') for kern_alg in ms_names] if kern==alg}

### Utility ####################################################################
def pjoin(*args):
  return reduce(os.path.join, args)

### Platform-specific ##########################################################
platform = ARGUMENTS.get('OS',Platform())

### Base Environment ###########################################################
E = Environment(
)

# Options which can be overridden
E.SetDefault(verbose=False)

### Command Line Options #######################################################
# (to edit these in vim with proper formatting) set tw=70, set autoindent
#Help('''
#USAGE: scons [<option>=<value>] ...
#  scale=<number>|all
#    Builds a set of benchmarks at scale <number>. Scale values are
#    ordinal keys which map to predefined parameters in each benchmark,
#    such that performance characteristics scale similarly across the
#    board. A scale of 1 produces benchmarks similar to the original
#    MachSuite. A scale of 'all' builds all benchmarks at all scales.
#    *Warning: This can take a long time if used with some flags*.
#  benchmark=<kern>/<alg>,...
#    Builds only the specified benchmark. If not specified, all
#    benchmarks are built.
#  data[=True]
#    Generates input and reference output files for all specified
#    benchmarks. In some cases, generating this data takes
#    significantly longer than the actual benchmark.
#  metrics[=True]
#    Builds instrumentation tools and collects performance
#    characteristics on all specified benchmarks.
#OTHER TARGETS AND ALIASES:
#  test
#    Builds all scales and all inputs, but no metrics. Checks for
#    correctness.
#  regression
#    Builds everything, all settings enabled.
#''')
#
cli_vars = Variables(None, ARGUMENTS)
cli_vars.Add('verbose','Be more thorough about reporting build steps', False)
cli_vars.Add('LLVM_ROOT', 'Set the path to LLVM', os.environ.get('LLVM_ROOT',''))
cli_vars.Add('CXX', 'Select the C++ compiler', os.environ.get('CXX','c++'))
cli_vars.Add('CC', 'Select the C compiler', os.environ.get('CC','cc'))
cli_vars.Add('scale', 'JSON file containing scaling parameters', None)
#cli_vars.Add(ListVariable('benchmark', 'Builds the only the specified benchmarks', default=ms_names, names=ms_names, map=ms_aliases))
#cli_vars.Add(BoolVariable('data', 'Generate input and reference files', 0))
#cli_vars.Add(BoolVariable('metrics', 'Build and run performance metrics', 0))

cli_vars.Update(E)

### Builders ###################################################################
def Scale(target,source,env):
  (trans_dict,) = map(str,target)
  (scalefile, paramfile) = map(str,source)
  tools.generate.scale_files(scalefile, paramfile, trans_dict)
  return 0
Scale_Builder = Builder(action=Scale)

def Emit(target,source,env):
  (outfile,) = map(str,target)
  (trans_dict, infile) = map(str,source)
  tools.generate.translate_files(trans_dict, infile, outfile)
  return 0
Emit_Builder = Builder(action=Emit)

CompileMS_Builder = Builder(action='$CC $CFLAGS $SOURCES -o $TARGET $LDFLAGS')
RunMS_Builder = Builder(action='$SOURCES $TARGET') # sources=(bin, input)
RunGenerate_Builder = Builder(action='$SOURCE $TARGET')

def add_shlib_suffix(target,source,env):
  return ([str(t)+'$SHLIBSUFFIX' for t in target], source)
CompileTool_Builder = Builder(action='$CXX $CXXFLAGS $SOURCE -o $TARGET $LDFLAGS', emitter=add_shlib_suffix)


### Environment Setup ##########################################################
E_emit = E.Clone()
E_emit['BUILDERS']['Scale'] = Scale_Builder
E_emit['BUILDERS']['Emit'] = Emit_Builder

# For compiling MachSuite benchmarks
E_machsuite = E.Clone()
E_machsuite['BUILDERS']['CompileMS'] = CompileMS_Builder
E_machsuite.Append(CFLAGS=['-O2','-Wall','-Wno-unused-label'])
E_machsuite.Append(CFLAGS=['-I'+pjoin(emit_dir,'common')])

E_machsuite['BUILDERS']['RunGenerate'] = RunGenerate_Builder
E_machsuite['BUILDERS']['RunMS'] = RunMS_Builder

# For building the instruction-counting and footprint tools.
E_tool = E.Clone()
E_tool.Replace(LLVM_CONFIG=pjoin(E['LLVM_ROOT'],'bin','llvm-config'))
if E['PLATFORM']=='darwin':
  E_tool.Append(CXXFLAGS=' -fno-common ')
  E_tool.Append(LDFLAGS=' -dynamiclib -Wl,-undefined,dynamic_lookup ')
else:
  E_tool.Append(CXXFLAGS=' -fPIC ')
  E_tool.Append(LDFLAGS=' -shared ')
E_tool.Append(CXXFLAGS=' `$LLVM_CONFIG --cxxflags` ')
E_tool.Append(LDFLAGS=' `$LLVM_CONFIG --ldflags` ')
E_tool['BUILDERS']['CompileTool'] = CompileTool_Builder

# For building instrumented MachSuite benchmarks
E_insn = E_machsuite.Clone()
E_insn.Replace(CC=pjoin(E['LLVM_ROOT'],'bin','clang'))
E_insn['BUILDERS']['CompileMS'] = CompileMS_Builder
E_foot = E_machsuite.Clone()
E_foot.Replace(CC=pjoin(E['LLVM_ROOT'],'bin','clang'))
E_foot['BUILDERS']['CompileMS'] = CompileMS_Builder

### Targets ####################################################################
E.Alias('baseline')
E.Alias('data')

E.Alias('run')
E.Depends('run',['baseline','data'])
E.Alias('metrics')
E.Depends('metrics',['baseline','data'])
E.Alias('report')
E.Depends('report',['metrics'])
E.Alias('all')
E.Depends('all',['report','metrics','run','data','baseline'])

Default('baseline')

################################################################################
kern_alg_list = [ka.split('/') for ka in ms_names]

##### FIXME: something about finding/generating scale parameter file?

##### Generate scaled MachSuite
for (k,a) in kern_alg_list:
  trans_dict = E_emit.Scale( [pjoin(emit_dir,k,a,'trans.dict')],
                             [pjoin(template_dir,k,a,'scale.py'),
                              E_emit['scale']] )
  for f in [k+'.c', k+'.h', 'local_support.c', 'generate.c','hls.tcl']:
    E_emit.Emit( [pjoin(emit_dir,k,a,f)],
                 [trans_dict,
                  pjoin(template_dir,k,a,f)] )
    E.Depends('baseline', pjoin(emit_dir,k,a,'hls.tcl'))
  # FIXME: backprop is glitched (has extra files)
  # this is a terrible hack to get that working.
  if k=='backprop':
    E_emit.Emit( [pjoin(emit_dir,'backprop','backprop','sol.h')],
                 [trans_dict,
                  pjoin(template_dir,'backprop','backprop','sol.h')] )
    E_emit.Emit( [pjoin(emit_dir,'backprop','backprop','train.h')],
                 [trans_dict,
                  pjoin(template_dir,'backprop','backprop','train.h')] )
    E.Depends( [pjoin(emit_dir,'backprop','backprop','generate')],
               [pjoin(emit_dir,'backprop','backprop','train.h'),
                pjoin(emit_dir,'backprop','backprop','sol.h')] )
for f in ['harness.c','support.c','support.h']:
  E_emit.Emit( [pjoin(emit_dir,'common',f)],
               [trans_dict,
                pjoin(template_dir,'common',f)] )

##### Generate Input Data
for (k,a) in kern_alg_list:
  gen = E_machsuite.CompileMS(
    [pjoin(emit_dir,k,a,'generate')],
    [pjoin(emit_dir,k,a,'generate.c'),
     pjoin(emit_dir,k,a,k+'.c'),
     pjoin(emit_dir,k,a,'local_support.c'),
     pjoin(emit_dir,'common','support.c')] )
  E.Depends(gen, pjoin(emit_dir,k,a,k+'.h'))
  E.Depends(gen, pjoin(emit_dir,'common','support.h'))
  input = E_machsuite.RunGenerate(pjoin(emit_dir,k,a,'input.data'), gen)
  E.Depends('data', input)

##### Build and run native MachSuite binaries
E_machsuite.Append(CFLAGS=['-I'+pjoin(emit_dir,k,a)])
for (k,a) in kern_alg_list:
  # Build
  bin = E_machsuite.CompileMS(
    [pjoin(emit_dir,k,a,k)],
    [pjoin(emit_dir,k,a,k+'.c'),
     pjoin(emit_dir,k,a,'local_support.c'),
     pjoin(emit_dir,'common','support.c'),
     pjoin(emit_dir,'common','harness.c')] )
  E.Depends(bin, pjoin(emit_dir,k,a,k+'.h'))
  E.Depends(bin, pjoin(emit_dir,'common','support.h'))
  E.Depends('baseline',bin)
  # Run
  out = E_machsuite.RunMS(
    [pjoin(emit_dir,k,a,'output.data')],
    [bin, pjoin(emit_dir,k,a,'input.data')] )
  E.Depends('run', out)

##### Build tools
insn_lib = E_tool.CompileTool(
  [pjoin(tools_dir,'instrumentation','insn_tool')],
  [pjoin(tools_dir,'instrumentation','insn_tool.cpp')] )
foot_lib = E_tool.CompileTool(
  [pjoin(tools_dir,'instrumentation','foot_tool')],
  [pjoin(tools_dir,'instrumentation','foot_tool.cpp')] )

##### Build instrumented MachSuite binaries
E_insn.Append(CFLAGS=' -Xclang -load -Xclang '+str(insn_lib[0])+' ')
E_foot.Append(CFLAGS=' -Xclang -load -Xclang '+str(foot_lib[0])+' ')
for (k,a) in kern_alg_list:
  insn_bin = E_insn.CompileMS(
    [pjoin(emit_dir,k,a,k+'-insn')],
    [pjoin(emit_dir,k,a,k+'.c'),
     pjoin(emit_dir,k,a,'local_support.c'),
     pjoin(emit_dir,'common','support.c'),
     pjoin(emit_dir,'common','harness.c')] )
  E.Depends(insn_bin, insn_lib)
  foot_bin = E_foot.CompileMS(
    [pjoin(emit_dir,k,a,k+'-foot')],
    [pjoin(emit_dir,k,a,k+'.c'),
     pjoin(emit_dir,k,a,'local_support.c'),
     pjoin(emit_dir,'common','support.c'),
     pjoin(emit_dir,'common','harness.c')] )
  E.Depends(foot_bin, foot_lib)

  E.Depends('metrics',insn_bin)
  E.Depends('metrics',foot_bin)

##### Run instrumented MachSuite binaries and collect data
# FIXME
if 0:
  if config['metric_flag']:
    for (k,a) in kern_alg_list:
      Run( pjoin(emit_dir,k,a,k), env=insn_env )
      Copy( pjoin(emit_dir,k,a,'insn.data'),
            pjoin(data_dir,'insn_'+k+'/'+a+'.data') )
      Run( pjoin(emit_dir,k,a,k), env=foot_env )
      Copy( pjoin(emit_dir,k,a,'foot.data'),
            pjoin(data_dir,'foot_'+k+'/'+a+'.data') )

##### Generate Reports
# FIXME
#  if metric_flag:
#    [ Collect data from all K/A, all scales ]
#    [ Run plotting scripts ]

