#!/usr/bin/python3

from optparse import OptionParser
import sys
import os

parser = OptionParser()
parser.add_option("-c", "--clean", action="store_true", dest="gitclean", help="Do a git clean on the build folder", default=False)
parser.add_option("-d", "--demo", action="store_true", dest="cmake", help="Use cmake to build.", default=False)
parser.add_option("-e", "--extra-warnings", action="store_true", dest="extra", help="Enable -Wextra and -Weffc++ warnings", default=False)
parser.add_option("-g", "--gcc", action="store_true", dest="gcc", help="Use gcc compiler instead of clang", default=False)
parser.add_option("-o", "--optimization", dest="optimization", help="Enable optimization level", type="int", default=3, metavar="LEVEL")
parser.add_option("-l", "--logging", action="store_true", dest="logging", help="Enable network logging", default=False)

(options, args) = parser.parse_args()

try:
    os.environ['DESKTOP_PREFIX']
except:
    print("Environment not set")
    sys.exit(-1)

if options.gitclean:
    ret = os.system("git clean -dxf")
    if ret != 0:
        sys.exit(ret)

extra_warnings=''
if options.extra:
    extra_warnings='-Wextra -Weffc++'

ccache = 'ccache'
compiler = 'clang++-6.0'
if options.gcc:
    compiler = 'g++-6'


os.environ['INSTALL']='./install-sh -c -C'
os.environ['CXX']=ccache + ' ' + compiler
os.environ['CXXFLAGS']='-O' + str(options.optimization) + ' -Wall -Wno-unused-parameter -std=c++14 -g' + extra_warnings

if options.cmake:
    configure_command="cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX . -DCMAKE_BUILD_TYPE=Debug"
else:
    configure_command="autoreconf --install && ./configure --prefix=$DESKTOP_PREFIX --enable-debug=yes --enable-profile=UBUNTU --enable-vulkan=1.0"

if options.logging:
    configure_command += " --enable-networklogging"

make_command="make install -j21"

ret = os.system(configure_command)

if ret == 0:
    os.system(make_command)
else:
    sys.exit(ret)
