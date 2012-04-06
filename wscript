srcdir = '.'
blddir = 'build'
VERSION = '0.0.1'

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')
  # conf.check_cfg(package='gdk-pixbuf-2.0', args='--cflags --libs', mandatory=True)

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'tiff-multipage'
  obj.source = './src/libtiff.cc'
  obj.lib = 'tiff'
  #obj.uselib = ['GDK-PIXBUF-2.0']
