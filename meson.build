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

deplist = [opengl, glfw, thread, xorg, xrandr, xi, glad_dep]

# Tutorial 1: Hello Window
executable('tut1', 'tut1.cpp', dependencies: deplist, link_args: ['-ldl'])

# Tutorial 2: Hello Triangle
executable('tut2.1', 'tut2.1.cpp', dependencies: deplist, link_args: ['-ldl'])
# Part 2: Rectangle
executable('tut2.2', 'tut2.2.cpp', dependencies: deplist, link_args: ['-ldl'])
# Part 3: Element buffer objects
executable('tut2.3', 'tut2.3.cpp', dependencies: deplist, link_args: ['-ldl'])
run_command('cp', ['-t', meson.build_root(), files('tut2.fp', 'tut2.vp')])

# Tutorial 3: GLSL shaders
config.set('TUT3_VERTEX_SHADER', '"tut3.vp"')
config.set('TUT3_FRAGMENT_SHADER', '"tut3.fp"')
config.set('TUT32_FRAGMENT_SHADER', '"tut3.2.fp"')
config.set('TUT33_VERTEX_SHADER', '"tut3.3.vp"')
config.set('TUT33_FRAGMENT_SHADER', '"tut3.3.fp"')
config.set('TUT332_VERTEX_SHADER', '"tut3.3.2.vp"')
config.set('TUT332_FRAGMENT_SHADER', '"tut3.3.2.fp"')
executable('tut3.1', 'tut3.1.cpp', dependencies: deplist, link_args: ['-ldl'])
executable('tut3.2', 'tut3.2.cpp', dependencies: deplist, link_args: ['-ldl'])
executable('tut3.3', 'tut3.3.cpp', 'shader.cpp', dependencies: deplist, link_args: ['-ldl'])
executable('tut3.3.2', 'tut3.3.2.cpp', 'shader.cpp', dependencies: deplist, link_args: ['-ldl'])
run_command('cp', ['-t', meson.build_root(), files('tut3.fp', 'tut3.2.fp', 'tut3.vp', 'tut3.3.vp', 'tut3.3.fp', 'tut3.3.2.vp', 'tut3.3.2.fp')])

# Tutorial 4: Textures
executable('tut4', 'tut4.cpp', 'shader.cpp', dependencies: deplist, link_args: ['-ldl'])
executable('tut4.2', 'tut4.2.cpp', 'shader.cpp', dependencies: deplist, link_args: ['-ldl'])
executable('tut4.3', 'tut4.3.cpp', 'shader.cpp', dependencies: deplist, link_args: ['-ldl'])
executable('tut4.4', 'tut4.4.cpp', 'shader.cpp', dependencies: deplist, link_args: ['-ldl'])
executable('tut4.5', 'tut4.5.cpp', 'shader.cpp', dependencies: deplist, link_args: ['-ldl'])
run_command('cp', ['-t', meson.build_root(), files('tut4.vp', 'tut4.fp', 'tut4.2.fp', 'tut4.2.1.fp', 'tut4.5.fp', 'dirbri18.png', 'awesomeface.png')])

# Tutorial 5: Transformations
executable('tut5', 'tut5.cpp', 'shader.cpp', 'kmatrix.cpp', dependencies: deplist, link_args: ['-ldl'])
run_command('cp', ['-t', meson.build_root(), files('tut5.vp')])

configure_file(output: 'config.h', configuration: config)