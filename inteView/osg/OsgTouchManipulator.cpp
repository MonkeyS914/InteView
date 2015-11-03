#include "OsgTouchManipulator.h"
#include <osg/io_utils>
#include <osg/Math>


using namespace osg;
using namespace osgGA;

#define  LOG_TAG    "TouchManipulator"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)

#define  PI    3.1416

OsgTouchManipulator::OsgTouchManipulator(int flags )
   : inherited( flags )
{
	  setVerticalAxisFixed( false );
	  setAllowThrow(true);

	  float xrotateangle = acos(cos(30.0/180*PI)*cos(35.27/180*PI));
}

OsgTouchManipulator::OsgTouchManipulator( const OsgTouchManipulator& tm, const CopyOp& copyOp )
    : osg::Object(tm, copyOp), inherited( tm, copyOp )
{
}

void OsgTouchManipulator::handleMultiTouchDrag(GUIEventAdapter::TouchData* now, GUIEventAdapter::TouchData* last, const double eventTimeDelta)
{
    const float zoom_threshold = 1.0f;

    osg::Vec2 pt_1_now(now->get(0).x,now->get(0).y);
    osg::Vec2 pt_2_now(now->get(1).x,now->get(1).y);
    osg::Vec2 pt_1_last(last->get(0).x,last->get(0).y);
    osg::Vec2 pt_2_last(last->get(1).x,last->get(1).y);



    float gap_now((pt_1_now - pt_2_now).length());
    float gap_last((pt_1_last - pt_2_last).length());

    // osg::notify(osg::ALWAYS) << gap_now << " " << gap_last << std::endl;

    if (fabs(gap_last - gap_now) >= zoom_threshold)
    {
        // zoom gesture
        zoomModel( (gap_last - gap_now) * eventTimeDelta, false );
    }

    // drag gesture

    osg::Vec2 delta = ((pt_1_last - pt_1_now) + (pt_2_last - pt_2_now)) / 2.0f;

    float scale = 0.2f * _distance * eventTimeDelta;

    // osg::notify(osg::ALWAYS) << "drag: " << delta << " scale: " << scale << std::endl;

    //_distance += scale;

    panModel( delta.x() * scale, delta.y() * scale * (-1)); // flip y-coord because of different origins.
    //Matrix matrix;
    Vec3d dv(delta.x(), delta.y() ,0);
    //rotation_matrix[12] += delta.x() * scale;
    //rotation_matrix[13] -= delta.y() * scale;
    //_center += dv * rotation_matrix;

    //_eye += dv;

    //dv += rotation_matrix.getTrans();
    //rotation_matrix.setTrans(dv);
    //_rotation = rotation_matrix.getRotate();
//    matrix = getMatrix();
//    dv += matrix.getTrans();
//    matrix.setTrans(dv);
//    setByMatrix(matrix);
//
//    osg::Vec3d eye,center,up;
//    getHomePosition(eye,center,up);
//    setHomePosition(eye,_center,up);

    //rotate gesture
   	const float epsilon=1.0e-6;
   	const float nyPI=acosf(-1.0);
   	float angle;

	osg::Vec2 v1=pt_1_now - pt_2_now;
	osg::Vec2 v2=pt_1_last - pt_2_last;
	v1.normalize();
	v2.normalize();

    float dot=v1*v2;
    //LOGW("dot: %f",dot);
   	if (fabs(dot-1.0)<=epsilon)
   		angle=0.0f;
   	else if (fabs(dot+1.0)<epsilon)
   		angle=nyPI;
   	else
   	{
   		float cross;

   		angle=acos(dot);
   		cross=v1[0]*v2[1]-v2[0]*v1[1];
   		if (cross<0)
   		{
   			//LOGW("cross<0");
    		angle=2*nyPI-angle;
   	    }
    }

   	if (!isNaN(angle))
   	{
   		rotateModel(angle);
   	}
}

//“‘œ‡ª˙∫ÕŒÔÃÂµƒ¡¨œﬂŒ™÷·–˝◊™
void OsgTouchManipulator::rotateModel(float angle)
{
	//osg::Vec3d axis=Vec3d(0,1,0);
	osg::Matrixd rotation_matrix(_rotation);

	//∏˙openglµƒ◊¯±Í÷·“ª—˘...
	//osg::Vec3d axis = Vec3d(0.0f,1.0f,0.0f)*rotation_matrix;
	//osg::Vec3d axis = Vec3d(1.0f,0.0f,0.0f)*rotation_matrix;
	osg::Vec3d axis = Vec3d(0.0f,0.0f,-1.0f)*rotation_matrix;

	Quat new_rotate;
	new_rotate.makeRotate( angle, axis );

   _rotation = _rotation * new_rotate;
}
//void OsgTouchManipulator::home( const GUIEventAdapter& ea, GUIActionAdapter& us )
//{
//    if( getAutoComputeHomePosition() )
//    {
//        const Camera *camera = us.asView() ? us.asView()->getCamera() : NULL;
//        computeHomePosition( camera, ( _flags & COMPUTE_HOME_USING_BBOX ) != 0 );
//    }
//
//    _thrown = false;
//    setTransformation( _homeEye, _homeCenter, _homeUp );
//
//    us.requestRedraw();
//    us.requestContinuousUpdate( false );
//    flushMouseEventStack();
//}
void OsgTouchManipulator::ChangeNavigationView(int viewtype)
{
	osg::Vec3d axis1 = Vec3d(1.0f,0.0f,0.0f);
	osg::Vec3d axis2 = Vec3d(0.0f,1.0f,0.0f);
	osg::Vec3d axis3 = Vec3d(0.0f,0.0f,1.0f);
	Quat new_rotate;
	switch (viewtype)
	{
	case CenterView:
		return;
	case IsotremicView:
		//new_rotate.makeRotate(-PI/12,axis1,PI/12, axis2,PI*3/4,axis3);
		new_rotate.makeRotate((-35.27/180)*PI,axis1,0, axis2,PI-PI/6,axis3);
		break;
	case FrontView:
		return;
	case BackView:
		new_rotate.makeRotate(0,axis1,0, axis2,PI,axis3);
		break;
	case LeftView:
		new_rotate.makeRotate(-PI/2,axis1,-PI/2, axis2,0,axis3);
		break;
	case RightView:
		new_rotate.makeRotate(-PI/2,axis1,PI/2, axis2,0,axis3);
		break;
	case TopView:
		new_rotate.makeRotate(-PI/2,axis1,0, axis2,0,axis3);
		break;
	case BottomView:
		new_rotate.makeRotate(-PI/2,axis1,PI, axis2,0,axis3);
		break;
	default:
		return;
	}

	_rotation = _rotation * new_rotate;
}
bool OsgTouchManipulator::handle( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    bool handled(false);

    switch(ea.getEventType())
    {

        case osgGA::GUIEventAdapter::PUSH:
        case osgGA::GUIEventAdapter::DRAG:
        case osgGA::GUIEventAdapter::RELEASE:
            if (ea.isMultiTouchEvent())
            {
                double eventTimeDelta = 1/60.0; //_ga_t0->getTime() - _ga_t1->getTime();
                if( eventTimeDelta < 0. )
                {
                    OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
                    eventTimeDelta = 0.;
                }
                osgGA::GUIEventAdapter::TouchData* data = ea.getTouchData();

                // three touches or two taps for home position
                if ((data->getNumTouchPoints() == 3) || ((data->getNumTouchPoints() == 1) && (data->get(0).tapCount >= 2)))
                {
                    flushMouseEventStack();
                    _thrown = false;
                    //home(ea,us);
                    //ChangeNavigationView(IsotremicView);
                    handled = true;

            
                }

                else if (data->getNumTouchPoints() == 2)
                {
                    if ((_lastTouchData.valid()) && (_lastTouchData->getNumTouchPoints() >= 2))
                    {
                        handleMultiTouchDrag(data, _lastTouchData.get(), eventTimeDelta);
                    }

                    handled = true;

              
                }

                _lastTouchData = data;

                // check if all touches ended
                unsigned int num_touches_ended(0);
                for(osgGA::GUIEventAdapter::TouchData::iterator i = data->begin(); i != data->end(); ++i)
                {
                    if ((*i).phase == osgGA::GUIEventAdapter::TOUCH_ENDED)
                        num_touches_ended++;
                }

                if(num_touches_ended == data->getNumTouchPoints())
                {
                    _lastTouchData = NULL;
                }

            }
            break;
        case osgGA::GUIEventAdapter::DOUBLECLICK:
        
        default:
            break;
    }

    return handled ? handled : TrackballManipulator::handle(ea, us);
}



//¥¶¿Ìµ•µ„¥•øÿ£¨≤‚ ‘£¨«“”–¥ÌŒÛ
//¡Ω∏ˆµ„ªπ–Ë“™±Í◊ºªØ
void OsgTouchManipulator::handleSingleTouch(float nowx,float nowy,float oldx,float oldy)
{
	double eventTimeDelta = 1/60.0;//‘› ±…Ë≥…’‚—˘

	// get deltaX and deltaY
	float dx =nowx - oldx;
	float dy =nowy- oldy;

	// return if there is no movement.
	if( dx == 0. && dy == 0. )
		return ;

	performMovementLeftMouseButton(eventTimeDelta,nowx,nowy);
}

//¥¶¿Ì∂‡µ„¥•øÿ£¨≤‚ ‘
void OsgTouchManipulator::handleMultiTouchDrag(float nowx0,float nowy0,float nowx1,float nowy1,float oldx0,float oldy0, float oldx1,float oldy1)
{
	//LOGW("nowx0: %f nowy0: %f nowx1: %f nowy1: %f oldx0: %f oldy0: %f oldx1: %f oldy1: %f",nowx0,nowy0,nowx1,nowy1,oldx0,oldy0,oldx1,oldy1);

	double eventTimeDelta = 1/60.0;

	const float zoom_threshold = 1.0f;

	osg::Vec2 pt_1_now(nowx0,nowy0);
	osg::Vec2 pt_2_now(nowx1,nowy1);
	osg::Vec2 pt_1_last(oldx0,oldy0);
	osg::Vec2 pt_2_last(oldx1,oldy1);

	float gap_now((pt_1_now - pt_2_now).length());
	float gap_last((pt_1_last - pt_2_last).length());

	if (fabs(gap_last - gap_now) >= zoom_threshold)
	{
		// zoom gesture
		zoomModel( (gap_last - gap_now) * eventTimeDelta, true );
	}

	// drag gesture

	osg::Vec2 delta = ((pt_1_last - pt_1_now) + (pt_2_last - pt_2_now)) / 2.0f;

	float scale = 0.2f * _distance * eventTimeDelta;

	// osg::notify(osg::ALWAYS) << "drag: " << delta << " scale: " << scale << std::endl;

	panModel( delta.x() * scale, delta.y() * scale * (-1)); // flip y-coord because of different origins.

}
