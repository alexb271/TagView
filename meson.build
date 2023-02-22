project('TagView', 'cpp', default_options : 'cpp_std=c++17', version : '0.1')
gtkdep = dependency('gtkmm-4.0', version: '>= 4.6')

# src_files declared in subfolder 'src'
subdir('src')

executable('tagview', src_files, dependencies: gtkdep)
