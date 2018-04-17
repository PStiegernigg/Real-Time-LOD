Last update: 2017-04-17 by Philipp Stiegernigg

main.cpp
	Application entry point.

	Scene loading is performed here, comment/uncomment
	to switch scenes.
	
	NOTE: Using a release build can dramatically decrease
	scene loading times.

rendering/RenderContext
	RenderContext::render() is where all rendering
	calls are dispatched from.

geometry/LODGenerator
    Mesh simplification algorithms for all level-Of-detail approaches.

bin/scenes
    Various test models.

bin/shaders
    All rendering and compute shader of the algorithm.

## BUILD REQUIREMENTS ##
OpenGL >= 4.3
ImageMagick (libMagick++)


## BUILD INSTRUCTIONS ##

Execute ./build_dependencies.sh to build all included libaries within ./vendor
The application itself is a standart cmake project.

mkdir build
cd build
cmake ..
make

Executable can be found in ./bin and must be executed within this directory,
otherwise shader files and models will not be found by the application. 

## CONTROLS ##

LEFT MOUSE
	Change camera orientation

W/A/S/D
	Move camera forward / left / backward / right
	Use W/S during directional light camera view to adjust clipping!
	
Q/E
	Move camera up / down
	
SHIFT
	Increased camera movement speed
	
TAB
	Toggle menu
	
ESC
	Exit
