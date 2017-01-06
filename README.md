boids
=====

A gpu-accelerated implementation of a flocking algorithm.

This application implements the boids flocking model, as designed by [Craig Reynolds](http://www.red3d.com/cwr/boids/), and borrows constants from Andrew Merrill.

Also implements a gpu-accelerated spatial indexing algorithm.

Includes portions of supplementary code from Interactive Computer Graphics, Sixth Edition by Edward Angel and Dave Shreiner. These portions of code are licensed under the MIT license.

building
--------

dependencies:

- [Generate Your Projects (GYP)](https://gyp.gsrc.io/) to generate the build scripts
- [Python](https://www.python.org/) 3 to pack the shaders into the binary
- [FreeGLUT](http://freeglut.sourceforge.net/) or similar
- [GLEW](http://glew.sourceforge.net/)

### linux

```sh
$ gyp boids.gyp -f make --depth=.
$ make
```

### windows

Check out the [prebuilt binary](https://github.com/skeggse/boids/releases) copy of this application.

```sh
$ gyp boids.gyp -f msvs --depth=.
```

running
-------

### linux

```sh
$ ./out/Release/boids
```

license
-------

The contents of this repository are licensed under the MIT license. The following license applies to files without their own license header.

> The MIT License (MIT)
>
> Copyright &copy; 2017 Eli Skeggs
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
