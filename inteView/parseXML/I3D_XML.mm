//
//  I3D_XML.cpp
//  inteView
//
//  Created by Sunc on 15/10/28.
//  Copyright (c) 2015年 whtysf. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "I3D_XML.h"
#include <cassert>

using namespace slim;
#include <string.h>
#define  LOG_TAG    "I3D_XML"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)

namespace I3D_XML
{
    //读取root节信息
    void Root3D::read( const XmlNode* node )
    {
        
        assert( node != NULL );
        
        NodeIterator iter;
        const XmlNode* childNode = NULL;
        const XmlAttribute* attribute = NULL;
        
        //1.0.0
        attribute = node->findAttribute("version" );
        if ( attribute != NULL )
        {
            Version = attribute->getString();
        }
        
        //APS
        attribute = node->findAttribute("type" );
        if ( attribute != NULL )
        {
            Type = attribute->getString();
        }
        
        //读取根节点下子节点asmProcess
        childNode = node->findChild("asmProcess" );
        if ( childNode != NULL )
        {
            m_asmProcess.read(childNode);
        }
        
        //读取根节点下面的modelSpace，与asminfo并行
        childNode = node->findChild( "ModelSpace" );
        if ( childNode != NULL )
        {
            m_modelSpace.modelSpaceName = RootName;
            
            m_modelSpace.read(childNode);
            
        }
        
        //读取根节点下面的View，与asminfo并行
        childNode = node->findChild( "View" );
        if ( childNode != NULL )
        {
            m_view.viewName = RootName;
            
            m_view.read(childNode);
            
        }
    }
    
    void Root3D::write( XmlNode* node ) const
    {
        
    }
    
    //modelSpace 储存iva和ivp的数据
    void modelSpace::read(const XmlNode *node){
        assert( node != NULL );
        
        NodeIterator iter;
        const XmlNode* childNode = NULL;
        const XmlAttribute* attribute = NULL;
        
        attribute = node->findAttribute("mainFile");
        if (attribute != NULL) {
            mainFile = attribute->getString();
        }
        
        childNode = node->findFirstChild( "FileNode", iter );
        while ( childNode != NULL )
        {
            m_fileNode.resize( m_fileNode.size() + 1 );
            
            m_fileNode.back().fileNodeName = modelSpaceName;
            
            m_fileNode.back().read( childNode );
            
            //读取下一个step节点的信息
            childNode = node->findNextChild( "FileNode", iter );
        }
    }
    
    void modelSpace::write(slim::XmlNode *node) const{
        
    }
    
    //View 储存图片的数据
    void View::read(const XmlNode *node){
        assert( node != NULL );
        
        NodeIterator iter;
        const XmlNode* childNode = NULL;
//        const XmlAttribute* attribute = NULL;
        
        childNode = node->findFirstChild(iter);
        
        while ( childNode != NULL )
        {
            m_stepView.stepViewName = viewName;
            m_stepView.read(childNode);
            //读取下一个step节点的信息
            childNode = node->findNextChild(iter);
            
        }
    }
    
    void View::write(slim::XmlNode *node) const{
        
    }
    
    //View 储存图片的数据
    void stepView::read(const XmlNode *node){
        assert( node != NULL );
        
        NodeIterator iter;
        const XmlNode* childNode = NULL;
        const XmlAttribute* attribute = NULL;
        
        //读取根节点下面的View，与asminfo并行
        childNode = node->findChild( "Part" );
        if ( childNode != NULL )
        {
            
            m_partView.read(childNode);
            
        }
        
        attribute = node->findAttribute( "viewName" );
        if ( attribute != NULL )
        {
            fileName = attribute->getString();
        }
        
        attribute = node->findAttribute("ImageData");
        
        if (attribute != NULL) {
            
            ModelData = attribute->getString();
            
            //这里注意转码问题，中文字符用下面coding方式，不然会乱码
            NSStringEncoding enc = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_18030_2000);
            
            NSString *dataStr = [NSString stringWithCString:ModelData.c_str() encoding:[NSString defaultCStringEncoding]];
            NSString *nameStr = [NSString stringWithCString:fileName.c_str() encoding:enc];
            
            if ([nameStr rangeOfString:@"视图1"].location == NSNotFound ) {
                
                return;
            }
            
            NSString *docName = [NSString stringWithCString:stepViewName.c_str() encoding:[NSString defaultCStringEncoding]];
            
            //base64Decode
            NSData *theData = [[NSData alloc]
                               initWithBase64EncodedString:dataStr options:NSDataBase64DecodingIgnoreUnknownCharacters];
            
            //创建文件管理器
            NSFileManager *fileManager = [NSFileManager defaultManager];
            
            //获取document路径,括号中属性为当前应用程序独享
            
            NSArray *directoryPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            
            NSString *documentDirectory = [directoryPaths objectAtIndex:0];
            
            //定义记录文件全名以及路径的字符串filePath
            NSString *docFilePath = [documentDirectory stringByAppendingPathComponent:docName];
            
            NSString *filePath = [docFilePath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.png",nameStr]];
            
            //创建子文件夹
            BOOL isDir = NO;
            BOOL existed = [fileManager fileExistsAtPath:docFilePath isDirectory:&isDir];
            if ( !(isDir == YES && existed == YES) )
            {
                [fileManager createDirectoryAtPath:docFilePath withIntermediateDirectories:YES attributes:nil error:nil];
            }
            
            //查找文件，如果不存在，就创建一个文件
            if (![fileManager fileExistsAtPath:filePath]) {
                
                [fileManager createFileAtPath:filePath contents:theData attributes:nil];
                
            }

        }
        
    }
    
    void stepView::write(slim::XmlNode *node) const{
        
    }
    
    //partView
    void partView::read(const XmlNode *node)
    {
        assert( node != NULL );
        
        NodeIterator iter;
        //        const XmlNode* childNode = NULL;
        const XmlAttribute* attribute = NULL;
        
        for (int i = 0; i<16; i++) {
            
            matrixKey = 'M'+i;
            
            //string转const char
            const char* c_s = matrixKey.c_str();
            
            //matrixStr
            attribute = node->findAttribute( c_s );
            if ( attribute != NULL )
            {
                matrixStr = attribute->getString();
            }
            
            //把matrix字符串转成vector
            
            const char *str = matrixStr.c_str();
            float floatValue = atof(str);
            matrix.push_back(floatValue);
        }
    }
    
    //fileNode
    void fileNode::read(const XmlNode *node){
        assert( node != NULL );
        
        NodeIterator iter;
//        const XmlNode* childNode = NULL;
        const XmlAttribute* attribute = NULL;
        
        attribute = node->findAttribute( "fileName" );
        if ( attribute != NULL )
        {
            fileName = attribute->getString();
        }
        
        attribute = node->findAttribute("ModelData");
        
        if (attribute != NULL) {
            ModelData = attribute->getString();
            
            printf("%s",fileNodeName.c_str());
            
            
            NSString *dataStr = [NSString stringWithCString:ModelData.c_str() encoding:[NSString defaultCStringEncoding]];
            NSString *nameStr = [NSString stringWithCString:fileName.c_str() encoding:[NSString defaultCStringEncoding]];
            NSString *docName = [NSString stringWithCString:fileNodeName.c_str() encoding:[NSString defaultCStringEncoding]];
           
            //base64Decode
            NSData *theData = [[NSData alloc]
                                              initWithBase64EncodedString:dataStr options:NSDataBase64DecodingIgnoreUnknownCharacters];
            
            //创建文件管理器
            NSFileManager *fileManager = [NSFileManager defaultManager];
            
            //获取document路径,括号中属性为当前应用程序独享
            
            NSArray *directoryPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            
            NSString *documentDirectory = [directoryPaths objectAtIndex:0];
            
            //定义记录文件全名以及路径的字符串filePath
            NSString *docFilePath = [documentDirectory stringByAppendingPathComponent:docName];
            
            NSString *filePath = [docFilePath stringByAppendingPathComponent:nameStr];
            
            //创建子文件夹
            BOOL isDir = NO;
            BOOL existed = [fileManager fileExistsAtPath:docFilePath isDirectory:&isDir];
            if ( !(isDir == YES && existed == YES) )
            {
                [fileManager createDirectoryAtPath:docFilePath withIntermediateDirectories:YES attributes:nil error:nil];
            }

            //查找文件，如果不存在，就创建一个文件
            if (![fileManager fileExistsAtPath:filePath]) {
            
                [fileManager createFileAtPath:filePath contents:theData attributes:nil];
                
            }
        }
        
    }
    
    //asmProcess
    void asmProcess::read( const XmlNode* node )
    {
        assert( node != NULL );
        
        NodeIterator iter;
        const XmlNode* childNode = NULL;
//        const XmlAttribute* attribute = NULL;
        
        //读取asmProcRoot信息
        childNode = node->findChild( "asmProcRoot" );
        if ( childNode != NULL )
        {
            m_asmProcRoot.read(childNode);
        }
        
        
    }
    
    void asmProcess::write( XmlNode* node ) const
    {
        
    }
    
    //asmProcRoot
    void asmProcRoot::read(const XmlNode *node)
    {
        assert( node != NULL );
        
        NodeIterator iter;
        const XmlNode* childNode = NULL;
        const XmlAttribute* attribute = NULL;
        
        //stepName
        attribute = node->findAttribute("name" );
        
        if ( attribute != NULL )
        {
            asmPrcocRootName = attribute->getString();
        }
        
//        读取step信息
        childNode = node->findFirstChild( "step", iter );
        
        while ( childNode != NULL )
        {
            m_step.resize( m_step.size() + 1 );
            m_step.back().read( childNode );
            
            //读取下一个step节点的信息
            childNode = node->findNextChild( "step", iter );
            
        }
        
    }
    
    void asmProcRoot::write( XmlNode *node) const{
        
    }
    
    //step
    void step::read(const XmlNode *node)
    {
        assert( node != NULL );
        
        NodeIterator iter;
        const XmlNode* childNode = NULL;
        const XmlAttribute* attribute = NULL;
        
        //stepName
        attribute = node->findAttribute( "name" );
        if ( attribute != NULL )
        {
            stepName = attribute->getString();
        }
        
        //读取componentPackage信息
        childNode = node->findChild( "componentPackage" );
        if ( childNode != NULL )
        {
            m_componentPackage.read(childNode);
        }
    }
    
    //componentPackage
    void componentPackage::read(const XmlNode *node)
    {
        assert( node != NULL );
        
        NodeIterator iter;
        const XmlNode* childNode = NULL;
//        const XmlAttribute* attribute = NULL;
        
        //读取part信息
        
        childNode = node->findFirstChild( "part", iter );
        while ( childNode != NULL )
        {
            m_part.resize( m_part.size() + 1 );
            m_part.back().read( childNode );
            
            //读取下一个step节点的信息
            childNode = node->findNextChild( "part", iter );
        }
        
    }
    
    //part
    void part::read(const XmlNode *node)
    {
        assert( node != NULL );
        
        NodeIterator iter;
//        const XmlNode* childNode = NULL;
        const XmlAttribute* attribute = NULL;
        
        //零件名
        attribute = node->findAttribute( "name" );
        if ( attribute != NULL )
        {
            fileName = attribute->getString();
            printf("%s",fileName.c_str());
        }
        
        //ivpath
        attribute = node->findAttribute( "ivPath" );
        if ( attribute != NULL )
        {
            ivPath = attribute->getString();
            printf("%s",fileName.c_str());
        }
        
        //matrixStr
        attribute = node->findAttribute( "matrix" );
        if ( attribute != NULL )
        {
            matrixStr = attribute->getString();
        }
        
        //把matrix字符串转成vector
        
        const char *str = matrixStr.c_str();
        const char * split = ",";
        char * p;
        char *buf = new char[strlen(str)+1];
        strcpy(buf, str);
        
        p = strtok (buf,split);
        
        while(p!=NULL) {
            printf ("%s\n",p);
            float floatValue = atof(p);
            matrix.push_back(floatValue);
            p = strtok(NULL,split); 
        }
        
    }
    
    void step::write( XmlNode *node) const{
        
    }
}

