/*
 * OsgMainApp.hpp
 *
 *  Created on: 29/05/2011
 *      Author: Jorge Izquierdo Ciges
 */

#ifndef OSGMAINAPP_HPP_
#define OSGMAINAPP_HPP_

//Android log
#include <iostream>
#include <cstdlib>
#include <math.h>

//Standard libraries
#include <string>

//osg
#include <osg/GL>
#include <osg/GLExtensions>
#include <osg/Depth>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Node>
#include <osg/Notify>
//osgText
#include <osgText/Text>
//osgDB
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
//osg_viewer
#include <osgViewer/Viewer>
#include <osgViewer/Renderer>
#include <osgViewer/ViewerEventHandlers>
//osgGA
#include <osgGA/GUIEventAdapter>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>
//Self headers
#include "OsgAndroidNotifyHandler.hpp"
#include "OsgConverter.h"
#include "OsgTouchManipulator.h"

//windowing system
USE_GRAPICSWINDOW_IMPLEMENTATION(IOS)

//plugins

//USE_OSGPLUGIN(obj)

//USE_OSGPLUGIN(ive)
//USE_OSGPLUGIN(osg)
//USE_OSGPLUGIN(osg2)
//USE_OSGPLUGIN(terrain)
//USE_OSGPLUGIN(rgb)
//USE_OSGPLUGIN(OpenFlight)
//USE_OSGPLUGIN(dds)
////Static DOTOSG
//USE_DOTOSGWRAPPER_LIBRARY(osg)
//USE_DOTOSGWRAPPER_LIBRARY(osgFX)
//USE_DOTOSGWRAPPER_LIBRARY(osgParticle)
//USE_DOTOSGWRAPPER_LIBRARY(osgTerrain)
//USE_DOTOSGWRAPPER_LIBRARY(osgText)
//USE_DOTOSGWRAPPER_LIBRARY(osgViewer)
//USE_DOTOSGWRAPPER_LIBRARY(osgVolume)
////Static serializer
//USE_SERIALIZER_WRAPPER_LIBRARY(osg)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgAnimation)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgFX)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgManipulator)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgParticle)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgTerrain)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgText)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgVolume)

struct Model{
    std::string filename;
    std::string name;
    char* cfilename;
};


class OsgMainApp{
private:
    osg::ref_ptr<osg::Referenced> _window;
    osg::ref_ptr<osgViewer::Viewer> _viewer;
    osg::ref_ptr<osg::Group> _root;
    osg::ref_ptr<osg::Geode> _textGeode;
    osg::ref_ptr<osg::StateSet> _state;
    osg::ref_ptr<osgGA::OsgTouchManipulator> _manipulator;
//    osg::ref_ptr<osg::GraphicsContext> graphicsContext;

    float _lodScale;
    unsigned int _prevFrame;

    bool _initialized;
    bool _clean_scene;

    OsgAndroidNotifyHandler *_notifyHandler;
    
    std::vector<Model> _vModels;
    std::vector<Model> _vModelsToLoad;
    std::vector<Model> _vModelsToDelete;

    void loadModels();
    void loadTags();
    void deleteModels();

public:
    std::vector<child> vModelsToLoad;
    std::vector<std::string> vModelsToLoad1;
    std::vector<std::string> temvModelsToLoad1;
    //解密iva文件时候用
//    std::string ciptxtph;
//    std::string tmpTxtPh;
//    std::string ivafilename;
    std::vector<std::string> ivavector;
    
public:
    OsgMainApp();
    ~OsgMainApp();

    //Initialization function
    void initSimOsgWindow();
    void initOsgWindow(int x,int y,int width,int height, osg::ref_ptr<osg::Referenced>& windata);
    void getciphrtxFilePath(std::string ciphrtextph, std::string tmptxph, std::string  filename);
    
    //Draw
    void draw( int step,int isFromPic );
    //Events
    void mouseButtonPressEvent(float x,float y,int button);
    void mouseButtonReleaseEvent(float x,float y,int button);
    void mouseMoveEvent(float x,float y);
    void keyboardDown(int key);
    void keyboardUp(int key);
    //Loading and unloading
    void loadObject(std::string filePath);
    void loadObject(std::string name,std::string filePath);
    void unLoadObject(int number);
    void clearScene();
    //Other functions
    int  getNumberObjects();
    std::string getObjectName(int nunmber);

    void setClearColor(osg::Vec4f color);
    osg::Vec4f getClearColor();

    osg::ref_ptr<osg::Node> createShape();
    void loadIvpModel();
    void loadModelFile( int step ,int isFromPic);
    void NavigationButtonDown(int viewtype);

    //¥•øÿ
    void touchBeganEvent(int id,int phase,float x,float y);
    void touchMovedEvent(int id,int phase,float x,float y);
    void touchEndedEvent(int id,int phase,float x,float y);
    void doubleTouchEvent(int phase,int id0,float x0,float y0,int id1,float x1,float y1);
    
    void releaseView();
};


#endif /* OSGMAINAPP_HPP_ */
