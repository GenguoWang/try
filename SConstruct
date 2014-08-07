import os
home_dir = os.environ["HOME"]
env = Environment()
env['CPPPATH'] = ['#include','%s/Softwares/include' % (home_dir)]
env['LIBPATH'] = ['%s/Softwares/lib' % (home_dir), '#common']
env['CCFLAGS'] = ['-Wall','-O3']
env.AppendUnique(LIBS=["boost_system","boost_thread", "boost_program_options", "gmp", "gmpxx", "boost_filesystem","crypto"])
Export('env')
SConscript(['tryFork/SConscript'])
SConscript(['verify_index/SConscript'])
