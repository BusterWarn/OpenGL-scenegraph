Buster Hultgre Wärn,

Umeå University
2020-03-17

1. DESCRIPTION
2. BUILDING
  2.1 LINUX
  2.2 WINDOWS
3. COMPILATION AND EXECUTING
4. CONTROLS
5. CREATING A SCENEGRAPH

1. DESCRIPTION
-------------

A scenegraph in OpenGL. First laboration in course "Advanced computer graphics and application" given by Umeå University.

A scenegraph is a way of rendering multiple 3d objects in a tree hiearchy. Here leaves are theactual geometries to be drawn and their parents are groups or transforms. Each group representa group of an object, e.g. a room or a car, where a transform is some matrix transformation thatwill apply to all it’s children.This report will discuss my implementation of a scenegraph; how it functions, its features and howit is used.
  
2. BUILDING
-----------

2.0 dependencies
You will find a link to download the file "dependencies.zip" on the same page you found this zip file.
Unpack dependencies.zip into the same directory.
Notice that you need first need to execute setup_env.bat/bash depending on your platform before continuing
Then depending on your platform, start a command shell and run build_all.bat or build_all.bash.
On Unix, png, glew, glut, jpeg and zlib will be required pre-installed dependencies.


2.1 LINUX
---------

Note: The scripts (build_all.bash/setup_env.bash) assumes you can or are using the bash shell.
So if you are using some other shell, just look into setup_env.bash and mimic whatever you have to expose the same environment variables as that script.

Linux usually have a lot of stuff available as packages (whereas windows have none)
You need to make sure you have glut, glew, png, zlib, freetype installed in your default paths.

Run 

You need to build some of the dependencies (in the dependencies directory)
See build_all.bash

You might need to change permission on the script:

> chmod 755 build_all.bash

Then you can run it:

> ./build_all.bash

Everything should end up in the installed directory (after a make install)

When you have run build_all.bat once, you do not need to run it again.
It is only for building all the dependencies once.

See chapter 5 below.

Look into bin directory for the executables.

Before running (especially objViewer) you might need to set an environment variable VR_PATH.
This should point to the vrlib directory (where the sub-directories models, shaders exist).
If you are running bash you can do:

> source setup_env.bash 

Now if you want to build only the examples again, just write:

> make 
> make install

In the top directory

2.2 WINDOWS
-----------

The build_all.bat should build everything you need.
It defaults to use VS2017. If you want to use some other build too, just write:

> build_all /VS2015

If it doesn't, investigate and fix :-) (also report to me so I can fix it.)

To start visual studio with the project (vrlib) then just go to build\vrlib\ where you find vrlib.sln
Write devenv /useenv vrlib.sln to start visual studio with this project.

When you have run build_all.bat once, you do not need to run it again.
It is only for building all the dependencies once.

If you now want to build only the examples just do:

> setup_env.bat /VS2017 (or whatever VS version you are using)
> devenv /useenv vrlib.sln

To add your own example, See chapter 5 below.


3. COMPILATION AND EXECUTING
------------
The program is compiled with cmake by from the main folder running:
> cmake .

To run the program
> ./scenegraph/scenegraph [filename]

where `filename` is a scene (.xml) or object file (.obj). If left empty the scenegraph will run `scenes/party.xml` as its default.

4. CONTROLS
-------------

The program will launch with camera mode which will control the camera. With WASD + QE and holding down the mouse button the camera can be rotated around its XY axises and translated in XYZ directions. Pressing F1 will initiate object mode (F2 for camera mode). Here the mouse can be moved as in camera mode, but this will spin the scene graph instead.

The home key will reset the scene graph and the escape key will close the program.


5. CREATING A SCENEGRAPH
-------------
In the directory scenes there are examples of an xml format which can be read using the scenegraph (files with extension .xml)

Below is an example of this format. It should be easy to understand.
The parsing of this format is done in loader.cpp. Notice that it is not very error safe.

<?xml version="1.0" encoding="utf-8"?>
<scene>
  <node name="[name]">
    <file path="[filepath]"> </file>
    <transform translate="x y z" rotate="x y z" scale="x y z" spin="x y z"></transform>
    <draw cull_face_mode="[option]" polygon_mode="[option]"></draw>
    <no_light id="light_1 light_2 ... light_n"></no_light>
  </node>

  <light name="[name]" position="x y z" diffuse="x y z" specular="x y z" spin="x y z"></light>

</scene>

Each `node` represents a geometry to be rendered. Each of its child tags represent the folloing of the node

- `file` The filename where the geometry object is stored.
- `transform` If the object should be transformed somehow. Here a spin can also be set around the geometry.
- `draw` OpenGL drawing options for the geometry. This includes two optional settings: `cull_face_mode="[front|back|front_and_back]"` and `polygon_mode="[wireframe|points|filled]"`
- `no_light` If the geometry should ignore some light. The id's to be ignored is only the position that the `light` tag are ordered in the xml scene file.

The `light` tag is straight forward and will construct a light with specular, diffuse and ambient lightning. The light may also spin around its translated position axis.