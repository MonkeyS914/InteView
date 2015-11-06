#include "OsgMainApp.hpp"
#include <osgViewer/api/IOS/GraphicsWindowIOS>
#include "osgDB/ReadFile"
#include "osgViewer/Viewer"
#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include "osg/ShapeDrawable"
#include "osg/Shape"

#include<osgDB/FileUtils>
#include <osg/Material>
#include <osg/LightModel>
#include <osg/MatrixTransform>
#include <osg/CameraNode>
#include "Osgpick.h"

#include <osgText/Text>

#include <OpenGLES/ES1/gl.h>


#define  LOG_TAG    "OsgMainApp"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)


OsgMainApp::OsgMainApp(){

    _lodScale = 1.0f;
    _prevFrame = 0;

    _initialized = false;
    _clean_scene = false;


}
OsgMainApp::~OsgMainApp(){

}

void OsgMainApp::deleteModels(){
    if(_vModelsToDelete.size()==0) return;

    osg::notify(osg::ALWAYS)<<"There are "<<_vModelsToDelete.size()<<" models to delete"<<std::endl;

    Model modelToDelete;
    for(unsigned int i=0; i<_vModelsToDelete.size(); i++){
        modelToDelete = _vModelsToDelete[i];
        osg::notify(osg::ALWAYS)<<"Deleting: "<<modelToDelete.name<<std::endl;

        for(unsigned int j=_root->getNumChildren(); j>0; j--){
            osg::ref_ptr<osg::Node> children = _root->getChild(j-1);
            if(children->getName() == modelToDelete.name){
                _root->removeChild(children);
            }
        }

    }

    _vModelsToDelete.clear();
    osg::notify(osg::ALWAYS)<<"finished"<<std::endl;
}

void OsgMainApp::getciphrtxFilePath(std::string ciphrtextph, std::string tmptxph, std::string filename){
    // memcpy
    ivavector.push_back(ciphrtextph.c_str());
    ivavector.push_back(tmptxph.c_str());
    ivavector.push_back(filename.c_str());
//    memcpy(&ciptxtph, &ciphrtextph, ciphrtextph.length());
//    memcpy(&tmpTxtPh, &tmptxph, tmptxph.length());
//    memcpy(&ivafilename, &filename, filename.length());
    
}

void OsgMainApp::initSimOsgWindow(){

    _viewer = new osgViewer::Viewer();
    _viewer->setCameraManipulator(new osgGA::MultiTouchTrackballManipulator);
    _viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);//SingleThreaded DrawThreadPerContext
    _viewer->realize();

    osg::setNotifyLevel(osg::DEBUG_FP);
    _initialized = true;
}

//Initialization function
void OsgMainApp::initOsgWindow(int x,int y,int width,int height, osg::ref_ptr<osg::Referenced>& windata){
    
    _window = windata;
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 0;
    traits->y = 0;
    traits->width = width*2;
    traits->height = height*2;
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->setInheritedWindowPixelFormat = true;
    traits->inheritedWindowData = _window;
    osg::ref_ptr<osg::GraphicsContext> graphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());
    
    if (_viewer == NULL) {
        _viewer = new osgViewer::Viewer();
    }
    _viewer->addEventHandler(new osgViewer::StatsHandler);
    _viewer->setCameraManipulator(new osgGA::MultiTouchTrackballManipulator);
    _viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);//SingleThreaded DrawThreadPerContext
    _manipulator = new osgGA::OsgTouchManipulator;
    _viewer->setCameraManipulator( _manipulator.get());//save angle of camera
    _viewer->getViewerStats()->collectStats("scene", true);
    _viewer->realize();
    _viewer->getCamera()->setGraphicsContext(graphicsContext);
    _viewer->getCamera()->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));
   
    osg::setNotifyLevel(osg::DEBUG_FP);
    _initialized = true;

}

void OsgMainApp::loadModelFile( int step )
{
    
    if(vModelsToLoad1.size()==0) return;
    
    _root = OsgConverter::readFile(vModelsToLoad1[0],step, ivavector);
  
    Model newModel;
    newModel.filename = vModelsToLoad1[0];
    if(_vModels.size() == 0)
    {
        _vModels.push_back(newModel);
    }
    else
    {
    	_vModels[0] = newModel;
    }
    
    _viewer->setSceneData(NULL);
    _viewer->setSceneData(_root.get());
    _manipulator->getNode();
    _viewer->home();
    _manipulator->ChangeNavigationView(0);
    _viewer->getDatabasePager()->clear();
    _viewer->getDatabasePager()->registerPagedLODs(_root.get());
    _viewer->getDatabasePager()->setUpThreads(3, 1);
    _viewer->getDatabasePager()->setTargetMaximumNumberOfPageLOD(2);
    _viewer->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(true, true);

    vModelsToLoad1.clear();
}

//Draw
void OsgMainApp::draw( int step ){
    
    loadModelFile( step );
    deleteModels();
    if (_viewer != NULL) {
        _viewer->frame();
    }
}
//Events
void OsgMainApp::mouseButtonPressEvent(float x,float y,int button){
    _viewer->getEventQueue()->mouseButtonPress(x, y, button);
}
void OsgMainApp::mouseButtonReleaseEvent(float x,float y,int button){
    _viewer->getEventQueue()->mouseButtonRelease(x, y, button);
}
void OsgMainApp::mouseMoveEvent(float x,float y){
    _viewer->getEventQueue()->mouseMotion(x, y);
}
void OsgMainApp::keyboardDown(int key){
    _viewer->getEventQueue()->keyPress(key);
}
void OsgMainApp::keyboardUp(int key){
    _viewer->getEventQueue()->keyRelease(key);
}
//œÏ”¶ ”Õº«–ªª∞¥≈•
void OsgMainApp::NavigationButtonDown(int viewtype)
{
	_viewer->home();
 
    
	_manipulator->ChangeNavigationView(viewtype);
}
//Loading and unloading
void OsgMainApp::loadObject(std::string filePath){

	if(filePath.compare("") == 0) return;

//    for(unsigned int i=0;i<_vModels.size();i++){
//        if(_vModels[i].filename.compare(filePath) == 0)
//            return;
//    }

    vModelsToLoad1.push_back(filePath);
    temvModelsToLoad1.push_back(filePath);
}
void OsgMainApp::loadObject(std::string name,std::string filePath){

    Model newModel;
    newModel.filename = filePath;
    newModel.name = name;

    for(unsigned int i=0;i<_vModels.size();i++){
        if(_vModels[i].name==newModel.name){
            osg::notify(osg::ALWAYS)<<"Name already used"<<std::endl;
            return;
        }
    }

    _vModelsToLoad.push_back(newModel);
}
void OsgMainApp::unLoadObject(int number){
    if(_vModels.size() <= number){
        osg::notify(osg::FATAL)<<"Index number error"<<std::endl;
        return;
    }

    Model modelToDelete = _vModels[number];
    _vModels.erase(_vModels.begin()+number);
    _vModelsToDelete.push_back(modelToDelete);
}
void OsgMainApp::clearScene(){
    _vModelsToDelete = _vModels;
    _vModels.clear();

}
//Other Functions
int OsgMainApp::getNumberObjects(){
    return (int)_vModels.size();
}
std::string OsgMainApp::getObjectName(int number){
    return _vModels[number].name;
}
void OsgMainApp::setClearColor(osg::Vec4f color){
    osg::notify(osg::ALWAYS)<<"Setting Clear Color"<<std::endl;
    _viewer->getCamera()->setClearColor(color);
}
osg::Vec4f OsgMainApp::getClearColor(){
    osg::notify(osg::ALWAYS)<<"Getting Clear Color"<<std::endl;
    return _viewer->getCamera()->getClearColor();
}

void OsgMainApp::touchBeganEvent(int id,int phase,float x,float y){
	  _viewer->getEventQueue()->touchBegan(id,osgGA::GUIEventAdapter::TOUCH_BEGAN,x, y);
	 
}

void OsgMainApp::touchMovedEvent(int id,int phase,float x,float y){
	  _viewer->getEventQueue()->touchMoved(id,osgGA::GUIEventAdapter::TOUCH_MOVED,x, y);
}

void OsgMainApp::touchEndedEvent(int id,int phase,float x,float y){
	  _viewer->getEventQueue()->touchEnded(id,osgGA::GUIEventAdapter::TOUCH_ENDED,x, y,1);
}

void OsgMainApp::doubleTouchEvent(int phase,int id0,float x0,float y0,int id1,float x1,float y1){
	osg::ref_ptr<osgGA::GUIEventAdapter> osg_event;
	switch(phase){
		case 1:
			osg_event = _viewer->getEventQueue()->touchBegan(id0,osgGA::GUIEventAdapter::TOUCH_BEGAN, x0, y0);
			osg_event->addTouchPoint(id1, osgGA::GUIEventAdapter::TOUCH_BEGAN, x1, y1);
			break;
		case 2:
			osg_event = _viewer->getEventQueue()->touchMoved(id0,osgGA::GUIEventAdapter::TOUCH_MOVED, x0, y0);
			osg_event->addTouchPoint(id1, osgGA::GUIEventAdapter::TOUCH_MOVED, x1, y1);
			break;
		case 4:
			osg_event = _viewer->getEventQueue()->touchEnded(id0,osgGA::GUIEventAdapter::TOUCH_ENDED, x0, y0,1);
			osg_event->addTouchPoint(id1, osgGA::GUIEventAdapter::TOUCH_ENDED, x1, y1);
			break;
		default:
			break;
		}
}

void OsgMainApp::releaseView() {
    if (_viewer != NULL) {
        _viewer -> done();
        _viewer = NULL;
        _root = NULL;
        _window = NULL;
    }
}
