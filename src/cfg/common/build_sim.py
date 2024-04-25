
import os
from pathlib import Path
from utils   import *

#-------------------------------------------------------------------------------
def compile_sc(target, source, env):
    
    trg     = target[0]
    trg_dir = str(trg.dir)

    dpp     = import_config('dpp.yml')
    env     = import_config('env.yml')
    params  = import_config('params.yml')
    general = import_config('main.yml')
    env     = import_config('env.yml')

    HLS_DIR = os.path.join(os.path.abspath(str(Dir('#'))), 'src', 'hls', 'dpu')

    # cpp sources
    src_csim = read_sources('src/cfg/common/hls/dpu/src_csim.yml')
    src_csim = ' '.join(src_csim)

    cxxflags = []
    incpath  = []
    libpath  = []
    libs     = []


    cxxflags.append(' -DSIM')
    cxxflags.append(' -DAXIS_WIDTH='            + str(dpp.AXIS_WIDTH))
    cxxflags.append(' -DFW_ENABLE='             + str(os.environ["FG_FW_ENABLE"]))
    cxxflags.append(' -DFW_LOG_ENABLE='         + str(os.environ["FG_FW_LOG_ENABLE"]))
    cxxflags.append(' -DIDPS_ENABLE='           + str(os.environ["FG_IDPS_ENABLE"]))
    cxxflags.append(' -DDOS_ENABLE='            + str(os.environ["FG_DOS_ENABLE"]))
    cxxflags.append(' -DDPP_UTM_IF='            + str(params.UTM_PORT_INDEX))
    cxxflags.append(' -DDPP_DROP_IF='           + str(params.DROP_PORT_INDEX))
    cxxflags.append(' -DDPP_OWN_MAC='           + str(dpp.OWN_MAC))
    cxxflags.append(' -DDPP_UTM_MAC='           + str(dpp.UTM_MAC))
    cxxflags.append(' -DDPP_OWN_IP='            + str(dpp.OWN_IP))
    cxxflags.append(' -DDPP_UTM_IP='            + str(dpp.UTM_IP))
    cxxflags.append(' -DDPP_SNMP_IF='           + str(params.SNMP_MODULE_INDEX))
    cxxflags.append(' -DDPP_IDPS_IF='           + str(params.IDPS_MODULE_INDEX))
    cxxflags.append(' -DDPP_IDPS_CONFIG_IF='    + str(params.IDPS_MODULE_INDEX))
    cxxflags.append(' -DDPP_IDPS_MP_CONFIG_IF=' + str(params.IDPS_MP_MODULE_INDEX))
    cxxflags.append(' -DDPP_QOS_CONFIG_IF='     + str(params.QOS_MODULE_INDEX))
    cxxflags.append(' -DDPP_SDPE_CONFIG_IF='    + str(params.VPN_MODULE_INDEX))
    cxxflags.append(' -DPHY_IF_COUNT='          + str(params.PHY_IF_COUNT))
    cxxflags.append(' -DDPP_LOOKUP_CTRL_SIZE='  + str(dpp.LOOKUP_CTRL_SIZE))
    cxxflags.append(' -DDPP_LOOKUP_KEY_SIZE='   + str(dpp.LOOKUP_KEY_SIZE))
    cxxflags.append(' -DDPP_LOOKUP_VALUE_SIZE=' + str(dpp.LOOKUP_VALUE_SIZE))
    cxxflags.append(' -U__SYNTHESIS__'          + str(dpp.LOOKUP_VALUE_SIZE))
    cxxflags.append(' -g ')

    incpath.append(' -I' + os.path.join(env.XILINX_HLS, 'include'))
    incpath.append(' -I' + os.path.join(env.EXTERNAL_LIB, 'yaml-cpp', 'include'))
    incpath.append(' -I' + os.path.join(env.EXTERNAL_LIB, 'pcap++', 'include', 'pcapplusplus'))
    incpath.append(' -I' + os.path.join(HLS_DIR))
    incpath.append(' -I' + os.path.join(HLS_DIR, 'tb'))
    incpath.append(' -I' + os.path.join(HLS_DIR, 'tb', 'utils'))
    incpath.append(' -I' + os.path.join(HLS_DIR, 'tb', 'sc'))

    libpath.append(' -L' + os.path.join(env.EXTERNAL_LIB, 'yaml-cpp', 'lib'))
    libpath.append(' -L' + os.path.join(env.EXTERNAL_LIB, 'pcap++', 'lib'))

    libs.append(' -l' + 'yaml-cpp')
    libs.append(' -l' + 'Pcap++')
    libs.append(' -l' + 'Packet++')
    libs.append(' -l' + 'Common++')

    cxxflags = ''.join(cxxflags)
    incpath  = ''.join(incpath)
    libpath  = ''.join(libpath)
    libs     = ''.join(libs)

    msg = colorize('Compile SC src', 'yellow')
    print(colorize('-'*80, 'yellow'))
    print(' '*20, msg, os.linesep)

    # bind sv for c++
    cmd = 'vlib sclib'
    rcode = pexec(cmd, trg_dir)

    cmd = 'vmap sclib sclib'
    rcode = pexec(cmd, trg_dir)


    # cmd = 'scgenmod -lib sclib -bool sc_dut > sc_dut.h'
    # rcode = pexec(cmd, trg_dir)
    

    # compile c++ src

    cmd = 'sccom -work sclib ' + '-incr -j 16 ' + incpath + cxxflags + src_csim
    rcode = pexec(cmd, trg_dir)

    cmd = 'sccom -work sclib ' + '-incr' + incpath + cxxflags + os.path.join(HLS_DIR, 'tb', 'sc', 'sc_dut_tb.cpp')
    rcode = pexec(cmd, trg_dir)


    # link libs
    cmd = 'sccom -link -work sclib ' + libpath + libs
    rcode = pexec(cmd, trg_dir)


    print(colorize('-'*80, 'yellow'))

    if rcode:
        return rcode

    return None

#-------------------------------------------------------------------------------
def compile_tb_pkg(target, source, env):
    trg     = target[0]
    trg_dir = str(trg.dir)

    worklib  = ' -work tb_pkg'
    uvm_lib  = ' -L ' + env['UVM_PATH']
    sv_seed  = ' +define+SV_SEED={}'.format(env['SV_SEED'])
    incdir   = ' +incdir+{}+{}'.format(env['BUILD_SRC_PATH'], os.path.join(env['SRC_SIM'], 'lib', 'tb'))
    pkg_src  = ' ' + os.path.join(env['SRC_SIM'], 'lib', 'tb', 'tb_lib.sv')
    defines  =  ' +define+' + '+'.join(env['PRJ_MACRODEFS'])

    defines += '' if not 'VLOG_DEFINES' in env.Dictionary() else env['VLOG_DEFINES']

    cmd = env['VLOGCOM'] + worklib + uvm_lib + sv_seed + incdir + defines + pkg_src
    print(cmd)

    msg = colorize('Compile testbench package', 'yellow')
    print(colorize('-'*80, 'yellow'))
    print(' '*20, msg, os.linesep)

    rcode = pexec(cmd, trg_dir)
    print(colorize('-'*80, 'yellow'))

    if rcode:
        return rcode

    return None

#-------------------------------------------------------------------------------
def create_test_header(target, source, env):
    trg = target[0]
    test_name = env['TEST_NAME']

    search_path   = [Path(p) for p in env['SIM_INC_PATH']]
    test_filename = test_name + '.svh'
    test_header   = None
    for p in search_path:
        if p.joinpath(test_filename).is_file():
            test_header = str(p.joinpath(test_filename))
            break

    if not test_header:
        print_error('E: cannot find test source "' + test_filename + '" while create test header in search path list:')
        for p in search_path:
            print_error('    ' + str(p))
        Exit(-1)

    test_dir = test_header.split(test_name + '.svh')[0]
    text = '`include "' + test_header + '"\n'

    with open(os.path.join(env['BUILD_SIM_PATH'], 'test.svh'), 'w') as ofile:
        ofile.write(text)

    return None

#-------------------------------------------------------------------------------
#     Create links to IDPS simulation data
def link_idps_data(target, source, env):
    sim_path = env['BUILD_SIM_PATH']

    src_ram_init_path = os.path.join(env['ROOT_PATH'], 'src', 'syn', 'idps', 'ram_init')
    dst_ram_init_path = os.path.join(sim_path, 'ram_init')

    src_idps_data_path = os.path.join(env['ROOT_PATH'], 'src', 'sim', 'tests', 'idps', 'data', 'payload')
    dst_idps_data_path = os.path.join(sim_path, 'idps')

    if not os.path.exists(dst_ram_init_path):
        os.symlink(src_ram_init_path, dst_ram_init_path)

    if os.path.exists(src_idps_data_path) and not os.path.exists(dst_idps_data_path) :
        os.symlink(src_idps_data_path, dst_idps_data_path)

#-------------------------------------------------------------------------------
#     Show sv_seed value
def show_sv_seed(target, source, env):
    print("\033[33;40m=======================\033[0m")
    print("\033[33;40m  SV_SEED: {}\033[0m".format(env['SV_SEED']))
    print("\033[33;40m=======================\033[0m")
