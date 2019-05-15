# My meson build file for the tutorials from http://www.learnopengl.com

project('LearnOpenGL', 'c', 'cpp', default_options: ['cpp_std=c++11'])

opengl = dependency('GL')
glfw = dependency('glfw3')
thread = dependency('threads')
xorg = dependency('x11')
xrandr = dependency('xrandr')
xi = dependency('xi')

glad_targ = custom_target('glad',
input: 'dlglad.sh',
output: ['glad.c','glad.h', 'khrplatform.h'],
command: ['zsh', '@INPUT@'],
install: false)

glad_dep = declare_dependency(sources: [glad_targ], link_args: ['-ldl'])

config = configuration_data()
config.set('TUT2_VERTEX_SHADER', '"tut2.vp"')
config.set('TUT2_FRAGMENT_SHADER', '"tut2.fp"')
configure_file(output: 'config.h', configuration: config)

executable('tut1', 'tut1.cpp', dependencies: [opengl, glfw, thread, xorg, xrandr, xi, glad_dep], link_args: ['-ldl'])
executable('tut2', 'tut2.cpp', dependencies: [opengl, glfw, thread, xorg, xrandr, xi, glad_dep], link_args: ['-ldl'])
install_data('tut2.vp', 'tut2.fp', install_dir: '.')