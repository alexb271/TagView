project('TagView', 'cpp', default_options : 'cpp_std=c++17')
gtkdep = dependency('gtkmm-4.0', version: '>= 4.6')

# src_files declared in subfolder 'src'
subdir('src')

executable('app', src_files, dependencies: gtkdep)
