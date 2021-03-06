#include "IVA_XML.h"
#include <cassert>

using namespace slim;
#include <string.h>
#define  LOG_TAG    "IVA_XML"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)

namespace IVA_XML
{
	void Root::read( const XmlNode* node )
	{
		assert( node != NULL );

		NodeIterator iter;
		const XmlNode* childNode = NULL;
		const XmlAttribute* attribute = NULL;

		attribute = node->findAttribute("version" );
		if ( attribute != NULL )
		{
			Version = attribute->getString();
		}

		attribute = node->findAttribute("type" );
		if ( attribute != NULL )
		{
			Type = attribute->getString();
		}

		childNode = node->findChild("AsmInfo" );
		if ( childNode != NULL )
		{
			m_Asminfo.read(childNode);
		}

		childNode = node->findChild( "Children" );
		if ( childNode != NULL )
		{
			m_Children.read(childNode);
		}
	}

	void Root::write( XmlNode* node ) const
	{

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	void Asminfo::read( const XmlNode* node )
	{
		assert( node != NULL );

		NodeIterator iter;
		const XmlNode* childNode = NULL;
		const XmlAttribute* attribute = NULL;

		childNode = node->findChild( "Params" );
		if ( childNode != NULL )
		{
			m_Params.read(childNode);
		}
	}

	void Asminfo::write( XmlNode* node ) const
	{

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	void Params::read( const XmlNode* node )
	{
		assert( node != NULL );

		NodeIterator iter;
		const XmlNode* childNode = NULL;
		const XmlAttribute* attribute = NULL;

		childNode = node->findChild( "FullName" );
		if ( childNode != NULL )
		{
			FullName = childNode->getString();
		}
	}

	void Params::write( XmlNode* node ) const
	{

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	void Children::read( const XmlNode* node )
	{
		assert( node != NULL );

		NodeIterator iter;
		const XmlNode* childNode = NULL;
		const XmlAttribute* attribute = NULL;

		childNode = node->findFirstChild( "child", iter );
		while ( childNode != NULL )
		{
			m_child.resize( m_child.size() + 1 );
			m_child.back().read( childNode );
			childNode = node->findNextChild( "child", iter );

		}
	}

	void Children::write( XmlNode* node ) const
	{

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	void child::read( const XmlNode* node )
	{
		assert( node != NULL );

		NodeIterator iter;
		const XmlNode* childNode = NULL;
		const XmlAttribute* attribute = NULL;

		attribute = node->findAttribute( "Type" );
		if ( attribute != NULL )
		{
			child_type = attribute->getString();
		}

		attribute = node->findAttribute( "RelPath" );
		if ( attribute != NULL )
		{
//			FullFileName = attribute->getString();
//			FullFileName = FullFileName.substr(2);
            FullFileName = attribute->getString();
            if(FullFileName.size()>=2)
            {
                FullFileName = FullFileName.substr(2);
                
                printf("%s",FullFileName.c_str());
            }
		}

		childNode = node->findChild( "Orign" );
		if ( childNode != NULL )
		{
			m_Orign.read(childNode);
		}
	}

	void child::write( XmlNode* node ) const
	{

	}

///////////////////////////////////////////////////////////////////////////////////////////////////
	void Orign::read( const XmlNode* node )
	{
		assert( node != NULL );

		NodeIterator iter;
		const XmlNode* childNode = NULL;
		const XmlAttribute* attribute = NULL;

		char mat[256];
		for (int i=0;i<16;i++)
		{
			sprintf(mat,"%s%d","M",i);
			attribute = node->findAttribute( mat );
			if ( attribute != NULL )
			{
				float num= attribute->getFloat();
				Matrix.push_back(num);
			}
		}
	}

	void Orign::write( XmlNode* node ) const
	{

	}

}
