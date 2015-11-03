
#include "Osgpick.h"
#define  LOG_TAG    "Osgpick"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)



bool PickHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::PUSH):
        {
        	
            osgViewer::Viewer* view = dynamic_cast<osgViewer::Viewer*>(&aa);
            if (view) pick(view,ea);
            return false;
        }    
        case(osgGA::GUIEventAdapter::KEYDOWN):
        {
            if (ea.getKey()=='c')
            {        
                osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
                osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
                event->setX((ea.getXmin()+ea.getXmax())*0.5);
                event->setY((ea.getYmin()+ea.getYmax())*0.5);
                if (view) pick(view,*event);
            }
            return false;
        }    
        default:
            return false;
    }
}

void PickHandler::pick(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& ea)
{
    float x = ea.getX()*500;
    float y = ea.getY()*500;
	osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
		new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);

	osgUtil::IntersectionVisitor iv( intersector.get() );
	osg::Node::NodeMask traversalMask = 0xffffffff;
	iv.setTraversalMask(traversalMask);
	viewer->getCamera()->accept( iv );

	if ( intersector->containsIntersections() )
	{

		const osgUtil::LineSegmentIntersector::Intersection& result =
			*(intersector->getIntersections().begin());
		int size = result.nodePath.size ();
	

		osg::MatrixTransform* selection = 0;

		for(int index=0; index < size; index++)
		{
			selection = dynamic_cast<osg::MatrixTransform*>(result.nodePath[index]);
			if (selection)
			{
				std::string str = selection->getName();
				
//				if ( std::find(_selectionNames.begin(), _selectionNames.end(), str) != _selectionNames.end() )
//				{
//					return selection;
//				}
			}
		}
	}

}

void PickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
{
	osg::Node* node = view->getSceneData();
	node->getName();


    osgUtil::LineSegmentIntersector::Intersections intersections;

    std::string gdlist="";
    float x = ea.getX();
    float y = ea.getY();



    if (view->computeIntersections(x,y,intersections))
    {
    
        for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
            hitr != intersections.end();
            ++hitr)
        {
            std::ostringstream os;
            if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
            {
                // the geodes are identified by name.
                os<<"Object \""<<hitr->nodePath.back()->getName()<<"\""<<std::endl;

               
            }
            else if (hitr->drawable.valid())
            {
                os<<"Object \""<<hitr->drawable->className()<<"\""<<std::endl;

               
            }

            
            gdlist += os.str();
        }
    }

}
