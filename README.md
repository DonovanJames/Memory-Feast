Memory Feast
============

This project was commissioned for Ingenuity Festival Cleveland 2012. 
It is a Kinect based interactive animation that uses the following dependant libraries:
ofxBox2d
ofxOpenNI
ofxOpenCV
MSAPhysics

The project has only been tested for Mac OSX 10.6. I plan on making minor updates for OSX 10.8 in the near future. The main purpose of this repo is to move towards a more open coding practice. This is the first project I've written completely independently, so there are many bugs, hacks, and areas of poor style. I've tried to integrate as much of my learning in C++ and openFrameworks as possible. I have to give huge credit to Memo Atkins for this MSA library and examples. 

Some of the things going on in this program:

1. OpenCV polyLine transform
2. OpenNI skeleton tracking
3. OpenGL 3D geometry texture mapping
4. MSAPhysics fluid simulation
5. Box2D animated 2D geometry


OpenCV polyLine transform
=========================

When a user enters the scene, OpenNI provides a simple RGBA pixel mask. Although this is a useful element for masking video, the pixel mask is difficult to stylize in a meaningful way. So, I've isolated a single channel, and put that through an OpenCV polyLine transform that creates a polyLine object. This can then be animated and stylized using standard openFrameworks style calls. I only draw some of the point in the lines, then smooth the line. When a user enters the scene they see a minimal silhouette in the style of other elements in the program. 

++ A potential feature is adding the polyLine to the Box2D world to add additional interactive elements. 


Box2D animated 2d geometry
==========================

For the animated monster, I generated a custom class to draw the heads. each head consists of a long chain of Box2D rectangles that are jointed together using revolute joints. The joints have different parameters depending on their position along the chain. By limiting joint angles and adding motor forces the chains are animated to mimic the movements of snakes. All of the neck joints along the chain are given a no collision parameter that prevents them from colliding with each other and creating erratic behavior. The heads do have collision properties which help contain the bodies and provide more realistic behavior. Once the geometry and behavior is set to desired effect, I iterate over the various elements and generate cohesive polyLine objects that are styled and drawn with OpenFrameworks styles. 
