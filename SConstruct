import os
home_dir = os.environ["HOME"]
print home_dir
env = Environment()
env['CPPPATH'] = ['#include','%s/Softwares/include' % (home_dir)]
env['LIBPATH'] = ['%s/Softwares/lib' % (home_dir), '#common']
env.AppendUnique(LIBS=["ntl", "boost_system", "boost_program_options", "gmp"])
Export('env')
SConscript(['common/SConscript'])
SConscript(['tryFork/SConscript'])
