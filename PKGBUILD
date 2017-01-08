# Maintainer: Eli Skeggs <me@eliskeggs.com>
pkgname=boids
pkgver=v1.1
pkgrel=1
pkgdesc="A gpu-accelerated implementation of a flocking algorithm."
arch=('i686' 'x86_64')
url="https://github.com/skeggse/boids"
license=('MIT')
depends=('freeglut' 'glew')
makedepends=('gyp-git' 'python')

build() {
	gyp ../boids.gyp -fmake --depth=.
	BUILDTYPE=Release make
}

package() {
	mkdir -p "$pkgdir/usr/bin"
	mkdir -p "$pkgdir/usr/share/licenses/boids"
	cp out/Release/boids "$pkgdir/usr/bin"
	cp ../LICENSE "$pkgdir/usr/share/licenses/boids"
}
