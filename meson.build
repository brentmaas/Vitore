project(
    'Vitore',
    ['cpp'],
    default_options: [
        'cpp_std=c++20',
        'buildtype=debugoptimized',
    ],
)

sources = [
    'src/main.cpp',
    'src/shader.cpp',
]

shaders = [
    'shaders/shader.frag',
    'shaders/shader.vert',
]

glslang_validator = find_program('glslangValidator')
glsl_gen = generator(
    glslang_validator,
    output: '@PLAINNAME@.h',
    arguments: ['-G', '-o', '@OUTPUT@', '--quiet', '-g', '@INPUT@', '@EXTRA_ARGS@'],
)

shader_headers = []
foreach shader : shaders
    shader_name = shader.underscorify()
    shader_headers += glsl_gen.process(shader, extra_args: ['--vn', shader_name])
endforeach

dependencies = [
    subproject('glad').get_variable('glad_dep'),
    dependency('GL'),
    dependency('glfw3'),
    dependency('glm'),
    dependency('fmt'),
]

link_args = []
if host_machine.system() == 'windows'
    link_args += ['-mwindows']
elif host_machine.system() == 'linux'
    dependencies += meson.get_compiler('cpp').find_library('dl')
endif

executable(
    'vitore',
    [sources, shader_headers],
    dependencies: dependencies,
    install: true,
    build_by_default: true,
    include_directories: include_directories('include'),
)
