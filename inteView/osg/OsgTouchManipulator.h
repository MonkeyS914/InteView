#pragma once
#include <osgGA/TrackballManipulator>

namespace osgGA {

enum Encode
{
	CenterView = 0,
	IsotremicView,
	FrontView,
	BackView,
	LeftView,
	RightView,
	TopView,
	BottomView
};
//    TrackballManipulator
//MultiTouchTrackballManipulator
class  OsgTouchManipulator: public TrackballManipulator
{
	  typedef TrackballManipulator inherited;

public:
	OsgTouchManipulator( int flags = DEFAULT_SETTINGS);
	OsgTouchManipulator(const OsgTouchManipulator& tm, const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY);

	META_Object( osgGA, OsgTouchManipulator );

	bool handle( const GUIEventAdapter& ea, GUIActionAdapter& us );
	//void home( const GUIEventAdapter& ea, GUIActionAdapter& us );

	void ChangeNavigationView(int viewtype);
protected:

    void handleMultiTouchDrag(GUIEventAdapter::TouchData* now, GUIEventAdapter::TouchData* last, const double eventTimeDelta);

    void rotateModel(float angle);

    osg::ref_ptr<GUIEventAdapter::TouchData> _lastTouchData;

   //最早的版本，不经过事件列表，直接操作相机
	void handleSingleTouch(float nowx,float nowy,float oldx,float oldy);
	void handleMultiTouchDrag(float nowx0,float nowy0,float nowx1,float nowy1,float oldx0,float oldy0, float oldx1,float oldy1);
};

}
