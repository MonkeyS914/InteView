
#include "BStream.h"
#include "BOpcodeHandler.h"
#include "BStreamFileToolkit.h"
#include "OsgConverter.h"
class OSG_Color_RGB : public TK_Color_RGB {
public:
	/*! constructor */
	OSG_Color_RGB (int mask=0, float const * rgb=0)
		: TK_Color_RGB () {
			m_mask = mask;
			if (rgb != 0) {
				m_rgb[0] = rgb[0];  m_rgb[1] = rgb[1];  m_rgb[2] = rgb[2];
			}
	}

	TK_Status   Execute (BStreamFileToolkit & tk) alter;
	//TK_Status   Interpret (BStreamFileToolkit & tk, ID_Key key, int variant=0) alter;
	//TK_Status   Interpret (BStreamFileToolkit & tk, ID_Key key, char const * special) alter
	//{ return TK_Color_RGB::Interpret(tk, key, special); }
	//TK_Status   Clone (BStreamFileToolkit & tk, BBaseOpcodeHandler **handler) const;
};
