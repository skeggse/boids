#!/usr/bin/env python

from os.path import abspath, basename, join, splitext
from sys import argv

import itertools, subprocess

if len(argv) < 3:
  print('usage: pack-shaders.py <c preprocessor command> <output directory> [<source> ...]')
  exit(1)

cpp = argv[1][1:].split(' ')
dest = argv[2]
src = itertools.islice(argv, 3, len(argv))

def getsource(source):
  path, name = source
  yield b'const char %b_source[] = "' % name.encode()
  proc = subprocess.Popen(cpp + [path], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
  while True:
    chunk = proc.stdout.read(1024)
    yield b''.join(b'\\x%02x' % b for b in chunk)
    if len(chunk) < 1024 and proc.poll() is not None:
      break
  yield b'";\n'

def getsources(sources):
  return itertools.chain.from_iterable(map(getsource, sources))

filesa, filesb = itertools.tee((abspath(f), splitext(basename(f))[0]) for f in src)

with open(join(dest, 'boids-shaders.cc'), 'wb') as out:
  out.write(b'#include "boids-shaders.h"\n\n')
  for data in getsources(filesa):
    out.write(data)

with open(join(dest, 'boids-shaders.h'), 'wb') as out:
  for path, name in filesb:
    out.write(b'extern const char %b_source[];\n' % name.encode())
