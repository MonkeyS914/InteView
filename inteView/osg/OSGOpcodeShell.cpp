
#include "OSGOpcodeShell.h"



TK_Status OSG_Shell::Execute (BStreamFileToolkit & tk) alter
{
	TK_Status	status = TK_Normal;
	//status = HTK_Shell::Execute(tk);

	bool re = false;
	re = HasVertexNormals();
	re = HasFaceNormals();
	OsgConverter::OSG_Insert_Shell(mp_pointcount,mp_points,GetVertexNormals(),m_flistlen,m_flist);

	return status;
}
