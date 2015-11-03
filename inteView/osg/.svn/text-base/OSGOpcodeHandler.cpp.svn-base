
#include "OSGOpcodeHandler.h"
#include "OsgConverter.h"

TK_Status OSG_Color_RGB::Execute (BStreamFileToolkit & tk) alter 
{
	auto   status = TK_Normal;     

	//if (tk.GeometryIsOpen()) {
	//	auto	HC_KEY					last_key;

	//	if ((status = tk.GetLastKey(last_key)) != TK_Normal)
	//		return status;
	//	HC_Open_Geometry (last_key);
	//}

	//auto		char					buffer[4096];
	//auto		char					*tmp;

	//tmp = make_type_string(m_mask, buffer);

	//if (tmp[-1] == '=') {
	//	tmp[-1] = '\0';
	//	HC_Set_Color_By_Value (buffer, "RGB", m_rgb[0], m_rgb[1], m_rgb[2]);
	//}

	//if (tk.GeometryIsOpen())
	//	HC_Close_Geometry ();

	OsgConverter::SetRGB(m_rgb);
	return status;
}