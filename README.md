boids
=====

A gpu-accelerated implementation of a flocking algorithm.

This application implements a gpu-accelerated flocking model, dubbed boids. [Craig Reynolds](http://www.red3d.com/cwr/boids/) originally designed the model, and this implementation borrows constants from Andrew Merrill.

Includes portions of supplementary code from Interactive Computer Graphics, Sixth Edition by Edward Angel and Dave Shreiner. These portions of code are licensed under the MIT license.

building
--------

dependencies:

- [Generate Your Projects (GYP)](https://gyp.gsrc.io/) to generate the build scripts
- [Python](https://www.python.org/) 3 to pack the shaders into the binary
- [FreeGLUT](http://freeglut.sourceforge.net/) or similar
- [GLEW](http://glew.sourceforge.net/)
- OpenGL 4.3.0 or higher

### linux

```sh
$ gyp boids.gyp -f make --depth=.
$ make
```

### windows

Check out the [prebuilt binary](https://github.com/skeggse/boids/releases) copy of this application.

```sh
$ gyp boids.gyp -f msvs --depth=. --generator-output out
# you can also open this in Visual Studio and Build from there
$ msbuild out\boids.vcxproj
```

### mac

You're out of luck. Apple doesn't support anything greater than OpenGL 4.1 with a few 4.2 extensions on any versions of OS X.

running
-------

### linux

```sh
$ ./out/Release/boids
```

### windows

Run `boids.exe` in the `out` directory from the "building" step.

design
------

This application extends the basic algorithm described by Craig Reynolds. All algorithms are written mostly or entirely in compute shaders, and run on available GPU hardware.

### brute force

The brute force implementation measures the distance between each pair of boids twice - once for each boid. This performs at O(N<sup>2</sup>).

Switch to this implementation by pressing `s` then `b`.

### spatial indexing

The spatial indexing implementation breaks the simulation world into axis-aligned cells, where each cell is size of the largest radius of influence. The cell then stores the number and list of boids within its bounds. This means that any boid need only look at the nine cells centered around its current cell to find all boids that could influence its movement.

OpenGL's compute shaders have no support for dynamic memory allocation, so we allocate a slab of integers - the `index` - with the same length as the set of boids. In a slab of memory called `ranges`, we separately track the offsets and sizes of each cell into the `index`, such that the `index` itself stores the concatenation of all the cells' lists. For example, if cell 0 stores the boids `[13, 0, 4, 3]`, and cell 1 stores the boids `[9, 12, 1]`, then the `ranges` slab would look like `[(0, 4), (4, 3), ...]`, and the `index` would look like `[13, 0, 4, 3, 9, 12, 1]`.

This algorithm has a best-case time complexity of O(N), and a worst-case of O(N<sup>2</sup>).

Switch to this implementation by pressing `s` then `i`.

### preaggregated spatial indexing

This implementation extends the spatial indexing implementation. Instead of each cell storing only the boids in that cell, each cell stores the boids its immediate nine cell area, such that each boid need only look at its current cell to find all influence candidates.

Switch to this implementation by pressing `s` twice.

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
