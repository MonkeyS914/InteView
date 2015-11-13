
#include "OsgConverter.h"

//#include "crypttest.h"
//#include "testaaa.h"


#include<osgDB/FileUtils>
#include <osg/Material>
#include <osg/LightModel>
#include "OSGOpcodeShell.h"
#include "OSGOpcodeHandler.h"
#include "osgDB/ReadFile"
#include "osgViewer/Viewer"
#include "osg/Node"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/ShapeDrawable"
#include "osg/Shape"
#include <osg/MatrixTransform>

#include <osg/CameraNode>
#include <osgText/Text>
#include <osgText/Text3D>
#include <OpenGLES/ES1/gl.h>
#import <Foundation/Foundation.h>
#include "ShowOsgViewController.h"

#include <string.h>
#include <map>
#include <vector>

std::vector<std::string> strtmp;
std::vector<int> inttmp;

static osg::ref_ptr<osg::Group> m_root = NULL;//≥°æ∞∏˘Ω⁄µ„
static osg::ref_ptr<osg::Geode> m_geode = NULL;

static osg::ref_ptr<osg::Geometry> m_geometry = NULL;
static osg::ref_ptr<osg::Vec3Array> m_array3 = NULL;
static osg::ref_ptr<osg::Vec3Array> m_aNormal = NULL;
static osg::ref_ptr<osg::UIntArray> m_array = NULL;
static long m_i = 0;

static std::string temString;

std::vector<std::string> documentvector; // 存放 解密文件目录 . 临时文件目录 文件名



static float m_rgb[3];
//static JNIEnv * m_env = NULL;
static int m_shellcount = 0;
static std::map<std::string, osg::ref_ptr<osg::Group> > m_Map;
//void OsgConverter::SetJNIEnv (JNIEnv * env)
//{
//	m_env = env;
//}
void OsgConverter::SetRGB (float const * rgb)
{
	m_rgb[0] = rgb[0]; m_rgb[1] = rgb[1]; m_rgb[2] = rgb[2];
}
float* OsgConverter::GetRGB()
{
	return m_rgb;
}
OsgConverter::OsgConverter(void)
{
	m_rgb[0] = 0.5;
	m_rgb[1] = 0.5;
	m_rgb[2] = 0.5;
	if (m_geode == NULL)
	{
		m_geode = new osg::Geode();
	}
	//m_geode = new osg::Geode;
	m_StateSet = m_geode->getOrCreateStateSet();
}

OsgConverter::~OsgConverter(void)
{
}

char* OsgConverter::unZlib(const char* filename)
{
	//FILE *fptemp=fopen("/mnt/sdcard/InteVue/temp.txt","w");
	//fclose(fptemp);
	//LOGI("33");
	FILE *fp=fopen(filename,"rb");

	fseek(fp, 0, SEEK_END);
	long file_size = ftell( fp );
	char* buff = (char*)malloc(sizeof(char) * file_size*50 + 1);
	strcpy(buff,"");
//	buff[0] = 0xef;
//	buff[1] = 0xbb;
//	buff[2] = 0xbf;
    
	unsigned long  uFilePos;
    printf("%lu",uFilePos);
    
	fseek(fp, 80, SEEK_SET);
	Bytef cInput[FILEBUFFERSIZE];
	Bytef* cOutput;
	cOutput = (Bytef*)malloc(sizeof(Bytef) * FILEBUFFERSIZE + 1);
	//≥ı ºªØcoutput
	while(!feof(fp))
	{
		uLongf uZipedLength=0,uOutput,uInput;
		int rLen = fread(&uZipedLength, sizeof(int32_t), 1, fp);
        
//		if(uZipedLength<=0)
//			break;
		uInput = fread(cInput,sizeof(Bytef),uZipedLength, fp);
		if(uInput!=uZipedLength) //¡Ω∏ˆ≤ªœ‡µ»£¨ø…ƒ‹Œƒº˛≥ˆœ÷¡À¥ÌŒÛ£°
		{
			break;
		}
		uOutput=FILEBUFFERSIZE;
		int iResult=0;
		//≥ı ºªØcoutput
		iResult=uncompress(cOutput,&uOutput,cInput,uZipedLength);
		if(uOutput<=0) //ø…ƒ‹≥ˆœ÷¡ÀŒ Ã‚
			break;

		//break;
		//LOGI("44");
		//int len = fwrite(cOutput,sizeof(Bytef),uOutput,fptemp);
		//int len = fwrite("12345",sizeof(char),6,fptemp);
		cOutput[uOutput] = '\0';
		strcat(buff,(const char *)cOutput);
		//LOGI("55:%d",len);
	}
	fclose(fp);
	return buff;
}
char* OsgConverter::ANSI2UTF8(char* buf)
{
    return buf;
}

osg::ref_ptr<osg::Geode> createQuad()
{
    //创建叶节点
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    //创建几何体对象
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    //创建定点数字，添加顺序是逆时针
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    
    v->push_back(osg::Vec3(0.0f,0.0f,0.0f));
    v->push_back(osg::Vec3(1000.0f,0.0f,0.0f));
    v->push_back(osg::Vec3(1000.0f,0.0f,1000.0f));
    v->push_back(osg::Vec3(0.0f,0.0f,1000.0f));
    
    //设置定点数据
    geom->setVertexArray(v.get());
    
    //创建纹理坐标
    osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
    //添加数据
    vt->push_back(osg::Vec2(0.0f,0.0f));
    vt->push_back(osg::Vec2(1.0f,0.0f));
    vt->push_back(osg::Vec2(1.0f,1.0f));
    vt->push_back(osg::Vec2(0.0f,1.0f));
    
    //设置纹理坐标
    geom->setTexCoordArray(0, vt.get());
    
    //创建颜色数组
    osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array();
    //添加数据
    vc->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
    vc->push_back(osg::Vec4(0.0f,1.0f,0.0f,1.0f));
    vc->push_back(osg::Vec4(0.0f,0.0f,1.0f,1.0f));
    vc->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
    
    //设置颜色数组
    geom->setColorArray(vc.get());
    //设置颜色绑定方式为单个定点
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    //创建法线数组
    osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array();
    //添加法线
    nc->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
    //设置法线数组
    geom->setNormalArray(nc.get());
    
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
    
    //添加元素绘制基元为四边形
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,3));
    
    geode->addDrawable(geom.get());
    
    return geode.get();
}

osg::ref_ptr<osg::Group> OsgConverter::readFile(std::string cfileName, int step ,int isFromPic, std::vector<std::string> filedocument)
{
    
    for(int i=0;i<filedocument.size();i++)
    {
        documentvector.push_back(filedocument[i].c_str());
    }

	m_Map.clear();

	osg::ref_ptr<osg::Group> osgRoot = new osg::Group();
	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
	float tempM[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	transform->setMatrix(osg::Matrix(tempM[0],tempM[1],tempM[2],tempM[3],
									tempM[4],tempM[5],tempM[6],tempM[7],
									tempM[8],tempM[9],tempM[10],tempM[11],
									tempM[12],tempM[13],tempM[14],tempM[15]));
	osgRoot->addChild(transform);
    
	if(cfileName.find(".iva") != -1)
	{
		readAsmFile(cfileName,transform);
	}
    else if(cfileName.find(".i3d") != -1 )
    {
        readI3dFile(cfileName, step,isFromPic, transform);
    }
	else
	{
		readIvpFile(cfileName,transform);
	}
    
	osgRoot->setName(cfileName);

	return osgRoot;
}

osg::ref_ptr<osg::Geode> createLine(){
    
    //创建叶节点
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    //创建几何体对象
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    //创建定点数字，添加顺序是逆时针
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    
    //x y z
    v->push_back(osg::Vec3(100.0f,0.0f,100.0f));
    v->push_back(osg::Vec3(0.0f,0.0f,1000.0f));
    
    //设置定点数据
    geom->setVertexArray(v.get());
    
    //创建纹理坐标
    osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
    
    //添加数据
    vt->push_back(osg::Vec2(0.0f,0.0f));
    vt->push_back(osg::Vec2(1.0f,0.0f));
    
    //添加元素绘制基元为四边形
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
    
    geode->addDrawable(geom.get());
    
    return geode.get();
}

osg::Camera* createHUDCamera( double left, double right, double bottom, double top )
{
    //创建相机
    
    //    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    
    osg::CameraNode* camera = new osg::CameraNode;
    camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    camera->setClearMask( GL_DEPTH_BUFFER_BIT );
    //    camera->setRenderOrder( osg::Camera::POST_RENDER );
    camera->setAllowEventFocus( true );
    camera->setViewMatrix(osg::Matrix::identity());
    //正射投影变换矩阵
    camera->setProjectionMatrix( osg::Matrix::ortho2D(left, right, bottom, top) );
    //设置渲染顺序
    camera->setRenderOrder(osg::CameraNode::POST_RENDER);
    
    return camera;
}

osgText::Text* createText( const osg::Vec3& pos, const std::string& content, float size )
{
    //创建文字
//        osg::ref_ptr<osgText::Text> text = new osgText::Text;
    osgText::Text* text = new osgText::Text;
    
    //    text->setFont( g_font.get() );

    text->setCharacterSize( size );
    text->setAxisAlignment( osgText::TextBase::XY_PLANE );
    text->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
    text->setPosition( pos );
    text->setText( content );
    text->setColor( osg::Vec4 ( 1.f, 1.f, 1.f, 1.f ) );
//    text->setBackdropType (osgText::Text ::NONE ) ;
    //设置依赖方式,选择为屏幕依赖,无论怎么旋转都会面朝屏幕
    text->setAxisAlignment(osgText::Text::SCREEN);
    
    text->setAutoRotateToScreen(true);
    return text;
}

bool OsgConverter::readIvpFile(std::string cfileName,osg::ref_ptr<osg::MatrixTransform>& parent)
{
	osg::ref_ptr<osg::Group> loadedModel = NULL;
    
//	readIvpFile(cfileName);
//	osg::ref_ptr<osg::Group> loadedModel = getRoot();
//	m_Map[cfileName] = loadedModel;

	loadedModel = m_Map[cfileName];
	if(loadedModel == NULL)
	{
        TK_Status status = readIvpFile(cfileName);
		if(!(status!= TK_Pending || status != TK_Complete))
		{

			return false;
		}

		loadedModel = getRoot();
		m_Map[cfileName] = loadedModel;
	}
	else
	{
		parent->addChild(loadedModel);
		return true;
	}

    if (loadedModel == 0)
    {
    	return false;
    }
    else
    {
    
        loadedModel->setName(cfileName);
        osg::ref_ptr<osg::StateSet> _state;
        _state = loadedModel->getOrCreateStateSet();
        osg::Program * program = new osg::Program;
        _state->setAttributeAndModes(program,osg::StateAttribute::ON);
        osg::Shader *frag = new osg::Shader(osg::Shader::FRAGMENT);
        std::string fragSource = "precision mediump float;"
        " vec3 Color = vec3(0.5, 0.5, 0.5); \n"
        " varying float LightIntensity; \n"
        " void main(void)  \n"
        "{\n"
        "    gl_FragColor = vec4 (Color * LightIntensity, 1.0);\n"
        " } \n";
        std::string replace = std::string("(0.5, 0.5, 0.5)");
        std::size_t start = fragSource.find(replace);
        std::stringstream ostr;
        ostr<<"("<<GetRGB()[0]<<","<<GetRGB()[1]<<","<<GetRGB()[2]<<")";
        fragSource.replace(start, replace.size(),ostr.str());
        frag->setShaderSource(fragSource);
        
        program->addShader(frag);
        program->addShader(new osg::Shader(osg::Shader::VERTEX,gVertexShader));
        parent->addChild(loadedModel);
        
    }
    return true;
}

std::string OsgConverter::getMenuClass(){
    
    std::string menuClass;
    
    menuClass = temString;
    
    return menuClass;
}

bool OsgConverter::readI3dFile(std::string cfileName, int step,int isFromPic, osg::ref_ptr<osg::MatrixTransform>& parent)
{

    std::string ivafinalpath;

    ivafinalpath = cfileName;

    XmlDocument readingDoc;
    
    //这个地方不能注释，unused variable
    char *buf = ((char*)unZlib(ivafinalpath.c_str()));
    bool re = readingDoc.loadFromMemory(buf,strlen(buf));
    printf("%d",re);
    
    std::string path = cfileName.substr(0,cfileName.find_last_of('/')+1);
    
    //获取i3d文件名
    std::string documentName = cfileName.substr(cfileName.find_last_of('/')+1,cfileName.find_last_of('.')-cfileName.find_last_of('/')-1);
    
    Root3D root;
    XmlNode* readingNode = readingDoc.findChild("root");
    
    root.RootName = documentName;
    
    if ( readingNode != NULL )
    {
        root.read( readingNode );
    }
    else
    {
        return false;
    }
    
    if (step == 0) {
        
        //总装图
        NSString *temStr = [NSString stringWithCString:cfileName.c_str() encoding:[NSString defaultCStringEncoding]];
        temStr = [temStr stringByDeletingPathExtension];
        temStr = [NSString stringWithFormat:@"%@.iva",temStr];
        cfileName = *new std::string([temStr UTF8String]);
        printf("%s",cfileName.c_str());
        
    }
    
    osg::ref_ptr<osg::MatrixTransform> ortransform = new osg::MatrixTransform();
    float tempM[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    ortransform->setMatrix(osg::Matrix(tempM[0],tempM[1],tempM[2],tempM[3],
                                       tempM[4],tempM[5],tempM[6],tempM[7],
                                       tempM[8],tempM[9],tempM[10],tempM[11],
                                       tempM[12],tempM[13],tempM[14],tempM[15]));
    parent->addChild(ortransform);
    ortransform->setName(cfileName);
    
    //获取step名称，返回ShowOsgViewController显示装配图的多级菜单
    temString = root.m_asmProcess.m_asmProcRoot.asmPrcocRootName;
    
    for (int j = 0; j<root.m_asmProcess.m_asmProcRoot.m_step.size(); j++) {
        
        temString = temString + ',' + root.m_asmProcess.m_asmProcRoot.m_step[j].stepName;
        
    }
    
    if (step == 0) {
        
        //显示总装图
        std::string ivaName;
        
        std::string extensionName = std::string(".iva");
        
        ivaName = path + documentName +'/'+documentName + extensionName;
        
        readAsmFile(ivaName, parent);
        
        return true;
    }
    
    if (step > 0) {
        //显示步骤图
        step = step-1;
    }
        
    for(int i=0;i<root.m_asmProcess.m_asmProcRoot.m_step[step].m_componentPackage.m_part.size();i++)
    {
        
        std::string mName = root.m_asmProcess.m_asmProcRoot.m_step[step].m_componentPackage.m_part[i].fileName;
        std::string ivPath = root.m_asmProcess.m_asmProcRoot.m_step[step].m_componentPackage.m_part[i].ivPath;
        std::string type = ivPath;
        
        std::string post = std::string(".ivp");
        
        if(type.find("ASM") != -1)
        {
            post = std::string(".iva");
        }
        else if(type.find("prt") != -1)
        {
            post = std::string(".ivp");
        }
        else
        {
            continue;
        }
        
        mName = mName.substr(0,mName.find_last_of('.'));
        std::string ivpname = mName + post;
        mName = path + documentName +'/' + mName + post;
        
        strtmp.push_back(mName.c_str());
        std::string ivpfinalpath;
        
        //        if ( 0 != ext_IntIsFileEncrypt((char*)mName.c_str())) {
        
        ivpfinalpath = mName;
        
        //        }else {
        //            ivpfinalpath = documentvector[0] + ivpname;
        //            std::string tmppath = documentvector[1] + ivpname + ".tytmp";
        //            ext_IntDecryptFile((char *)mName.c_str(), (char *)ivpfinalpath.c_str(), (char *)tmppath.c_str());
        //        }
        //        FILE *fp=fopen(mName.c_str(),"rb");
        
        FILE *fp=fopen(ivpfinalpath.c_str(),"rb");
        
        printf("%s",ivpfinalpath.c_str());
        
        if(fp == NULL) continue;
        fclose(fp);
        
        osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
        
        std::vector<float> tempM = root.m_asmProcess.m_asmProcRoot.m_step[step].m_componentPackage.m_part[i].matrix;
    
        if (isFromPic) {
            //根据图片位置显示
            tempM = root.m_view.m_stepView.m_partView[i].matrix;
        }
        
        transform->setMatrix(osg::Matrix(tempM[0],tempM[1],tempM[2],tempM[3],
                                         tempM[4],tempM[5],tempM[6],tempM[7],
                                         tempM[8],tempM[9],tempM[10],tempM[11],
                                         tempM[12],tempM[13],tempM[14],tempM[15]));
        
        if(type.find("ASM") != -1)
        {
            
            ortransform->addChild(transform);
            readAsmFile(mName,transform);
            continue;
        }
        
        if(/*readIvpFile(mName,transform)*/readIvpFile(ivpfinalpath,transform))
        {
            
            ortransform->addChild(transform);
        }
        else
        {
            
            break;
        }
        
        
    }
    
    return true;
}

bool OsgConverter::readAsmFile(std::string cfileName,osg::ref_ptr<osg::MatrixTransform>& parent)
{
    std::string ivafinalpath;
//    if ( 0 != ext_IntIsFileEncrypt((char*)cfileName.c_str())) {
        ivafinalpath = cfileName;
//    }else {
//        ivafinalpath = documentvector[0] + documentvector[2];
//        std::string tmppath = documentvector[1] + documentvector[2] + ".tytmp";
////        printf("ivafinalpath === %s", ivafinalpath.c_str());
////        printf("tmppath === %s", tmppath.c_str());
//        ext_IntDecryptFile((char *)cfileName.c_str(), (char *)ivafinalpath.c_str(),(char *)tmppath.c_str());
//    }
    
	XmlDocument readingDoc;
    
    //这个地方不能注释，unused variable
	char *buf = ((char*)unZlib(ivafinalpath.c_str()));
	bool re = readingDoc.loadFromMemory(buf,strlen(buf));
    
	std::string path = cfileName.substr(0,cfileName.find_last_of('/')+1);
	Root root;
	XmlNode* readingNode = readingDoc.findChild("root");
	if ( readingNode != NULL )
	{
		root.read( readingNode );
	}
	else
	{
		return false;
	}

	osg::ref_ptr<osg::MatrixTransform> ortransform = new osg::MatrixTransform();
	float tempM[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	ortransform->setMatrix(osg::Matrix(tempM[0],tempM[1],tempM[2],tempM[3],
									tempM[4],tempM[5],tempM[6],tempM[7],
									tempM[8],tempM[9],tempM[10],tempM[11],
									tempM[12],tempM[13],tempM[14],tempM[15]));
	parent->addChild(ortransform);
	ortransform->setName(cfileName);
    
	for(int i=0;i<root.m_Children.m_child.size();i++)
	{

    	std::string mName = root.m_Children.m_child[i].FullFileName;
    	std::string type = root.m_Children.m_child[i].child_type;

    	std::string post = std::string(".ivp");
        
		if(type.find("ASM") != -1)
		{
			post = std::string(".iva");
		}
		else if(type.find("PRT") != -1)
		{
			post = std::string(".ivp");
		}
		else
		{
			continue;
		}
        
        mName = mName.substr(0,mName.find_last_of('.'));
        std::string ivpname = mName + post;
        mName = path + mName + post;
        strtmp.push_back(mName.c_str());
        std::string ivpfinalpath;
        
//        if ( 0 != ext_IntIsFileEncrypt((char*)mName.c_str())) {

        ivpfinalpath = mName;
            
//        }else {
//            ivpfinalpath = documentvector[0] + ivpname;
//            std::string tmppath = documentvector[1] + ivpname + ".tytmp";
//            ext_IntDecryptFile((char *)mName.c_str(), (char *)ivpfinalpath.c_str(), (char *)tmppath.c_str());
//        }
//        FILE *fp=fopen(mName.c_str(),"rb");
        
        FILE *fp=fopen(ivpfinalpath.c_str(),"rb");
        if(fp == NULL)
            continue;
        fclose(fp);

    	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
    	std::vector<float> tempM = root.m_Children.m_child[i].m_Orign.Matrix;
    	transform->setMatrix(osg::Matrix(tempM[0],tempM[1],tempM[2],tempM[3],
										tempM[4],tempM[5],tempM[6],tempM[7],
										tempM[8],tempM[9],tempM[10],tempM[11],
										tempM[12],tempM[13],tempM[14],tempM[15]));

    
		if(type.find("ASM") != -1)
		{

			ortransform->addChild(transform);
            readAsmFile(mName,transform);
			continue;
		}


        if(/*readIvpFile(mName,transform)*/readIvpFile(ivpfinalpath,transform))
        {
     
        	ortransform->addChild(transform);
        }
        else
        {
            
        	break;
        }


	}
	return true;
}

void OsgConverter::readFile(std::string cfileName,std::vector<child>& vload)
{
	XmlDocument readingDoc;
	char *buf = (char*)unZlib(cfileName.c_str());
	char *ubuf = ANSI2UTF8(buf);
	free(buf);
	buf = NULL;

	readingDoc.loadFromMemory(ubuf,strlen(ubuf));

	std::string path = cfileName.substr(0,cfileName.find_last_of('/')+1);
	Root root;
	XmlNode* readingNode = readingDoc.findChild("root");
	if ( readingNode != NULL )
	{
		root.read( readingNode );
	}
	for(int i=0;i<root.m_Children.m_child.size();i++)
	{
    	std::string mName = root.m_Children.m_child[i].FullFileName;
    	std::string type = root.m_Children.m_child[i].child_type;
		if(type.find("ASM") != -1)
		{
	        std::string replace = std::string(".ASM");
	        std::size_t start = mName.find(replace);
	        mName.replace(start, replace.size(),".iva");
	        mName = path + mName;
			readFile(mName,vload);
			continue;
		}

        std::string replace = std::string(".PRT");
        std::size_t start = mName.find(replace);
        mName.replace(start, replace.size(),".ivp");
        mName = path + mName;
        root.m_Children.m_child[i].FullFileName = mName;

		vload.push_back(root.m_Children.m_child[i]);
	}

}
TK_Status OsgConverter::readIvpFile(std::string cfileName)
{
	m_geometry = NULL;
	m_array3 = NULL;
	m_aNormal = NULL;
	m_array = NULL;
	m_i = 0;

	TK_Status status;
	BStreamFileToolkit tk;
	tk.SetOpcodeHandler('S', new OSG_Shell);
	tk.SetOpcodeHandler('~', new OSG_Color_RGB);
	status = TK_Read_Stream_File (cfileName.c_str(), &tk);
	return status;
}
void OsgConverter::OSG_Open_Segment(const char* name)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float radius = 0.08f;
	float height = 0.1f;
	osg::TessellationHints* hints = new osg::TessellationHints;
	hints->setDetailRatio(1.0f);//‘Ω¥Û‘Ωœ∏ƒÂ
	geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),radius),hints));

	if (m_root == NULL)
	{
		m_root = new osg::Group();
	}
	m_root->addChild(geode.get());
}

osg::ref_ptr<osg::Group> OsgConverter::getRoot()
{
    if(NULL == m_geometry)
    {
        return 0;
    }
	m_geometry->setVertexArray(m_array3);
	m_geometry->setNormalArray(m_aNormal);
	m_geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);//»Áπ˚“™¡¢º¥œ‘ æ£¨±ÿ–Îº”…œ’‚∂Œ
	m_geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, m_array->size(), &m_array->front()));
	m_geode->addDrawable(m_geometry);
    m_geometry->setUseVertexBufferObjects(true);
    
	osg::ref_ptr<osg::Group> root = new osg::Group();
	root->addChild(m_geode.get());
    
    m_geometry->setUseVertexBufferObjects(true);
	m_geode = NULL;
    
	return root;
}


void OsgConverter::OSG_Insert_Shell(int iPointCount,float *pPoints,float *pNormals,int iFaceCount,int *iFaceIndex,double *face_rgb)
{
	//osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	if (m_geode == NULL)
	{
		m_geode = new osg::Geode();
	}

	if (m_geometry == NULL)
	{
		m_geometry = new osg::Geometry;
	}

	//m_geode->addDrawable(m_geometry);//»√geometry◊˜Œ™“ª∏ˆªÊÕº∂‘œÛº”»ÎµΩΩ⁄µ„÷–
	//ÃÌº”∂•µ„
	//osg::ref_ptr<osg::Vec3Array> array3 = new osg::Vec3Array;
	int i;
	//for(i=0;i<iPointCount;i++)
	//{
	//	array3->push_back(osg::Vec3(pPoints[3*i],pPoints[3*i+1],pPoints[3*i+2]));
	//}
	//geometry->setVertexArray(array3);

	if (m_array3 == NULL)
	{
		m_array3 = new osg::Vec3Array;
	}

	for(i=0;i<iPointCount;i++)
	{
		m_array3->push_back(osg::Vec3(pPoints[3*i],pPoints[3*i+1],pPoints[3*i+2]));
	}

	//ÃÌº”∑® ∏
	//osg::ref_ptr<osg::Vec3Array> aNormal = new osg::Vec3Array;
	//for(i=0;i<iPointCount;i++)
	//{
	//	aNormal->push_back(osg::Vec3(pNormals[3*i],pNormals[3*i+1],pNormals[3*i+2]));
	//}
	//geometry->setNormalArray(aNormal);
	//geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);//»Áπ˚“™¡¢º¥œ‘ æ£¨±ÿ–Îº”…œ’‚∂Œ

	if (m_aNormal == NULL)
	{
		m_aNormal = new osg::Vec3Array;
	}

	for(i=0;i<iPointCount;i++)
	{
		m_aNormal->push_back(osg::Vec3(pNormals[3*i],pNormals[3*i+1],pNormals[3*i+2]));
	}

	//for( i=0;i<iFaceCount/4;i++)
	//{
	//	osg::ref_ptr<osg::DrawElementsUInt> elem = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
	//	elem->push_back(iFaceIndex[i*4+1]);
	//	elem->push_back(iFaceIndex[i*4+2]);
	//	elem->push_back(iFaceIndex[i*4+3]);
	//	geometry->addPrimitiveSet(elem);
	//}

	//lis 2014.3.11
	//osg::ref_ptr<osg::UIntArray> array = new osg::UIntArray;
	//for (int i = 0; i <iFaceCount/4; i++)
	//{
	//	array->push_back(iFaceIndex[i*4+1]);
	//	array->push_back(iFaceIndex[i*4+2]);
	//	array->push_back(iFaceIndex[i*4+3]);

	//}
	//geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, array->size(), &array->front()));

	if (m_array == NULL)
	{
		m_array = new osg::UIntArray;
	}

	for (int i = 0; i <iFaceCount/4; i++)
	{
		m_array->push_back(iFaceIndex[i*4+1]+m_i);
		m_array->push_back(iFaceIndex[i*4+2]+m_i);
		m_array->push_back(iFaceIndex[i*4+3]+m_i);

	}
	m_i += iPointCount;

	////…Ë÷√—’…´
//    if (m_rgb != NULL)
//	{
//		osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;
//		m_geometry->setColorArray(c.get());
//		m_geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
//		c->push_back(osg::Vec4(m_rgb[0], m_rgb[1], m_rgb[2],1.f));
////        c->push_back(osg::Vec4(1.0, 0.0, 0.0,1.f));
//	}
}
