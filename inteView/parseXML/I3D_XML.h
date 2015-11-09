//
//  I3D_XML.h
//  inteView
//
//  Created by Sunc on 15/10/28.
//  Copyright (c) 2015年 whtysf. All rights reserved.
//

#pragma once
#include "SlimXml.h"
#include <vector>

namespace I3D_XML
{
    
    struct part{
        
        void read( const slim::XmlNode* node );
        void write( slim::XmlNode* node ) const;
        
        std::string ivPath;
        std::string fileName;
        std::string matrixStr;
        std::vector<float> matrix;
        std::vector<float> originalMatrix;
        
    };
    
    struct componentPackage{
        
        void read( const slim::XmlNode* node );
        void write( slim::XmlNode* node ) const;
        
        std::vector<part> m_part;
    };
    
    struct step{
        
        void read( const slim::XmlNode* node );
        void write( slim::XmlNode* node ) const;
        
        std::string stepName;
        componentPackage m_componentPackage;
        
    };
    
    struct OrignMatrix
    {
        void read( const slim::XmlNode* node );
        void write( slim::XmlNode* node ) const;
        
        std::vector<float>  Matrix;
    };
    
    struct Matrix
    {
        void read( const slim::XmlNode* node );
        void write( slim::XmlNode* node ) const;
        
        std::vector<float>  Matrix;
    };
    
    struct asmProcRoot{
        void read( const slim::XmlNode* node );
        void write( slim::XmlNode* node ) const;
        
        std::string asmPrcocRootName;
//        step m_step;
        std::vector<step> m_step;
    };
    
    struct partView{
        
        void read( const slim::XmlNode* node);
        void write( slim::XmlNode* node) const;
        
        std::string matrixStr;
        std::string matrixKey;
        std::vector<float> matrix;
    };
    
    struct stepView{
        
        void read( const slim::XmlNode* node);
        void write( slim::XmlNode* node) const;
        
        std::string fileName;
        std::string ModelData;
        std::string stepViewName;
        
        partView m_partView;
        
    };
    
    struct View{
        
        void read( const slim::XmlNode* node);
        void write( slim::XmlNode* node) const;
        
        stepView m_stepView;
        std::string viewName;
        
    };
    
    struct fileNode
    {
        void read( const slim::XmlNode* node);
        void write( slim::XmlNode* node) const;
        
        std::string documentName;
        std::string fileName;
        std::string ModelData;
        
        std::string fileNodeName;
    };
    
    struct modelSpace
    {
        void read( const slim::XmlNode* node);
        void write( slim::XmlNode* node) const;
        
        std::vector<fileNode> m_fileNode;
        std::string mainFile;
        
        std::string modelSpaceName;
    };
    
    struct asmProcess
    {
        void read( const slim::XmlNode* node);
        void write( slim::XmlNode* node) const;
        
        asmProcRoot m_asmProcRoot;
    };
    
    struct Root3D
    {
        void read( const slim::XmlNode* node );
        void write( slim::XmlNode* node ) const;
        
        std::string  Version;
        std::string  Type;
        
        asmProcess m_asmProcess;
        modelSpace m_modelSpace;
        View m_view;
        
        std::string RootName;
//        View m_view;
    };
    
    //create by sunc 2015-10-28
}
