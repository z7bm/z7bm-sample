#-------------------------------------------------------------------------------
#
#    Root construction script
#
#    Author: Harry E. Zhurov
#
#-------------------------------------------------------------------------------

import os
import sys

sys.dont_write_bytecode = True

import sys

sys.path.append( '.scons_ext' )

from helpers import set_comstr
#-------------------------------------------------------------------------------
#
#    Help info
#
help_info ="""
********************************************************************************     
    Available variants:
    ~~~~~~~~~~~~~~~~~~~
    zed
     
    Usage:
    ~~~~~  
    scons [options] [variant|bv=<[path/]name>] [targets]
"""

Help(help_info)

#-------------------------------------------------------------------------------
#
#    General Settings
#

#-------------------------------------------------------------------------------
#
#    Variant management
#
if 'bv' in ARGUMENTS:
    variant = ARGUMENTS.get('bv')
    ARGUMENTS['variant'] = variant
elif 'variant' in ARGUMENTS:
    variant = ARGUMENTS.get('variant')
    ARGUMENTS['bv'] = variant
else:
    print_error('\nError: build variant must be specified via \'variant=<variant name>\' or \'bv=<variant name>\' CLI argument')

variant_name = variant.split(os.sep)[-1]

print_info('*'*80)
print_info(' '*27 + 'build variant: ' + variant_name)
print_info('*'*80 + '\n')

variant_path = os.path.join('src', 'cfg', variant, variant_name + '.scons')

if not os.path.exists(variant_path):
    print_error('\nError: unsupported variant: ' + variant)
    print(help_info)
    Exit(-3)

#-------------------------------------------------------------------------------
#
#    Project Options (Features)
#
optnames = []
options  = {}

#-----------------------------------------------------------
#
#    Retrieve optins from environment variables
#
for name in optnames:
    key = 'PREFIX_' + name + '_ENABLE'
    if key not in os.environ:
        print_error('\nError: project option "' + name + '" not specified\n')
        print_info('The follwing envrionment variables should be defined:')
        for i in optnames:
            print_info('    PREFIX_' + i + '_ENABLE')
        Exit(-4)

    val = os.environ[key].lower()

    pos = ['1', 'yes', 'true']
    neg = ['0', 'no',  'false']

    if val in pos:
        options[name] = '1'
    elif val in neg:
        options[name] = '0'
    else:
        print_error('\nError: project option "' + name + '" has invalid value: "' + val +'"')
        print_error('      Valid values are: ' + ', '.join(['"{}"'.format(i) for i in pos+neg]) )
        Exit(-4)

#-----------------------------------------------------------
#
#    Process command-line options settings
#
opten  = []
optdis = []

if 'opten' in ARGUMENTS and ARGUMENTS['opten']:
    opten =  [i.strip().upper() for i in ARGUMENTS['opten'].split(',')]

if 'opten' in ARGUMENTS and ARGUMENTS['optdis']:
    optdis = [i.strip().upper() for i in ARGUMENTS['optdis'].split(',')]

if len(opten) and len(optdis) and not set(opten).isdisjoint(set(optdis)):
    com_items = list(set(opten) & set(optdis))
    print_error(os.linesep + 'Error: enable and disable options lists has common items: ' +
                 ', '.join(['"{}"'.format(i.strip()) for i in com_items])  + os.linesep)
    Exit(-4)

opten  = [i for i in opten  if i]
optdis = [i for i in optdis if i]
for i in opten + optdis:
    if i not in options:
        print_error('\nError: command-line project option "' + i + '" has invalid name')
        print_error('      Valid names are: ' + ', '.join(['"{}"'.format(i) for i in options]) )
        Exit(-4)

#-----------------------------------------------------------
#
#    Overlay command-line options above default
#
for i in opten:
    options[i] = '1'

for i in optdis:
    options[i] = '0'

opten  = [i for i in options if options[i] == '1']
optdis = [i for i in options if options[i] == '0']

#-----------------------------------------------------------
#
#    Display Project Option Summary
#

maxlen = max(len(opten), len(optdis))
opten.extend(['']*(maxlen-len(opten)))
optdis.extend(['']*(maxlen-len(optdis)))

INDENT = 20
print(colorize(' '*(INDENT-4) + 'Project Options for Current Build', 'yellow', True), os.linesep)
from tabulate import tabulate as tab

headers = [colorize('ENABLED', 'green', True), colorize('DISABLED', 'red', True)]
opttab  = tab([(colorize(x, 'green', True), colorize(y, 'red', True)) for x, y in zip(opten, optdis)], headers,
                tablefmt='fancy_grid')

print(' '*INDENT + opttab.replace('\n', '\n'+' '*INDENT))

#-------------------------------------------------------------------------------
#
#    Environment
#
envx = Environment() #( tools = {} )

envx['BUILD_VARIANT']   = variant
envx['PROJECT_OPTIONS'] = options

set_comstr(envx)

#SConscript(variant_path, exports='envx', variant_dir = '#build/' + variant_name, duplicate = 0)
SConscript(variant_path, exports='envx')

#-------------------------------------------------------------------------------

if 'dump' in ARGUMENTS:
    env_key = ARGUMENTS[ 'dump' ]
    if env_key == 'env':
        print( envx.Dump() )
    else:
        print( envx.Dump(key = env_key) )

#-------------------------------------------------------------------------------

