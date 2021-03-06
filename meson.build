# My meson build file for the tutorials from http://www.learnopengl.com

project('LearnOpenGL', 'c', 'cpp', default_options: ['cpp_std=c++11'])

opengl = dependency('GL')
glfw = dependency('glfw3')
thread = dependency('threads')
xorg = dependency('x11')
xrandr = dependency('xrandr')
xi = dependency('xi')
sdl = dependency('SDL2')
sdl_image = dependency('SDL2_image')

glad_targ = custom_target('glad',
input: 'dlglad.sh',
output: ['glad.c','glad.h', 'khrplatform.h'],
command: ['zsh', '@INPUT@'],
install: false)

glm_path = include_directories('glm')

glad_dep = declare_dependency(sources: [glad_targ], link_args: ['-ldl'])

config = configuration_data()

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
executable('tut3.1', 'tut3.1.cpp', dependencies: deplist, link_args: ['-ldl'])
executable('tut3.2', 'tut3.2.cpp', dependencies: deplist, link_args: ['-ldl'])
executable('tut3.3', 'tut3.3.cpp', 'shader.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
executable('tut3.3.2', 'tut3.3.2.cpp', 'shader.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
run_command('cp', ['-t', meson.build_root(), files('tut3.fp', 'tut3.2.fp', 'tut3.vp', 'tut3.3.vp', 'tut3.3.fp', 'tut3.3.2.vp', 'tut3.3.2.fp')])

# Tutorial 4: Textures
executable('tut4', 'tut4.cpp', 'shader.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
executable('tut4.2', 'tut4.2.cpp', 'shader.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
executable('tut4.3', 'tut4.3.cpp', 'shader.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
executable('tut4.4', 'tut4.4.cpp', 'shader.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
executable('tut4.5', 'tut4.5.cpp', 'shader.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
run_command('cp', ['-t', meson.build_root(), files('tut4.vp', 'tut4.fp', 'tut4.2.fp', 'tut4.3.fp', 'tut4.5.fp', 'dirbri18.png', 'awesomeface.png')])

# Tutorial 5: Transformations
executable('tut5', 'tut5.cpp', 'shader.cpp', 'kmatrix.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
executable('tut5.1', 'tut5.1.cpp', 'shader.cpp', 'kmatrix.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
executable('tut5.2', 'tut5.2.cpp', 'shader.cpp', 'kmatrix.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
run_command('cp', ['-t', meson.build_root(), files('tut4.2.1.fp', 'tut5.vp')])

# Tutorial 6: Coordinate systems
executable('tut6', 'tut6.cpp', 'shader.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
executable('tut6.1', 'tut6.1.cpp', 'shader.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
executable('tut6.2', 'tut6.2.cpp', 'shader.cpp', include_directories: glm_path, dependencies: deplist, link_args: ['-ldl'])
executable('tut6.3', 'tut6.3.cpp', 'shader.cpp', include_directories: glm_path, dependencies: [opengl, sdl, sdl_image, glad_dep])
run_command('cp', ['-t', meson.build_root(), files('tut6.vp', 'tut6.fp', '2d.vp', '2d.fp', 'bitmapfont.png')])