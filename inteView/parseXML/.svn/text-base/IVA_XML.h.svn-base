#pragma once
#include "SlimXml.h"
#include <vector>

namespace IVA_XML
{

	struct Params
	{
		void read( const slim::XmlNode* node );
		void write( slim::XmlNode* node ) const;

		//std::string DisplayName;
		std::string  FullName;
	};

	struct Asminfo
	{
		void read( const slim::XmlNode* node );
		void write( slim::XmlNode* node ) const;

		Params m_Params;
	};

	struct Orign
	{
		void read( const slim::XmlNode* node );
		void write( slim::XmlNode* node ) const;

		std::vector<float>   Matrix;
	};

	struct child
	{
		void read( const slim::XmlNode* node );
		void write( slim::XmlNode* node ) const;

		std::string  child_type;
		std::string  FullFileName;
		Orign m_Orign;
	};

	struct Children
	{
		void read( const slim::XmlNode* node );
		void write( slim::XmlNode* node ) const;

		std::vector<child> m_child;
	};

	struct Root
	{
		void read( const slim::XmlNode* node );
		void write( slim::XmlNode* node ) const;

		std::string  Version;
		std::string  Type;
		Asminfo m_Asminfo;
		Children m_Children;
	};
}
