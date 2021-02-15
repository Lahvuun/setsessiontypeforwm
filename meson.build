project('setsessiontypeforwm', 'c')

logind = dependency('libelogind', version : '>=246.9')

flags = ['-Wall','-Wextra', '-Wpedantic', '-Wconversion']
executable('setsessiontypeforwm',
  'src/main.c',
  c_args : flags,
  dependencies: logind,
  install: true)
