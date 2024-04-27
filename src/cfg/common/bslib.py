#---------------------------------------------------t----------------------------
#
#    Build System Library
#
#    Author: Harry E. Zhurov
#
#-------------------------------------------------------------------------------

import os
import time

from utils import *

#-------------------------------------------------------------------------------
def process_prjopt(bld):
    opts     = bld.envx['PROJECT_OPTIONS']

    if not opts:
        bld.envx.Append( PRJ_MACRODEFS = [] )
        return

    bld.prjopten  = [x + '_ENABLE=' + opts[x] for x in opts]
    bld.prjoptuse = [x + '_OPTION_USE' for x in opts if opts[x] == '1']

    prjoptvlog = 'parameters :' + os.linesep
    prjopttcl  = 'set_property verilog_define { \\' + os.linesep

    max_pn_len = max_str_len([k + '_ENABLE' for k in opts.keys()])
    for i in opts:
        k = i + '_ENABLE'
        v = opts[i]
        value_len     = len(v)
        name_padding  = max_pn_len - len(k) + 2
        prjoptvlog   += ' '*4 + k + ' '*name_padding + ' : ' + v + os.linesep
        prjopttcl    += k + '=' + v + ' \\' + os.linesep

    prjoptvlog += os.linesep

    for k in bld.prjoptuse:
        prjopttcl  +=  k + ' \\' + os.linesep

    prjopttcl += '} [current_fileset]' + os.linesep

    prjoptvlog_path = os.path.join(bld.envx['BUILD_SRC_PATH'], 'prjopts.yml')
    prjopttcl_path  = os.path.join(bld.envx['BUILD_SRC_PATH'], 'prjopts.tcl')
    if not os.path.exists(bld.envx['BUILD_SRC_PATH']):
        Execute( Mkdir(bld.envx['BUILD_SRC_PATH']) )

    with open(prjoptvlog_path, 'w') as vlog_fh:
        vlog_fh.write(prjoptvlog)
        vlog_fh.flush()
        os.fsync(vlog_fh.fileno())

    with open(prjopttcl_path, 'w') as tcl_fh:
        tcl_fh.write(prjopttcl)
        tcl_fh.flush()
        os.fsync(tcl_fh.fileno())
        
    while(not os.path.exists(prjoptvlog_path)):
        pass

    bld.envx.Append( PRJ_MACRODEFS = bld.prjopten + bld.prjoptuse)

#-------------------------------------------------------------------------------
def process_arguments(bld):
    bld.no_colors = int(bld.envx['ARGUMENTS'].get('no_colors', 0))
    bld.sim_args  = str(bld.envx['ARGUMENTS'].get('sim_args', ''))

#-------------------------------------------------------------------------------
def set_sv_seed(bld):
    seed = bld.envx['ARGUMENTS'].get('sv_seed', '')

    sv_seed = seed if seed else str(int(time.time()))

    bld.envx['SV_SEED'] = sv_seed
    print(colorize(os.linesep + ' '*12 + '*'*40, 'green', True))
    print(colorize(' '*12 + '*' + ' '*10 + 'sv_seed: ' + sv_seed + ' '*9 + '*', 'green', True))
    print(colorize(' '*12 + '*'*40, 'green', True)+os.linesep)

#-------------------------------------------------------------------------------

