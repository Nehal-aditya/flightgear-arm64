# FGQCanvas

A Qt-based remote canvas application for FlightGear. This app can connect to
a FlightGear instance which has the built-in HTTPD server enabled and display
any canvas in real-time.

## Building

On a Debian based system you have to install these packages:

```text
git cmake ninja-build gcc \
qtbase5-dev qtbase5-private-dev \
libqt5websockets5-dev \
qtdeclarative5-dev qtdeclarative5-private-dev \
qtquickcontrols2-5-dev qtquickcontrols2-5-private-dev \
qml-module-qtquick2
```

* Clone the flightgear repo with
  `git clone https://gitlab.com/flightgear/flightgear.git`
* Change into the `utils/fgqcanvas/` directory
* Run `cmake . -G Ninja && cmake --build .`
* If you want to install FGQcanvas system wide, run
  `cmake --install . --prefix=/your/install/path`

## Usage

Start FlightGear with the '--httpd' option, passing a port number. This can be
done in the 'additional options' box if using the launcher.

* `--httpd=8080`

Start FGQCanvas and enter the WebSocket url, with a suitable host-name and port.
Provide the path to the Canvas you want to display (this part will become
smarter in the future!)

Examples URLs:

* `ws://localhost:8080/PropertyTreeMirror`
* `ws://mycomputer.local:8001/PropertyTreeMirror`

Example Canvas path:

* `/canvas/by-index/texture[0]/`

## Limitations

* Clipping is still being worked on
* Image loading is still being worked on, no support for remote image loading
  yet.
* Performance is mediocre due to proof-of-concept implementation
* No input event support yet

## Future plans

* Finish image, clip and font loading
* Switch to OpenGL rendering
* Support event-input to the Canvas
* Rewrite to use [Skia](http://skia.org)

## Questions / support

Ask on the developer mailing list!
