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
ms_names = ['aes/aes','viterbi/viterbi'] # FIXME: remove; debug
ms_names = ['aes/aes'] # FIXME: remove; debug
#ms_aliases = {kern: kern+'/'+alg for (kern,alg) in [kern_alg.split('/') for kern_alg in ms_names] if kern==alg}
scale_values = range(1,10) # FIXME?
scale_values = [1] # FIXME: remove; debug

### Utility ####################################################################
def pjoin(*args):
  return reduce(os.path.join, args)

### Platform-specific ##########################################################
platform = ARGUMENTS.get('OS',Platform())

### Base Environment ###########################################################
E = Environment(
)

E.Replace(CC='cc')
E.Replace(CXX='c++')

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
#cli_vars.Add('scale', 'Generate and builds benchmarks at scale n', 'all')
#cli_vars.Add(ListVariable('benchmark', 'Builds the only the specified benchmarks', default=ms_names, names=ms_names, map=ms_aliases))
#cli_vars.Add(BoolVariable('data', 'Generate input and reference files', 0))
#cli_vars.Add(BoolVariable('metrics', 'Build and run performance metrics', 0))

cli_vars.Update(E)

### Builders ###################################################################
def Scale(target,source,env):
  (trans_dict,) = map(str,target)
  (scalefile,) = map(str,source)
  tools.generate.scale_files(scalefile, env['SCALE'], trans_dict)
  return 0
Scale_Builder = Builder(action=Scale)

def Emit(target,source,env):
  (outfile,) = map(str,target)
  (trans_dict, infile) = map(str,source)
  tools.generate.translate_files(trans_dict, infile, outfile)
  return 0
Emit_Builder = Builder(action=Emit)

CompileMS_Builder = Builder(action='$CC $CFLAGS $SOURCES -o $TARGET $LDFLAGS')

def add_shlib_suffix(target,source,env):
  return ([str(t)+'$SHLIBSUFFIX' for t in target], source)
CompileTool_Builder = Builder(action='$CXX $CXXFLAGS $SOURCE -o $TARGET $LDFLAGS', emitter=add_shlib_suffix)


### Environment Setup ##########################################################
E_emit = E.Clone()
E_emit['BUILDERS']['Scale'] = Scale_Builder
E_emit['BUILDERS']['Emit'] = Emit_Builder

E_machsuite = E.Clone()
E_machsuite['BUILDERS']['CompileMS'] = CompileMS_Builder
E_machsuite.Append(CFLAGS=['-O2','-Wall','-Wno-unused-label'])
E_machsuite.Append(CFLAGS=['-I'+pjoin(emit_dir,'common')])

E_tool = E.Clone()
#E_tool.Replace(CXX=pjoin(E_tool['LLVM_ROOT'],'clang++')) # FIXME? I think this should work with any modern compiler
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

E_insn = E_machsuite.Clone()
E_insn.Replace(CC=pjoin(E['LLVM_ROOT'],'bin','clang'))
E_insn['BUILDERS']['CompileMS'] = CompileMS_Builder
E_foot = E_machsuite.Clone()
E_foot.Replace(CC=pjoin(E['LLVM_ROOT'],'bin','clang'))
E_foot['BUILDERS']['CompileMS'] = CompileMS_Builder

## MachSuite environment
#E_ms = E.Clone()
#E_ms.Append(CCFLAGS=['-Wno-unused-label'])
#E_ms.Append(CCFLAGS=['-I'+pjoin(bench_dir, 'common')])
#
## Tool-build environment
#E_tool = E.Clone()
#E_tool.Replace(CXX='clang++')
#E_tool.Append(CXXFLAGS='`llvn-config --cxxflags`')
#E_tool.Append(LIBLINKPREFIX='`llvm-config --ldconfig`')
#
## Tool-run envionrment
##  conf = Configure(tool_env)
##  # check for llvm>3.7
##  tool_env = conf.Finish()
#E_insn = E.Clone()
#E_insn.Append(CFLAGS='-Xclang -load -Xclang insn_tool.so') # FIXME
#E_insn.Replace(EXTENSION='-insn')
#E_foot = E.Clone()
#E_foot.Append(CFLAGS='-Xclang -load -Xclang insn_tool.so') # FIXME
#E_foot.Replace(EXTENSION='-insn')

### Targets ####################################################################
E.Alias('baseline')
E.Alias('data')

E.Alias('check')
E.Depends('check',['baseline','data'])
E.Alias('metrics')
E.Depends('metrics',['baseline','data'])
E.Alias('report')
E.Depends('report',['metrics'])
E.Alias('all')
E.Depends('all',['report','metrics','check','data','baseline'])

Default('baseline')

################################################################################
kern_alg_list = [ka.split('/') for ka in ms_names]

for scale in scale_values:
  ##### Generate scaled MachSuite
  E_emit.Replace(SCALE=scale)
  for (k,a) in kern_alg_list:
    trans_dict = E_emit.Scale( [pjoin(emit_dir,k,a,'trans.dict')],
                               [pjoin(template_dir,k,a,'scale.py')] )
    for f in [k+'.c', k+'.h', 'local_support.c', 'generate.c']:
      E_emit.Emit( [pjoin(emit_dir,k,a,f)],
                   [trans_dict,
                    pjoin(template_dir,k,a,f)] )
  for f in ['harness.c','support.c','support.h']:
    E_emit.Emit( [pjoin(emit_dir,'common',f)],
                 [trans_dict,
                  pjoin(template_dir,'common',f)] )


  ##### Build native MachSuite binaries
  E_machsuite.Append(CFLAGS=['-I'+pjoin(emit_dir,k,a)])
  #E_machsuite['BUILDERS']['CompileMS'] = Builder(action='$CC $CXXFLAGS $SOURCE -o $TARGET $LDFLAGS')
  for (k,a) in kern_alg_list:
    bin = E_machsuite.CompileMS(
      [pjoin(emit_dir,k,a,k)],
      [pjoin(emit_dir,k,a,k+'.c'),
       pjoin(emit_dir,k,a,'local_support.c'),
       pjoin(emit_dir,'common','support.c'),
       pjoin(emit_dir,'common','harness.c')] )
    E.Depends(bin, pjoin(emit_dir,k,a,k+'.h'))
    E.Depends(bin, pjoin(emit_dir,'common','support.h'))
    E.Depends('baseline',bin)

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

if 0:
  if config['metric_flag']:
    for (k,a) in kern_alg_list:
      Build( pjoin(emit_dir,k,a), env=insn_env )
      Build( pjoin(emit_dir,k,a), env=foot_env )

if 0:
  ##### Prepare Data
  if config['data_flag']:
    for (k,a) in kern_alg_list:
      Build( pjoin(emit_dir,k,a,'generate'), env=E )
    for (k,a) in kern_alg_list:
      Run( pjoin(emit_dir,k,a,'generate'), env=E )
  else:
    for (k,a) in kern_alg_list:
      Copy( pjoin(template_dir,k,a,'input.data'),
            pjoin(emit_dir,k,a,'input.data') )
      Copy( pjoin(template_dir,k,a,'check.data'),
            pjoin(emit_dir,k,a,'check.data') )

  ##### Run
  if config['check_flag']:
    for (k,a) in kern_alg_list:
      Run( pjoin(emit_dir,k,a,k), env=E )
  if config['metric_flag']:
    for (k,a) in kern_alg_list:
      Run( pjoin(emit_dir,k,a,k), env=insn_env )
      Copy( pjoin(emit_dir,k,a,'insn.data'),
            pjoin(data_dir,'insn_'+k+'/'+a+'.data') )
      Run( pjoin(emit_dir,k,a,k), env=foot_env )
      Copy( pjoin(emit_dir,k,a,'foot.data'),
            pjoin(data_dir,'foot_'+k+'/'+a+'.data') )

#  ##### Report
#  if metric_flag:
#    [ Collect data from all K/A, all scales ]
#    [ Run plotting scripts ]

