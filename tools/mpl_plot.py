import numpy as np

# MPL Boilerplate
import matplotlib
import matplotlib.pyplot as plt 
from matplotlib import rcParams
import matplotlib.cm as cm
import matplotlib.patches
import matplotlib as mpl 
def rgb(r,g,b):
    return (float(r)/256.,float(g)/256.,float(b)/256.)
# Plot colors:
#   visually distinct under colorblindness and grayscale
crimson = rgb(172,63,64)
blue    = rgb(62,145,189)
teal    = rgb(98,189,153)
orange  = rgb(250,174,83)
#   luminance channel sweeps from dark to light, (for ordered comparisons)
clr = [crimson, blue, teal, orange]
# 1000 continuous colors
cclr = [0]*1000
def cclr_sweep(c0,c1,n):
  (r0,g0,b0) = c0
  (r1,g1,b1) = c1
  return [((r1-r0)*i/n+r0, (g1-g0)*i/n+g0, (b1-b0)*i/n+b0) for i in xrange(0,n)]
cclr[   :333] = cclr_sweep(crimson,blue,333)
cclr[333:667] = cclr_sweep(blue,teal,334)
cclr[667:   ] = cclr_sweep(teal,orange,333)
mrk = ['o','D','^','s']
rcParams['figure.figsize'] = (8,6)
rcParams['figure.dpi'] = 150 
rcParams['axes.color_cycle'] = clr 
rcParams['lines.linewidth'] = 2
rcParams['lines.marker'] = '.' 
rcParams['lines.markeredgewidth'] = 0 
rcParams['lines.markersize'] = 0 
rcParams['axes.facecolor'] = 'white'
rcParams['font.size'] = 14
rcParams['patch.edgecolor'] = 'black'
rcParams['patch.facecolor'] = clr[0]
#rcParams['font.family'] = 'Helvetica'
rcParams['font.weight'] = 100 

# Non-notebook stuff
#matplotlib.use('Agg')
