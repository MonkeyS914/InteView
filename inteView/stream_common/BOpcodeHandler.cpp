//
// Copyright (c) 2000 by Tech Soft 3D, LLC.
// The information contained herein is confidential and proprietary to
// Tech Soft 3D, LLC., and considered a trade secret as defined under
// civil and criminal statutes.  Tech Soft 3D shall pursue its civil
// and criminal remedies in the event of unauthorized use or misappropriation
// of its trade secrets.  Use of this information by anyone other than
// authorized employees of Tech Soft 3D, LLC. is granted only under a
// written non-disclosure agreement, expressly prescribing the scope and
// manner of such use.
//
// $Header: /files/homes/master/cvs/hoops_master/shared/stream_common/BOpcodeHandler.cpp,v 1.435.2.4 2009-06-25 00:28:31 covey Exp $
//

#include "BStream.h"
#include "BInternal.h"
#include "vwrapper_stream.h"
#include <ctype.h>
#define  LOG_TAG    "HoopsStreamTest"   //����logcat��tag��ǩ
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#ifndef DISABLE_STREAM_JPEG
	#define XMD_H
	#ifdef FAR
	#  undef FAR
	#endif

	extern "C" {
		//# include "jpeglib.h"
	}
	#include <setjmp.h>
#endif

#define ASSERT(x) 



static char const * opcode_string[256] = {
	"Termination         ", "Pause               ", "-- 0x02 --          ", "-- 0x03 --          ", 
	"-- 0x04 --          ", "-- 0x05 --          ", "-- 0x06 --          ", "Callback            ", 
	"Color_By_Index      ", "Color_By_Index_16   ", "Color_By_FIndex     ", "Color_By_Value      ", 
	"Color_Map           ", "Edge_Pattern        ", "Edge_Weight         ", "[Rsrv:Complex_Clip] ", 

	"PolyPolyline        ", "Infinite_Ray        ", "External_Reference  ", "External_Ref_Unicode", 
	"Thumbnail           ", "URL                 ", "Unicode_Options     ", "Geometry_Options    ", 
	"XML                 ", "LOD                 ", "Sphere              ", "-- 0x1B --          ", 
	"-- 0x1C --          ", "-- 0x1D --          ", "-- 0x1E --          ", "-- 0x1F --          ", 

	"Text_Spacing        ", "Selectability       ", "Color               ", "Window_Frame        ", 
	"Texture_Matrix      ", "Modelling_Matrix    ", "Repeat_Object       ", "-- ' --             ", 
	"Open_Segment        ", "Close_Segment       ", "Text_Alignment      ", "Marker_Size         ", 
	"<Streaming_Mode>    ", "Line_Pattern        ", "Local_Light         ", "Cutting_Plane       ", 

	"Priority            ", "-- 1 --             ", "-- 2 --             ", "-- 3 --             ", 
	"-- 4 --             ", "-- 5 --             ", "-- 6 --             ", "-- 7 --             ", 
	"-- 8 --             ", "-- 9 --             ", "Geometry_Attibutes  ", "Comment             ", 
	"Include_Segment     ", "Line_Weight         ", "Camera              ", "Conditional         ", 

	"Marker_Symbol       ", "NURBS_Surface       ", "Bounding_Info       ", "Circle              ", 
	"Dictionary          ", "Ellipse             ", "Text_Font           ", "Polygon             ", 
	"Heuristics          ", "File_Info           ", "Line_Style          ", "Renumber_Key_Global ", 
	"Polyline            ", "Mesh                ", "NURBS_Curve         ", "Clip_Region         ", 

	"Face_Pattern        ", "PolyCylinder        ", "Rendering_Options   ", "Shell               ", 
	"Text                ", "User_Options        ", "Visibility          ", "Window              ", 
	"Marker              ", "Cylinder            ", "Start_Compression   ", "Start_User_Data     ", 
	"Circular_Chord      ", "Stop_User_Data      ", "Spot_Light          ", "Dictionary_Locater  ", 

	"Infinite_Line       ", "Area_Light          ", "Bounding            ", "Circular_Arc        ", 
	"Distant_Light       ", "Elliptical_Arc      ", "Font                ", "Grid                ", 
	"Handedness          ", "Image               ", "Glyph Definition    ", "Renumber_Key_Local  ", 
	"Line                ", "-- m --             ", "User_Index          ", "Clip_Rectangle      ", 

	"Window_Pattern      ", "Tag                 ", "Reference           ", "Reopen_Segment      ", 
	"Texture             ", "-- u --             ", "User_Value          ", "Circular_Wedge      ", 
	"Text_With_Encoding  ", "-- y --             ", "Stop_Compression    ", "Style_Segment       ", 
	"Text_Path           ", "View                ", "Color_RGB           ", "Delete_Object       ", 

	"-- 0x80 --          ", "-- 0x81 --          ", "-- 0x82 --          ", "-- 0x83 --          ",
	"-- 0x84 --          ", "-- 0x85 --          ", "-- 0x86 --          ", "-- 0x87 --          ",
	"-- 0x88 --          ", "-- 0x88 --          ", "-- 0x8A --          ", "-- 0x8B --          ",
	"-- 0x8C --          ", "-- 0x8D --          ", "-- 0x8E --          ", "-- 0x8F --          ",
	"-- 0x90 --          ", "-- 0x91 --          ", "-- 0x92 --          ", "-- 0x93 --          ",
	"-- 0x94 --          ", "-- 0x95 --          ", "-- 0x96 --          ", "-- 0x97 --          ",
	"-- 0x98 --          ", "-- 0x98 --          ", "-- 0x9A --          ", "-- 0x9B --          ",
	"-- 0x9C --          ", "-- 0x9D --          ", "-- 0x9E --          ", "-- 0x9F --          ",

	"-< 0xA0 >-          ", "-< 0xA1 >-          ", "-< 0xA2 >-          ", "-< 0xA3 >-          ",	
	"-< 0xA4 >-          ", "-< 0xA5 >-          ", "-< 0xA6 >-          ", "-< 0xA7 >-          ",	
	"-< 0xA8 >-          ", "-< 0xA8 >-          ", "-< 0xAA >-          ", "-< 0xAB >-          ",	
	"-< 0xAC >-          ", "-< 0xAD >-          ", "-< 0xAE >-          ", "-< 0xAF >-          ",	
	"-< 0xB0 >-          ", "-< 0xB1 >-          ", "-< 0xB2 >-          ", "-< 0xB3 >-          ",	
	"-< 0xB4 >-          ", "-< 0xB5 >-          ", "-< 0xB6 >-          ", "-< 0xB7 >-          ",	
	"-< 0xB8 >-          ", "-< 0xB8 >-          ", "-< 0xBA >-          ", "-< 0xBB >-          ",	
	"-< 0xBC >-          ", "-< 0xBD >-          ", "-< 0xBE >-          ", "-< 0xBF >-          ",	
	"-< 0xC0 >-          ", "-< 0xC1 >-          ", "-< 0xC2 >-          ", "-< 0xC3 >-          ",	
	"-< 0xC4 >-          ", "-< 0xC5 >-          ", "-< 0xC6 >-          ", "-< 0xC7 >-          ",	
	"-< 0xC8 >-          ", "-< 0xC8 >-          ", "-< 0xCA >-          ", "-< 0xCB >-          ",	
	"-< 0xCC >-          ", "-< 0xCD >-          ", "-< 0xCE >-          ", "-< 0xCF >-          ",	
	"-< 0xD0 >-          ", "-< 0xD1 >-          ", "-< 0xD2 >-          ", "-< 0xD3 >-          ",	
	"-< 0xD4 >-          ", "-< 0xD5 >-          ", "-< 0xD6 >-          ", "-< 0xD7 >-          ",	
	"-< 0xD8 >-          ", "-< 0xD8 >-          ", "-< 0xDA >-          ", "-< 0xDB >-          ",	
	"-< 0xDC >-          ", "-< 0xDD >-          ", "-< 0xDE >-          ", "-< 0xDF >-          ",	
	"-< 0xE0 >-          ", "-< 0xE1 >-          ", "-< 0xE2 >-          ", "-< 0xE3 >-          ",	
	"-< 0xE4 >-          ", "-< 0xE5 >-          ", "-< 0xE6 >-          ", "-< 0xE7 >-          ",	
	"-< 0xE8 >-          ", "-< 0xE8 >-          ", "-< 0xEA >-          ", "-< 0xEB >-          ",	
	"-< 0xEC >-          ", "-< 0xED >-          ", "-< 0xEE >-          ", "-< 0xEF >-          ",	

	"-- 0xF0 --          ", "-- 0xF1 --          ", "-- 0xF2 --          ", "-- 0xF3 --          ",	
	"-- 0xF4 --          ", "-- 0xF5 --          ", "-- 0xF6 --          ", "-- 0xF7 --          ",	
	"-- 0xF8 --          ", "-- 0xF8 --          ", "-- 0xFA --          ", "-- 0xFB --          ",	
	"-- 0xFC --          ", "-- 0xFD --          ", "[Pseudo-Handler]    ", "<Extended>          "	
};

static bool strni_equal (
				char const      *a,
				char const      *b,
				int             max)
{

	do {
		if (--max < 0) return true;
		if (tolower(*a) != tolower(*b)) return false;
		++a;
	}
	until (*b++ == '\0');

	return true;
}


static void format_opcode (char alter * buffer, unsigned char opcode) {
	if (opcode >= ' ' && opcode <= '~')
		sprintf (buffer, "0x%02X (%c)    %s", opcode, opcode, opcode_string[opcode]);
	else
		sprintf (buffer, "0x%02X        %s", opcode, opcode_string[opcode]);
}

void BBaseOpcodeHandler::log_opcode (BStreamFileToolkit & tk, unsigned int sequence, unsigned char opcode) alter {
	char                buffer[128];

	if (tk.m_log_line_length != 0)
		tk.LogEntry ("\n");

	if (tk.GetLoggingOptions (TK_Logging_Sequence)) {
		sprintf (buffer, "%6d: ", sequence);
		tk.LogEntry (buffer);
	}

	format_opcode (buffer, opcode);
	tk.LogEntry (buffer);
}


BBaseOpcodeHandler::~BBaseOpcodeHandler () {
	delete [] m_debug_string;

	if (m_ascii_buffer)
	{
		delete [] m_ascii_buffer;
		m_ascii_buffer = 0;
	}
}


TK_Status BBaseOpcodeHandler::Execute (BStreamFileToolkit & tk) alter {
	tk.ClearLastKey();
	//LOGI("BBaseOpcodeHandler::Execute");
	if (tk.GetReadFlags(TK_Flag_Unhandled_Opcodes)) {
		char            buffer[1024];
		char            opstring[64];

		format_opcode (opstring, Opcode());
		sprintf (buffer, "Execute function not implemented for opcode %s", opstring);
		return tk.Error (buffer);
	}
	else
		return TK_Normal;

}

TK_Status BBaseOpcodeHandler::Interpret (BStreamFileToolkit & tk, ID_Key key, int variant) alter {
	char            buffer[1024];
	char            opstring[1024];

	UNREFERENCED(key);
	UNREFERENCED(variant);
	format_opcode (opstring, Opcode());
	sprintf (buffer, "Interpret function not implemented for opcode %s", opstring);
	return tk.Error (buffer);
}

TK_Status BBaseOpcodeHandler::Interpret (BStreamFileToolkit & tk, ID_Key key, char const * name) alter {
	char            buffer[1024];
	char            opstring[1024];

	UNREFERENCED(key);
	UNREFERENCED(name);
	format_opcode (opstring, Opcode());
	sprintf (buffer, "Interpret(string) function not implemented for opcode %s", opstring);
	return tk.Error (buffer);
}


void BBaseOpcodeHandler::Reset () alter {
	m_stage = 0;
	m_progress = 0;
	m_needs_tag = false;

	m_debug_length = 0;
	if (m_debug_string != null)
		m_debug_string[0] = '\0';

	m_ascii_length = 0;
	if (m_ascii_buffer != null)
		m_ascii_buffer[0] = '\0';
	else {
		m_ascii_buffer = new char[TK_DEFAULT_BUFFER_SIZE];
		m_ascii_size = TK_DEFAULT_BUFFER_SIZE;
	}
	m_ascii_stage = 0;
	m_ascii_progress = 0;
	m_byte = 0;
	m_unsigned_short = 0;
	m_int = 0;
	m_char  = '\0';
} 

bool BBaseOpcodeHandler::Match_Instance (BStreamFileToolkit const & tk, Recorded_Instance alter * instance) alter {
	UNREFERENCED(tk);
	UNREFERENCED(instance);
	return false;
}


void BBaseOpcodeHandler::SetLoggingString (int length) alter {
	m_debug_length = length;
	if (m_debug_length >= m_debug_allocated) {
		delete [] m_debug_string;
		m_debug_string = new char [m_debug_allocated = m_debug_length + 16];
	}
	m_debug_string[m_debug_length] = '\0';
}

void BBaseOpcodeHandler::SetLoggingString (char const * string) alter {
	SetLoggingString ((int) strlen (string));
	strcpy (m_debug_string, string);
}

void BBaseOpcodeHandler::LogDebug (BStreamFileToolkit & tk, char const * string) alter {
	if (string != null)
		SetLoggingString (string);
	if (tk.GetLogging()) {
		if (m_debug_length > 0 && m_debug_string != null && m_debug_string[0])
			tk.LogEntry (m_debug_string);
	}
}

float BBaseOpcodeHandler::read_float (char const *cp, char const * alter * newcpp) {
	char const      *cps;
	double          result = 0.0;
	bool            negate = false;

	while (*cp == ' ') ++cp;

	if (*cp == '+' || *cp == '-') {
		if (*cp++ == '-')
			negate = true;
	}

	cps = cp;
	while ('0' <= *cp && *cp <= '9')
		result = result * 10.0 + (double)((int)*cp++ - '0');

	if (cp == cps && *cp != '.')    
		return 0.0;

	if (*cp == '.') {
		double          multiplier = 1.0;

		++cp;
		while ('0' <= *cp && *cp <= '9') {
			multiplier *= 0.1;
			result += multiplier * (double)((int)*cp++ - '0');
		}

		if (cp == cps + 1)          
			return 0.0;
	}

	if (negate)
		result = -result;

	if (*cp == 'e' || *cp == 'E') {
		int             exp = 0;

		++cp;
		negate = false;

		while (*cp == ' ') ++cp;

		if (*cp == '+' || *cp == '-') {
			if (*cp++ == '-')
				negate = true;
		}

		cps = cp;
		while ('0' <= *cp && *cp <= '9')
			exp = exp * 10 + ((int)*cp++ - '0');

		if (cp == cps)              
			return 0.0;

		if (exp != 0) {
			double          mantissa = negate ? 0.1 : 10.0;

			do {
				while ((exp & 1) == 0) {
					mantissa *= mantissa;
					exp >>= 1;
				}
				result *= mantissa;
			} while (--exp > 0);
		}
	}

	if (newcpp != null)             
		*newcpp = cp;

	return (float)result;
}



char * BBaseOpcodeHandler::write_float (char * buffer, double f) {
	char alter *    bp = buffer;
	double          fraclim = 0.0;
	double          scale;
	int             exponent = 0;
	int             digit;

	

	#define     MAX_FLOAT       1.0e38
	#define     F_DIG           6
	#define     F_RES           1000000.0

	
	if (f >= 0.0) {
		if (f < 1.0/MAX_FLOAT)
			f = 0.0;
		else if (f > MAX_FLOAT)
			f = MAX_FLOAT;
	}
	else {
		if (f > -1.0/MAX_FLOAT)
			f = 0.0;
		else {
			if (f < -MAX_FLOAT)
				f = -MAX_FLOAT;

			*bp++ = '-';                    
			f = -f;
		}
	}

	if (f > 0.0) {
		if (f >= F_RES) {                   
			do ++exponent;
			until ((f *= 0.1) < 10);
		}
		else if (0.01 > f && f > 0.0) {     
			do --exponent;
			until ((f *= 10) >= 1.0);
		}

		scale = F_RES/10;

		while (scale > f)
			scale *= 0.1;

		fraclim = (scale >= F_RES) ? 0.1 : scale * 10.0 / F_RES;

		f += fraclim * 0.5;     

		if (!(scale*10 > f))
			scale *= 10;        
	}
	else
		scale = 0.1;

	if (scale < 0.99 )
		*bp++ = '0';
	else do {
		digit = (int)(f / scale);
		*bp++ = (char)(digit + '0');
		f -= (float)digit * scale;
	} until ((scale *= 0.1) < 0.99 );

	if (f > fraclim) {
		*bp++ = '.';
		scale = 0.1;

		forever {
			digit = (int)(f / scale);
			*bp++ = (char)(digit + '0');
			f -= digit * scale;
			if (f <= fraclim)
				break;
			scale *= 0.1;
		}
	}

	if (exponent != 0) {
		int         place = 100;    

		*bp++ = 'e';
		if (exponent > 0)
			*bp++ = '+';
		else {
			*bp++ = '-';
			exponent = -exponent;
		}
		do {
			if (exponent >= place) {
				digit = exponent / place;
				*bp++ = (char)(digit + '0');
				exponent -= digit*place;
			}
			place /= 10;
		} while (exponent > 0);
	}

	*bp = '\0';

	return bp;
}





TK_Status TK_Default::Write (BStreamFileToolkit & tk) alter {
	return tk.Error();
}


TK_Status TK_Default::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
	unsigned char   opcode = '\0';
	unsigned int    sequence;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	if ((status = GetData (tk, opcode)) != TK_Normal)
		return status;

	sequence = tk.NextOpcodeSequence();
	if (tk.GetLogging())
		log_opcode (tk, sequence, opcode);

	tk.m_current_object = tk.m_objects[opcode];
	tk.m_current_object->Reset();
	if (opcode == TKE_Pause)
		++tk.m_pass;

	if (opcode == 'S')
	{
		//LOGI("Find shell");
		int a=0;
		a++;
	}
	
	return tk.m_current_object->Read(tk);
}

#ifndef BSTREAM_DISABLE_ASCII

TK_Status TK_Default::ReadAscii (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
	unsigned char   opcode_char = '\0';
	unsigned int    sequence;

opcode_start:
	

switch (m_stage) {
		case 0: {
			LookatData(tk, opcode_char);
			while(opcode_char == 13 || opcode_char == 10 || opcode_char == '\0' || opcode_char == 9) { 
				if ((status = GetData (tk, opcode_char)) != TK_Normal)
					return status;
				LookatData(tk, opcode_char);
			}
			m_opcode_buffer = new char[1024];
			m_stage++;
		}	nobreak;

		case 1: {
			if ((status = GetData (tk, opcode_char)) != TK_Normal)
				return status;

			if (opcode_char  == ';') {	
				sequence = tk.NextOpcodeSequence();
				if (tk.GetLogging())
					log_opcode (tk, sequence, opcode_char);

				tk.m_current_object = tk.m_objects[opcode_char];
				tk.m_current_object->Reset();
				if (opcode_char == TKE_Pause)
					++tk.m_pass;

				return tk.m_current_object->Read(tk);
			}
			if (opcode_char == 0)
				return TK_Complete;

			m_opcode_buffer[m_buffer_count++] = opcode_char;

			m_stage++;
		}	nobreak;

		case 2: {
			while (opcode_char != '>') {	
				if ((status = GetData (tk, opcode_char)) != TK_Normal)
					return status;
				
				m_opcode_buffer[m_buffer_count++] = opcode_char;
			}

			if (strni_equal("<HSX>", m_opcode_buffer,5)) {
				m_buffer_count = 0;
				m_stage = 0;
				delete [] m_opcode_buffer;
				goto opcode_start;
			}
			else if (strni_equal("</HSX>", m_opcode_buffer,6)) {
				delete [] m_opcode_buffer;
				m_buffer_count = 0;
				return TK_Complete;		
			}
			else if (!strni_equal("<TKE_", m_opcode_buffer,5)) {
				return tk.Error(" error, expected <OPCODE\n");
			}

			if (!(RemoveAngularBrackets(m_opcode_buffer)))
					status = TK_Error;

			size_t len = strlen(m_opcode_buffer);
			while (len != 24) {
				m_opcode_buffer[len] = ' ';
				len++;
			}
			m_opcode_buffer[len] = 0;

			m_stage = 0;
		}	break;

		default:
			return tk.Error();
	}

	int i;
	for (i=0; i<256; i++) {
		if (strni_equal((const char *)& m_opcode_buffer[4], (const char *)opcode_string[i],(m_buffer_count)) == true)
			break;	
	}
	unsigned char opcode = (unsigned char)i;

	if (m_opcode_buffer)
		delete[] m_opcode_buffer;
	m_buffer_count = 0;
	sequence = tk.NextOpcodeSequence();
	if (tk.GetLogging())
		log_opcode (tk, sequence, opcode_char);

	tk.m_current_object = tk.m_objects[opcode];
	tk.m_current_object->Reset();
	if (opcode == TKE_Pause)
		++tk.m_pass;

	return tk.m_current_object->Read(tk);
}
#endif 


TK_Status  BBaseOpcodeHandler::PutAsciiOpcode (BStreamFileToolkit & tk, int adjust,bool is_end, bool want_newline) alter {
#ifndef BSTREAM_DISABLE_ASCII
				
	TK_Status       status=TK_Normal;
	unsigned int    sequence;
	unsigned char	opcode = Opcode();
	char const*		pointer_opcodeString = opcode_string[opcode];
	int				n_tabs = tk.GetTabs();

	if (is_end) {
		switch (m_ascii_stage)  {
			case 0: {
				for (; m_ascii_progress <n_tabs; m_ascii_progress++) {
					if ((status = PutData (tk, "\t", 1)) != TK_Normal)	
						return status;
				}
				m_ascii_stage++;
			}	nobreak;
			
			case 1: {
				if ((status = PutData (tk, "</TKE_", 6)) != TK_Normal)	
					return status;
				m_ascii_stage++;
			}	nobreak;

			case 2: {
				
				int opcode_len = (int)strlen (pointer_opcodeString);
				char trimmed_opcode_str[4096];
				strcpy (trimmed_opcode_str,pointer_opcodeString);
				char *cp = trimmed_opcode_str + (opcode_len-1);
				while(*cp == ' ') 
					cp--;
				cp++; 
				*cp = '\0';

				if ((status = PutData (tk, trimmed_opcode_str, (int)strlen(trimmed_opcode_str))) != TK_Normal)	
					return status;

				m_ascii_stage++;
			}	nobreak;

			case 3: {
				if ((status = PutData (tk, ">\x0D\x0A",3)) != TK_Normal)	
					return status;
				m_ascii_stage = 0;
			}	break;
			
			
			 default:
				return tk.Error();	
		}
		 m_ascii_progress = 0;
	}
	else {
		switch (m_ascii_stage) {
			case 0: {
				for (; m_ascii_progress <n_tabs; m_ascii_progress++) {
					if ((status = PutData (tk, "\t", 1)) != TK_Normal)	
						return status;
				}
				m_ascii_stage++;
			}	nobreak;
		
			case 1: {
				if ((status = PutData (tk, "<TKE_", 5)) != TK_Normal)	
							return status;
				m_ascii_stage++;
			}	nobreak;

			case 2: {
				
				int opcode_len = (int)strlen (pointer_opcodeString);
				char trimmed_opcode_str[4096];
				strcpy (trimmed_opcode_str, pointer_opcodeString);
				char *cp = trimmed_opcode_str + (opcode_len-1);
				while (*cp == ' ')
					cp--;
				cp++; 
				*cp = '\0';

				if ((status = PutData (tk, trimmed_opcode_str, (int)strlen(trimmed_opcode_str))) != TK_Normal)	
					return status;

				m_ascii_stage++;
			}	nobreak;

			case 3: {
				if (opcode == ')'|| opcode == '\x00'|| opcode == '\x01'|| opcode == 'z' || opcode =='Z'|| opcode =='q') {
					if ((status = PutData (tk, "/>",2)) != TK_Normal)
						return status;
				}
				else {
					if ((status = PutData (tk, ">",1)) != TK_Normal)	
						return status;
				}
				m_ascii_stage++;
			}	nobreak;

			case 4: {		
				if (want_newline) {
					if ((status = PutData (tk, "\x0D\x0A",2)) != TK_Normal)	
						return status;
				}
				
				tk.adjust_written (adjust);

				m_ascii_stage = 0;
			} break;
		
			default:
				return tk.Error();	
		}
		m_ascii_progress = 0;
	}

	sequence = tk.NextOpcodeSequence();
	if (tk.GetLogging())
		log_opcode (tk, sequence, Opcode());
	return status;
#else
	UNREFERENCED (adjust);
	return tk.Error(stream_disable_ascii);
#endif 
}



TK_Status TK_Unavailable::Write (BStreamFileToolkit & tk) alter {
	return tk.Error();
}

TK_Status TK_Unavailable::Read (BStreamFileToolkit & tk) alter {
	char            buffer[1024];
	char            opstring[64];

	format_opcode (opstring, Opcode());
	sprintf (buffer, "unhandled opcode %s read", opstring);
	return tk.Error (buffer);
}



TK_Header::~TK_Header() {
	delete m_current_object;
}

TK_Status TK_Header::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if (m_current_object == null) {
				char        buffer[32];

				sprintf (buffer, "; HSF V%d.%02d ",
						 tk.GetTargetVersion() / 100,
						 tk.GetTargetVersion() % 100);
				m_current_object = new TK_Comment (buffer);
			}

			if ((status = m_current_object->Write (tk)) != TK_Normal)
				return status;
			delete m_current_object;
			m_current_object = null;

			m_stage++;
		}   nobreak;

		case 1: {
			if (m_current_object == null) {
				m_current_object = new TK_File_Info;
				m_current_object->Interpret (tk, -1);
			}

			if ((status = m_current_object->Write (tk)) != TK_Normal)
				return status;
			delete m_current_object;
			m_current_object = null;
			tk.m_header_comment_seen = true;

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif 
}

TK_Status TK_Header::Read (BStreamFileToolkit & tk) alter {
	return tk.Error();
}

void TK_Header::Reset () alter {
	delete m_current_object;
	m_current_object = 0;

	BBaseOpcodeHandler::Reset();
}



TK_Status TK_File_Info::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_flags)) != TK_Normal)
				return status;

			if (tk.GetLogging()) {
				char        buffer[32];

				sprintf (buffer, "%08X", m_flags);
				tk.LogEntry (buffer);
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_File_Info::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_flags)) != TK_Normal)
				return status;

			if (tk.GetVersion() < 105)      
				m_flags ^= TK_Generate_Dictionary;

			if (tk.GetLogging()) {
				char        buffer[32];

				sprintf (buffer, "%08X", m_flags);
				tk.LogEntry (buffer);
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

TK_Status TK_File_Info::Execute (BStreamFileToolkit & tk) alter {
	tk.SetWriteFlags (m_flags);
	////LOGI("TK_File_Info::Execute");
	return TK_Normal;
}

TK_Status TK_File_Info::Interpret (BStreamFileToolkit & tk, ID_Key key, int variant) alter {
	UNREFERENCED(key);
	UNREFERENCED(variant);
	m_flags = tk.GetWriteFlags();
	return TK_Normal;
}



TK_Comment::TK_Comment (char const * comment)
	: BBaseOpcodeHandler (TKE_Comment)
	, m_length (0)
	, m_comment (null) {
	if (comment != null) {
		m_length = (int) strlen (comment);
		m_comment = new char [m_length + 1];
		strcpy (m_comment, comment);
	}
}

TK_Comment::~TK_Comment() {
   delete [] m_comment;
}

TK_Status TK_Comment::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_comment, m_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_length == 0 || m_comment[m_length-1] != '\n')
				if ((status = PutData (tk, '\n')) != TK_Normal)
					return status;

			if (tk.GetLogging()) {
				char        buffer[256];

				buffer[0] = '\0';
				strncat (buffer, m_comment, 64);
				tk.LogEntry (buffer);
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Comment::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
	unsigned char   byte;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	if (m_stage != 0)
		return tk.Error();

	while (1) {
		if ((status = GetData (tk, byte)) != TK_Normal)
			return status;


		if (m_progress == m_length) {
			char *      old_buffer = m_comment;
			m_comment = new char [m_length + 32];
			memcpy (m_comment, old_buffer, m_length);
			m_length += 32;         
			delete [] old_buffer;
		}

		if (byte == '\n') {
			m_comment[m_progress++] = '\0';
			break;
		}

		m_comment[m_progress++] = byte;
	}

	m_length = m_progress;

	if (tk.GetLogging()) {
		char        buffer[256];

		buffer[0] = '\0';
		strncat (buffer, m_comment, 64);
		tk.LogEntry (buffer);
	}

	m_stage = -1;

	return status;
}

TK_Status TK_Comment::Execute (BStreamFileToolkit & tk) alter {

	//LOGI("TK_Comment::Execute");
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ExecuteAscii (tk);
#endif


	if (!tk.m_header_comment_seen) {
		tk.m_header_comment_seen = true;

//		if (strncmp (m_comment, "; IVP V", 7))
//			return tk.Error ("file does not appear to be HSF format");

		if (strncmp (m_comment, "; IVP V", 7)&&strncmp (m_comment, "; HSF V", 7))
			return tk.Error ("file does not appear to be HSF format");

		char const *    cp = &m_comment[7];
		int             version = 0;

		while (cp) {
			if ('0' <= *cp && *cp <= '9')
				version = 10 * version  +  (int)(*cp++ - '0');
			else if (*cp == '.')
				++cp;       
			else if (*cp == ' ')
				cp = null;  
			else
				return tk.Error ("error reading version number");   
		}

		tk.m_file_version = version;    

		if (!tk.GetReadFlags(TK_Ignore_Version)) {
			if (version > TK_File_Format_Version) {
				
				return TK_Version;
			}
		}
	}
	else {
		
	}

	return TK_Normal;
}

void TK_Comment::Reset () alter {
	delete [] m_comment;
	m_comment = null;
	m_length = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_Comment::set_comment (int length) alter {
	delete [] m_comment;
	m_length = length;
	m_comment = new char [m_length + 1];
	m_comment[m_length] = '\0';
}

void TK_Comment::set_comment (char const * comment) alter {
	set_comment ((int) strlen (comment));
	strcpy (m_comment, comment);
}



TK_Status TK_Terminator::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 

		return WriteAscii (tk);
#endif

	status = PutOpcode (tk, 0);

	if (status == TK_Normal && Opcode() == TKE_Pause)
		tk.RecordPause (tk.GeneratedSoFar());

	if (tk.GetLogging())
		tk.LogEntry ("\n");

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Terminator::Read (BStreamFileToolkit & tk) alter {
	UNREFERENCED(tk);

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	if (tk.GetLogging())
		tk.LogEntry ("\n");

	if (Opcode() == TKE_Termination)
		return TK_Complete;
	else
		return TK_Pause;
}

TK_Status TK_Terminator::Execute (BStreamFileToolkit & tk) alter {
	//LOGI("TK_Terminator::Execute");
	if (Opcode() == TKE_Termination)    
		return tk.Error();
	else
		return TK_Normal;
}



TK_Status TK_Compression::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk, 0)) != TK_Normal)
				return status;

			m_stage++;
		}   nobreak;

		case 1: {
			if (Opcode() == TKE_Start_Compression)
				status = tk.start_compression();
			else
				status = tk.stop_compression();

			if (status != TK_Normal)
				return status;

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return TK_Normal;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Compression::Read (BStreamFileToolkit & tk) alter {
	UNREFERENCED(tk);
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	return TK_Normal;
}

TK_Status TK_Compression::Execute (BStreamFileToolkit & tk) alter {
	////LOGI("TK_Compression::Execute");
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ExecuteAscii (tk);
#endif

	if (Opcode() == TKE_Start_Compression)
		status = tk.start_decompression();
	else
		status = tk.stop_decompression();

	if (status != TK_Normal)
		return status;

	return TK_Revisit;      

}

TK_Status TK_Compression::Interpret (BStreamFileToolkit & tk, ID_Key key, int variant) alter {
	UNREFERENCED(tk);
	UNREFERENCED(key);
	UNREFERENCED(variant);
	return TK_Normal;
}



TK_Open_Segment::~TK_Open_Segment() {
	delete [] m_string;
}



TK_Status TK_Open_Segment::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char byte = 0;

			if (m_length >= 255)
				byte = 255;
			else
				byte = (unsigned char)m_length;

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;

			m_stage++;
		}   nobreak;

		case 2: {
			if (m_length >= 255) {
				if ((status = PutData (tk, m_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_string, m_length)) != TK_Normal)
				return status;

			if (tk.GetLogging() &&tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
				LogDebug (tk, "(");
				LogDebug (tk, m_string);
				LogDebug (tk, ")");
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Open_Segment::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			unsigned char byte = 0;
			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;

			m_length = byte;

			m_stage++;
		}   nobreak;

		case 1: {
			if (m_length == 255) {
				int length = 0;
				if ((status = GetData(tk, length)) != TK_Normal)
					return status;
				m_length = length;
			}
			
			set_segment (m_length);    

			m_stage++;
		} nobreak;

		case 2: {
			if ((status = GetData (tk, m_string, m_length)) != TK_Normal)
				return status;

			if (tk.GetLogging() &&tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
				LogDebug (tk, "(");
				LogDebug (tk, m_string);
				LogDebug (tk, ")");
			}
			increase_nesting(tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Open_Segment::set_segment (int length) alter {
	m_length = length;
	if (m_length >= m_allocated) {
		delete [] m_string;
		m_string = new char [m_allocated = m_length + 16];
	}
	m_string[m_length] = '\0';
}

void TK_Open_Segment::set_segment (char const * segment) alter {
	set_segment ((int) strlen (segment));
	strcpy (m_string, segment);
}

void TK_Open_Segment::Reset () alter {
	m_length = 0;

	BBaseOpcodeHandler::Reset();
}



TK_Status TK_Close_Segment::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if ((status = PutOpcode (tk)) != TK_Normal)
		return status;

	if (Tagging (tk))
		status = Tag (tk, 0);

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Close_Segment::Read (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	decrease_nesting(tk);
	return TK_Normal;       
}




TK_Status TK_Reopen_Segment::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_index)) != TK_Normal)
				return status;

			if (tk.GetLogging() &&tk.GetLoggingOptions (TK_Logging_Tagging)) {
				char        buffer[32];

				sprintf (buffer, "[%d]", m_index);
				LogDebug (tk, buffer);
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Reopen_Segment::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_index)) != TK_Normal)
				return status;

			if (tk.GetLogging() &&tk.GetLoggingOptions (TK_Logging_Tagging)) {
				char        buffer[32];

				sprintf (buffer, "[%d]", m_index);
				LogDebug (tk, buffer);
			}

			increase_nesting(tk);
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Status TK_Renumber::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif


	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk, 0)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			int         value = (int)m_key;

			if ((status = PutData (tk, value)) != TK_Normal)
				return status;

			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Tagging)) {
				char	buffer[32];

				sprintf (buffer, "%d", value);
				LogDebug (tk, buffer);
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Renumber::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
	int             value;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	if (m_stage != 0)
		return tk.Error();

	if ((status = GetData (tk, value)) != TK_Normal)
		return status;

	if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Tagging)) {
		char	buffer[32];

		sprintf (buffer, "%d", value);
		LogDebug (tk, buffer);
	}

	m_key = value;
	m_stage = -1;

	return status;
}



TK_Status TK_Geometry_Attributes::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk, 0)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Geometry_Attributes::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

	if (m_stage != 0)
		return tk.Error();

	m_stage = -1;

	return status;
}

TK_Status TK_Geometry_Attributes::Execute (BStreamFileToolkit & tk) alter {
	////LOGI("TK_Geometry_Attributes::Execute");
	tk.OpenGeometry();
	return TK_Revisit;
}



TK_Status TK_Tag::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk, 0)) != TK_Normal)
				return status;

			LogDebug (tk);
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Tag::Read (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	
	return TK_Normal;
}

TK_Status TK_Tag::Execute (BStreamFileToolkit & tk) alter {
	////LOGI("TK_Tag::Execute");
	TK_Status       status = TK_Normal;     
	int             i;

	switch (Opcode()) {
		case TKE_Tag: {
			int             index;
			char            buffer[32];
			bool            logging = tk.GetLogging() &&
												  tk.GetLoggingOptions (TK_Logging_Tagging);

			if (tk.m_last_keys_used == 0) {
				index = tk.NextTagIndex();

				tk.AddIndexKeyPair (index, -1);

				if (logging) {
					sprintf (buffer, "[%d]", index);
					SetLoggingString (buffer);
				}
			}
			else {
				if (logging) {
					SetLoggingString (10 * tk.m_last_keys_used);
					GetLoggingString()[0] = '\0';
				}
				for (i = 0; i < tk.m_last_keys_used; i++) {
					index = tk.NextTagIndex();

					tk.AddIndexKeyPair (index, tk.m_last_keys[i]);

					if (logging) {
						sprintf (buffer, "[%d]", index);
						strcat (GetLoggingString(), buffer);
						if (i%10 == 9)
							strcat (GetLoggingString(), "\n\t\t\t\t");
					}
				}
			}

			LogDebug (tk);
		}   break;

		default: {
			status = tk.Error("internal error in TK_Tag::Execute -- unrecognized opcode");
		}   break;
	}

	return status;
}

TK_Status TK_Tag::Interpret (BStreamFileToolkit & tk, ID_Key key, int variant) alter {
	UNREFERENCED(key);
	UNREFERENCED(variant);
	return tk.Error("TK_Tag::Interpret should never be possible");
}



TK_Status TK_Dictionary::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			m_format = (unsigned char)tk.GetDictionaryFormat();
			if (m_format == 3) {
				if (tk.GetTargetVersion() < 1155 || tk.GetWriteFlags (TK_Disable_Global_Compression))
					m_format = 2;
			}
			if (tk.GetTargetVersion() < 710)
				m_format = 0;

			if ((status = PutData (tk, m_format)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_format >= 2) {
				unsigned short          pauses = (unsigned short)tk.GetPauseCount();

				if ((status = PutData (tk, pauses)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if (m_format >= 2) {
				if ((status = PutData (tk, tk.GetPauseTable(), tk.GetPauseCount())) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if (m_format == 3)
				if ((status = tk.start_compression()) != TK_Normal)
					return status;

			m_stage++;
		}   nobreak;

		case 5: {
			int             item_count = 0;
			int             i;

			for (i=0; i<tk.m_translator.m_size; ++i) {
				if (tk.m_translator.m_pairs[i].m_index == i)
					++item_count;
			}
			if ((status = PutData (tk, item_count)) != TK_Normal)
				return status;

			m_placeholder = 0;  
			m_progress = -1;    
			m_stage++;
		}   nobreak;

		case 6: {
			
			while (m_placeholder < tk.m_translator.m_size) {
				Internal_Translator::Index_Key_Pair const &     item = tk.m_translator.m_pairs[m_placeholder];

				if (item.m_index == m_placeholder) {    
					switch (m_progress) {
						case -1: {
							if ((status = PutData (tk, m_placeholder)) != TK_Normal)
								return status;
							m_progress--;
						}   nobreak;

						case -2: {
							unsigned char       present = 0;
							int                 i;

							if (item.m_extra != null) {
								for (i=0; i<8; ++i)
									if (item.m_extra->m_variants[i][0] != 0)
										present |= 1 << i;
							}
							if ((status = PutData (tk, present)) != TK_Normal)
								return status;
							m_progress = 0;
						}   nobreak;

						default: {
							int         num_offsets = m_format == 0 ? 1 : 2;

							if (m_progress < 0 || m_progress > 7)
								return tk.Error();

							if (item.m_extra != null) {
								while (m_progress < 8) {
									if (item.m_extra->m_variants[m_progress][0] != 0) {
										if ((status = PutData (tk, item.m_extra->m_variants[m_progress],
																   num_offsets)) != TK_Normal)
											return status;
									}
									++m_progress;
								}
							}
							m_progress = -3;
						}   nobreak;

						case -3: {
							if (m_format > 0) {
								unsigned char       byte = 0;
                                
								if (item.m_extra != null)
									byte = (unsigned char)(item.m_extra->m_options & tk.m_dictionary_options);

								if ((status = PutData (tk, byte)) != TK_Normal)
									return status;
							}
							m_progress--;
						}   nobreak;

						case -4: {
							if (m_format > 0 && item.m_extra != null &&
								(item.m_extra->m_options & tk.m_dictionary_options & Internal_Translator::Bounds_Valid) != 0) {
								if ((status = PutData (tk, item.m_extra->m_bounds, 6)) != TK_Normal)
									return status;
							}
							m_progress = 666;
						}   break;
					}
				}

				++m_placeholder;
				m_progress = -1;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if (m_format == 3)
				if ((status = tk.stop_compression()) != TK_Normal)
					return status;

			m_stage++;
		}   nobreak;

		case 8: {
			m_format = (unsigned char)tk.GetDictionaryFormat();
			if (m_format == 1) {
				int         offset = tk.GetFirstPause();

				if ((status = PutData (tk, offset)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 9: {
			
			int         offset = tk.GetDictionaryOffset();

			if ((status = PutData (tk, offset)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Dictionary::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_format)) != TK_Normal)
				return status;
			if (m_format > 3)
				return tk.Error();
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_format >= 2) {
				unsigned short          pauses;

				if ((status = GetData (tk, pauses)) != TK_Normal)
					return status;
				m_progress = pauses;    
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_format >= 2) {
				int *       table = new int [m_progress];
				int         i;

				if ((status = GetData (tk, table, m_progress)) != TK_Normal) {
					delete [] table;
					return status;
				}

				tk.ClearPauses();
				for (i=0; i<m_progress; i++)
					tk.RecordPause(table[i]);
				delete [] table;
			}
			m_stage++;
			m_stage++;
		}   nobreak;

		case 3: {
			if (m_format == 3)
				if ((status = tk.start_decompression()) != TK_Normal)
					return status;

		}   nobreak;

		case 4: {
			if ((status = GetData (tk, m_number_of_items)) != TK_Normal)
				return status;
			m_placeholder = 0;  
			m_progress = -1;    
			m_stage++;
		}   nobreak;

		case 5: {
			
			while (m_placeholder < m_number_of_items) {
				switch (m_progress) {
					case -1: {
						int         index;

						if ((status = GetData (tk, index)) != TK_Normal)
							return status;

						index += tk.m_index_base;   
						m_item = &tk.m_translator.m_pairs[index];
						m_progress--;
					}   nobreak;

					case -2: {
						if ((status = GetData (tk, m_present)) != TK_Normal)
							return status;
						m_progress = 0;
					}   nobreak;

					default: {
						int         num_offsets = m_format == 0 ? 1 : 2;

						if (m_progress < 0 || m_progress > 7)
							return tk.Error();

						while (m_progress < 8) {
							if ((m_present & (1 << m_progress)) != 0) {
								int         value[2];

								value[1] = 0;
								if ((status = GetData (tk, value, num_offsets)) != TK_Normal)
									return status;
								
								tk.AddVariant (m_item->m_key, m_progress, value[0], value[1]);
							}
							++m_progress;
						}
						m_progress = -3;
					}   nobreak;

					case -3: {
						if (m_format > 0) {
							unsigned char       byte;

							if ((status = GetData (tk, byte)) != TK_Normal)
								return status;

							if (byte != 0) {
								if (m_item->m_extra == null) {
									m_item->m_extra = new IT_Index_Key_Extra;
									memset (m_item->m_extra, 0, sizeof (IT_Index_Key_Extra));
								}

								m_item->m_extra->m_options = byte;
							}
						}

						m_progress--;
					}   nobreak;

					case -4: {
						if (m_format > 0) {
							if (m_item->m_extra != null &&
								(m_item->m_extra->m_options & Internal_Translator::Bounds_Valid) != 0) {
								if ((status = GetData (tk, m_item->m_extra->m_bounds, 6)) != TK_Normal)
									return status;
							}
						}

						m_item = null;
						m_progress = 666;
					}   break;
				}

				++m_placeholder;
				m_progress = -1;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if (m_format == 3)
				if ((status = tk.stop_decompression()) != TK_Normal)
					return status;

			m_stage++;
		}   nobreak;

		case 7: {
			if (m_format == 1) {
				int         offset;

				if ((status = GetData (tk, offset)) != TK_Normal)
					return status;

				tk.SetFirstPause (offset);
			}
			m_stage++;
		}   nobreak;

		case 8: {
			
			int             offset;

			if ((status = GetData (tk, offset)) != TK_Normal)
				return status;

			tk.SetDictionaryOffset (offset);
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	status = TK_Normal;
	m_stage = -1;

	return status;
}


TK_Status TK_Dictionary::Execute (BStreamFileToolkit & tk) alter {
	////LOGI("TK_Dictionary::Execute");
	UNREFERENCED(tk);
	return TK_Normal;
}

TK_Status TK_Dictionary::Interpret (BStreamFileToolkit & tk, ID_Key key, int variant) alter {
	
	UNREFERENCED(tk);
	UNREFERENCED(key);
	UNREFERENCED(variant);
	return TK_Normal;
}

void TK_Dictionary::Reset () alter {
	BBaseOpcodeHandler::Reset();
}



TK_Status TK_Dictionary_Locater::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_size)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_offset)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Dictionary_Locater::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
    
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_size)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_offset)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}


TK_Status TK_Dictionary_Locater::Execute (BStreamFileToolkit & tk) alter {
	////LOGI("TK_Dictionary_Locater::Execute");
	tk.SetDictionaryOffset (m_offset);
	tk.SetDictionarySize (m_size);

	return TK_Normal;
}

TK_Status TK_Dictionary_Locater::Interpret (BStreamFileToolkit & tk, ID_Key key, int variant) alter {
	
	UNREFERENCED(key); UNREFERENCED(variant);

	SetOffset (tk.GetDictionaryOffset());
	SetSize (tk.GetDictionarySize());

	return TK_Normal;
}

void TK_Dictionary_Locater::Reset () alter {
	BBaseOpcodeHandler::Reset();
}



TK_Referenced_Segment::TK_Referenced_Segment (unsigned char opcode)
	: BBaseOpcodeHandler (opcode)
	, m_length (0)
	, m_allocated (0)
	, m_string (null)
	, m_cond_length (0)
	, m_cond_allocated (0)
	, m_condition (null)
	, m_key (-1)
	, m_referee (null)
	, m_follow (true) {
}

TK_Referenced_Segment::~TK_Referenced_Segment() {
	delete [] m_string;
	delete [] m_condition;
	delete m_referee;
}

TK_Status TK_Referenced_Segment::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte = 0;

			if (tk.GetTargetVersion() >= 1150 && m_cond_length > 0)
				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;

			m_stage++;
		}   nobreak;

		case 2: {
			unsigned char       byte = 0;
			
			if (m_length >= 255)
				byte = 255;
			else
				byte = (unsigned char)m_length;

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;

			m_stage++;
		}   nobreak;

		case 3: {
			if (m_length >= 255) {
				if ((status = PutData (tk, m_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}	nobreak;

		case 4: {
			 if ((status = PutData (tk, m_string, m_length)) != TK_Normal)
				return status;

			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
				LogDebug (tk, "<");
				LogDebug (tk, m_string);
				LogDebug (tk, ">");
			}

			m_stage++;
		}   nobreak;

		case 5: {
			if (tk.GetTargetVersion() >= 1150 && m_cond_length > 0) {
				unsigned char       byte = 0;

				if (m_cond_length >= 255)
					byte = 255;
				else
					byte = (unsigned char)m_cond_length;

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}

			m_stage++;
		}   nobreak;

		case 6: {
			if (m_cond_length >= 255) {
				if ((status = PutData (tk, m_cond_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}	nobreak;

		case 7: {
			if (tk.GetTargetVersion() >= 1150 && m_cond_length > 0) {
				 if ((status = PutData (tk, m_condition, m_cond_length)) != TK_Normal)
					return status;

				if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
					LogDebug (tk, "<");
					LogDebug (tk, m_condition);
					LogDebug (tk, ">");
				}
			}

			m_stage++;
		}   nobreak;

		case 8: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Referenced_Segment::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
	unsigned char   byte;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_progress = (int)byte;

			m_stage++;
		}   nobreak;

		case 1: {
			if (m_progress != 0)
				set_segment (m_progress);   
			else {
				
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_length = byte;
			}

			m_stage++;
		}   nobreak;

		case 2: {
			if (m_length == 255) {
				int		length = 0;

				if ((status = GetData (tk, length)) != TK_Normal)
					return status;
				m_length = length;
			}

			set_segment (m_length);    

			m_stage++;
		}	nobreak;

		case 3: {
			if ((status = GetData (tk, m_string, m_length)) != TK_Normal)
				return status;

			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
				LogDebug (tk, "<");
				LogDebug (tk, m_string);
				LogDebug (tk, ">");
			}

			m_stage++;
		}   nobreak;

		case 4: {
			if (m_progress == 0) {
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_cond_length = byte;
			}

			m_stage++;
		}   nobreak;

		case 5: {
			if (m_progress == 0) {
				if (m_cond_length == 255) {
					int length = 0;
					if ((status = GetData (tk, length)) != TK_Normal)
						return status;
					m_cond_length = length;
				}
				SetCondition (m_cond_length);    
			}
			else
				SetCondition (0);

			m_stage++;
		}   nobreak;

		case 6: {
			if (m_progress == 0) {
				if ((status = GetData (tk, m_condition, m_cond_length)) != TK_Normal)
					return status;

				if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
					LogDebug (tk, "<");
					LogDebug (tk, m_condition);
					LogDebug (tk, ">");
				}
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Referenced_Segment::set_segment (int length) alter {
	m_length = length;
	if (m_length >= m_allocated) {
		delete [] m_string;
		m_string = new char [m_allocated = m_length + 16];
	}
	m_string[m_length] = '\0';
}

void TK_Referenced_Segment::set_segment (char const * segment) alter {
	set_segment ((int) strlen (segment));
	strcpy (m_string, segment);
}


void TK_Referenced_Segment::SetCondition (int length) alter {
	m_cond_length = length;
	if (m_cond_length >= m_cond_allocated) {
		delete [] m_condition;
		m_condition = new char [m_cond_allocated = m_cond_length + 16];
	}
	m_condition[m_cond_length] = '\0';
}

void TK_Referenced_Segment::SetCondition (char const * condition) alter {
	SetCondition ((int) strlen (condition));
	strcpy (m_condition, condition);
}


void TK_Referenced_Segment::Reset () alter {
	m_length = 0;
	m_cond_length = 0;

	delete m_referee;
	m_referee = null;

	BBaseOpcodeHandler::Reset();
}



TK_Reference::TK_Reference ()
	: BBaseOpcodeHandler (TKE_Reference)
	, m_index (0)
	, m_cond_length (0)
	, m_cond_allocated (0)
	, m_condition (null)
	, m_this_key (-1)
	, m_key (-1)
	, m_referee (null)
	, m_follow (true) {
}

TK_Reference::~TK_Reference() {
	delete [] m_condition;
	delete m_referee;
}

TK_Status TK_Reference::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

	if (tk.GetTargetVersion() < 1520)
		return TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;

			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_index)) != TK_Normal)
				return status;

			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Tagging)) {
				char		buffer[64];

				sprintf (buffer, "[%d]", m_index);
				LogDebug (tk, buffer);
			}

			m_stage++;
		}   nobreak;

		case 2: {
			unsigned char       byte = 0;

			if (m_cond_length >= 255)
				byte = 255;
			else
				byte = m_cond_length;

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;

			m_stage++;
		}   nobreak;

		case 3: {
			if (m_cond_length >= 255) {
				if ((status = PutData (tk, m_cond_length)) != TK_Normal)
					return status;
			}

			m_stage++;
		}   nobreak;

		case 4: {
			if (m_cond_length > 0) {
				 if ((status = PutData (tk, m_condition, m_cond_length)) != TK_Normal)
					return status;

				if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
					LogDebug (tk, "<");
					LogDebug (tk, m_condition);
					LogDebug (tk, ">");
				}
			}

			m_stage++;
		}   nobreak;

		case 5: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Reference::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
	unsigned char   byte;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_index)) != TK_Normal)
				return status;

			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Tagging)) {
				char		buffer[64];

				sprintf (buffer, "[%d]", m_index);
				LogDebug (tk, buffer);
			}

			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_cond_length = byte;

			m_stage++;
		}   nobreak;

		case 2: {
			if (m_cond_length == 255) {
				int length = 0;
				if ((status = GetData (tk, length)) != TK_Normal)
					return status;
				m_cond_length = length;
			}
			SetCondition (m_cond_length);    

			m_stage++;
		}   nobreak;

		case 3: {
			if (m_cond_length != 0) {
				if ((status = GetData (tk, m_condition, m_cond_length)) != TK_Normal)
					return status;

				if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
					LogDebug (tk, "<");
					LogDebug (tk, m_condition);
					LogDebug (tk, ">");
				}
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Reference::SetCondition (int length) alter {
	m_cond_length = length;
	if (m_cond_length >= m_cond_allocated) {
		delete [] m_condition;
		m_condition = new char [m_cond_allocated = m_cond_length + 16];
	}
	m_condition[m_cond_length] = '\0';
}

void TK_Reference::SetCondition (char const * condition) alter {
	SetCondition ((int) strlen (condition));
	strcpy (m_condition, condition);
}


void TK_Reference::Reset () alter {
	m_index = 0;
	m_cond_length = 0;

	delete m_referee;
	m_referee = null;

	BBaseOpcodeHandler::Reset();
}



TK_Instance::TK_Instance (int from_index, int from_variant, int to_index, int to_variant,
						   int options, float const * xform)
	: BBaseOpcodeHandler (TKE_Repeat_Object)
	, m_from_index (from_index)
	, m_from_variant (from_variant)
	, m_to_index (to_index)
	, m_to_variant (to_variant)
	, m_options (options) {
	int     i;

	if (xform != null) {
		for (i=0; i<16; i++)
			m_matrix[i]  = xform[i];
	}
	else
		for (i=0; i<16; i++)
			m_matrix[i] = 0;
}

TK_Status TK_Instance::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_from_index)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			 if ((status = PutData (tk, m_from_variant)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_to_index)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			 if ((status = PutData (tk, m_to_variant)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			unsigned char       byte = (unsigned char)m_options;
			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 6: {
			 if ((status = PutData (tk, &m_matrix[0], 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 7: {
			 if ((status = PutData (tk, &m_matrix[4], 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 8: {
			 if ((status = PutData (tk, &m_matrix[8], 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 9: {
			 if ((status = PutData (tk, &m_matrix[12], 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 10: {
#if 0
			
			
			
			if ((status = Tag (tk, m_to_variant)) != TK_Normal)
				return status;
#else
			int                 i;
			int                 index;
			ID_Key              key;
			for (i = 0; i < tk.m_last_keys_used; i++) { 
				key = tk.m_last_keys[i];
				status = tk.KeyToIndex (key, index);
				if (status == TK_NotFound) {
					BBaseOpcodeHandler *    tagop = tk.GetOpcodeHandler (TKE_Tag);
					if ((status = tagop->Write (tk)) != TK_Normal)
						return status;
					tagop->Reset();
					
					tk.AddIndexKeyPair (index = tk.NextTagIndex(), key);
				}
				if (m_to_variant != -1) {
					
					tk.AddVariant (key, m_to_variant, tk.GetPosition() + tk.GetFileOffset());
				}
			}
#endif
			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Tagging)) {
				char            buffer[64];

				sprintf (buffer, "[%d:%d - %d:%d]", m_from_index, m_from_variant, m_to_index, m_to_variant);
				tk.LogEntry (buffer);
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Instance::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_from_index)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_from_variant)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_to_index)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = GetData (tk, m_to_variant)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			unsigned char       byte;
			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_options = byte;
			m_stage++;
		}   nobreak;

		case 5: {
			if ((status = GetData (tk, &m_matrix[0], 3)) != TK_Normal)
				return status;
			m_matrix[3] = 0.0f;
			m_stage++;
		}   nobreak;

		case 6: {
			if ((status = GetData (tk, &m_matrix[4], 3)) != TK_Normal)
				return status;
			m_matrix[7] = 0.0f;
			m_stage++;
		}   nobreak;

		case 7: {
			if ((status = GetData (tk, &m_matrix[8], 3)) != TK_Normal)
				return status;
			m_matrix[11] = 0.0f;
			m_stage++;
		}   nobreak;

		case 8: {
			if ((status = GetData (tk, &m_matrix[12], 3)) != TK_Normal)
				return status;
			m_matrix[15] = 1.0f;

			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Tagging)) {
				char            buffer[64];

				sprintf (buffer, "[%d:%d - %d:%d]", m_from_index, m_from_variant, m_to_index, m_to_variant);
				tk.LogEntry (buffer);
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Instance::Reset () alter {
	int i;

	m_from_index = m_from_variant = m_to_index = m_to_variant = m_options = 0;
	for (i = 0; i < 16; i++)
		m_matrix[i] = 0;
	BBaseOpcodeHandler::Reset();
}



TK_Status TK_Delete_Object::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_index)) != TK_Normal)
				return status;

			if (tk.GetLogging() &&tk.GetLoggingOptions (TK_Logging_Tagging)) {
				char        buffer[32];

				sprintf (buffer, "[%d]", m_index);
				LogDebug (tk, buffer);
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Delete_Object::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_index)) != TK_Normal)
				return status;

			if (tk.GetLogging() &&tk.GetLoggingOptions (TK_Logging_Tagging)) {
				char        buffer[32];

				sprintf (buffer, "[%d]", m_index);
				LogDebug (tk, buffer);
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Status TK_Bounding::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

	if (!m_is_valid)
		return status;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_type)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			bool is_sphere = (m_type == TKO_Bounding_Type_Sphere);
			if ((status = PutData (tk, m_values, is_sphere ? 4 : 6)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if (Opcode() == TKE_Bounding_Info) {
				if (m_type == TKO_Bounding_Type_Sphere)
					tk.SetWorldBoundingBySphere(m_values, m_values[3]);
				else
					tk.SetWorldBounding(m_values);
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error("internal error in TK_Bounding::Write");
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Bounding::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_type)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			bool is_sphere = (m_type == TKO_Bounding_Type_Sphere);
			if ((status = GetData (tk, m_values, is_sphere ? 4 : 6)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (Opcode() == TKE_Bounding_Info) {
				if (m_type == TKO_Bounding_Type_Sphere)
					tk.SetWorldBoundingBySphere(m_values, m_values[3]);
				else
					tk.SetWorldBounding(m_values);
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error("internal error in TK_Bounding::Read");
	}

	return status;
}



TK_Status TK_Streaming::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
  #if 0   
	if (m_flag)
		return PutOpcode (tk);
	else
  #else
	UNREFERENCED(tk);
  #endif
		return TK_Normal;       
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Streaming::Read (BStreamFileToolkit & tk) alter {
	UNREFERENCED(tk);
	return TK_Pause;
}




TK_LOD::~TK_LOD() 
{
	Reset();
}



TK_Status TK_LOD::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;
	TK_Terminator   terminate(TKE_Termination);

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif


	if (m_primitives == null)
		return TK_Normal;

	if (tk.GetTargetVersion() < 907)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		} nobreak;

		case 1: {
			while (m_substage <= m_highest_level) {
				if (m_num_primitives[m_substage]) {
					if (m_progress == 0) {
						unsigned char byte = (unsigned char)m_substage; 
						if ((status = PutData (tk, byte)) != TK_Normal)
							return status;
						m_progress++;
					}
					while (m_progress < m_num_primitives[m_substage] + 1) {
						if ((status = m_primitives[m_substage][m_progress-1]->Write(tk)) != TK_Normal)
							return status;
						m_progress++;
					}
					if (m_progress == m_num_primitives[m_substage] + 1) {
						if ((status = terminate.Write(tk)) != TK_Normal)
							return status;
						m_progress++;
					}
					m_progress = 0;
				}
				m_substage++;
			}
			m_substage = 0;
			m_stage++;
		} nobreak;

		case 2: {
			unsigned char byte = TKLOD_ESCAPE;
			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage = 0;
		} break;

		default:
			return tk.Error("unrecognized case in TK_LOD");

	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif

}


TK_Status TK_LOD::ReadOneList (BStreamFileToolkit & tk) alter {
	TK_Status status = TK_Normal;
	BBaseOpcodeHandler *handler = NULL;
	unsigned char   opcode = '\0';

	for (;;) {
		switch(m_substage) {
			case 0: {
				if ((status = GetData (tk, opcode)) != TK_Normal)
					return status;
				if (opcode == TKE_Termination) {
					return TK_Normal;
				}
				status = tk.GetOpcodeHandler(opcode)->Clone(tk, &handler);
				if (status != TK_Normal)
					return tk.Error("BBaseOpcodeHandler clone failed during TK_LOD::Read");
				//vlist_add_first(m_current_working, handler);
				m_substage++;
			} nobreak;

			case 1: {
				
				//handler = (BBaseOpcodeHandler *)vlist_peek_first(m_current_working);
				if ((status = handler->Read(tk)) != TK_Normal)
					return status;
				m_substage = 0;
			} break;

			default:
				return tk.Error("unrecognized case of m_substage during TK_LOD::Read");
		}
	}
}



TK_Status TK_LOD::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
	unsigned char byte;
	int j, count;
	BBaseOpcodeHandler *handler;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	if (m_current_working == null)
		m_current_working = new_vlist(malloc, free);
    
	forever {
		switch(m_stage) {
			case 0: {
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_current_level = byte;
				if (m_current_level == TKLOD_ESCAPE) {
					return TK_Normal;
				}
				if (m_current_level >= m_levels_allocated) {
					int old_levels_allocated;
					BBaseOpcodeHandler ***temp_primitives;
					int *temp_num_primitives;

					old_levels_allocated = m_levels_allocated;
					m_levels_allocated += 10;
					temp_primitives = new BBaseOpcodeHandler **[m_levels_allocated];
					temp_num_primitives = new int[m_levels_allocated];
					if (!temp_primitives || !temp_num_primitives)
						return tk.Error("memory allocation failed in TK_LOD::Read");
					if (old_levels_allocated) {
						memcpy(temp_primitives, m_primitives, old_levels_allocated * sizeof(BBaseOpcodeHandler **));
						memcpy(temp_num_primitives, m_num_primitives, old_levels_allocated * sizeof(int));
						delete [] m_primitives;
						delete [] m_num_primitives;
					}
					memset(&temp_primitives[old_levels_allocated], 0, (m_levels_allocated - old_levels_allocated) * sizeof(BBaseOpcodeHandler **));
					memset(&temp_num_primitives[old_levels_allocated], 0, (m_levels_allocated - old_levels_allocated) * sizeof(int));
					m_primitives = temp_primitives;
					m_num_primitives = temp_num_primitives;
				}
				if (m_current_level > m_highest_level)
					m_highest_level = m_current_level;
				m_stage++;
			} nobreak;
			case 1: {
				ASSERT(m_primitives[m_current_level] == null); 
				if ((status = ReadOneList(tk)) != TK_Normal)
					return status;
				
				count = vlist_count(m_current_working);
				m_num_primitives[m_current_level] = count;
				m_primitives[m_current_level] = new BBaseOpcodeHandler *[count];
				for (j = 0; j < count; j++) {
					handler = (BBaseOpcodeHandler *)vlist_remove_first(m_current_working);
					m_primitives[m_current_level][j] = handler;
				}
				m_substage = 0;
				m_stage = 0;
			} break;

			default:
				return tk.Error("unrecognized case of m_stage during TK_LOD::Read");
		}
	}
}


void TK_LOD::Reset () alter {
	int i, j;

	if (m_primitives != null) {
		for (i = 0; i <= m_highest_level; i++) {
			for (j = 0; j < m_num_primitives[i]; j++) {
				delete m_primitives[i][j];
			}
			if (m_primitives[i]) {
				delete [] m_primitives[i];
				m_primitives[i] = null;
			}
		}
		delete [] m_primitives;
		m_primitives = null;
	}
	if (m_current_working != null) {
		BBaseOpcodeHandler *handler;
		vlist_t *list;
        
		list = (vlist_t *)m_current_working;
		for (;;) {
			handler = (BBaseOpcodeHandler *)vlist_remove_first(list);
			if (handler == null)
				break;
			delete handler;
		}
		delete_vlist(list);
		m_current_working = null;
	}

	delete [] m_num_primitives;
	m_num_primitives = null;

	m_highest_level = 0;
	m_levels_allocated = 0;
	m_stage = m_substage = m_progress = 0;
	m_current_level = 0;

	BBaseOpcodeHandler::Reset();
}




TK_Status TK_Point::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_point, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (tk.GetTargetVersion() >= 1170 && 
				(m_opcode == TKE_Distant_Light || m_opcode == TKE_Local_Light)) {
				if ((status = PutData (tk, m_options)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if (Tagging (tk) && m_opcode != TKE_Text_Path)
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error("internal error in TK_Point::Write");
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Point::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_point, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if (tk.GetVersion() >= 1170 &&
				(m_opcode == TKE_Local_Light || m_opcode == TKE_Distant_Light)) {
				if ((status = GetData (tk, m_options)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error("internal error in TK_Point::Read");
	}

	return status;
}



TK_Status TK_Line::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (Opcode() == TKE_Infinite_Line && tk.GetTargetVersion() < 1515 ||
		Opcode() == TKE_Infinite_Ray && tk.GetTargetVersion() < 1605)
		return TK_Normal;


	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_points, 6)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Line::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_points, 6)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Polypoint::~TK_Polypoint() {
	delete [] m_points;
}

TK_Status TK_Polypoint::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			int			count = m_count;

			if (count < 0)
				count = -count;
			if ((status = PutData (tk, m_points, 3*count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Polypoint::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_count)) != TK_Normal)
				return status;
			if (Opcode() != TKE_Polyline && !validate_count (m_count))
				return tk.Error("bad Polypoint count");
			set_points (m_count);   
			m_stage++;
		}   nobreak;

		case 1: {
			int			count = m_count;

			if (count < 0)
				count = -count;
			if ((status = GetData (tk, m_points, 3*count)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}


void TK_Polypoint::Reset () alter {
	m_count = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_Polypoint::SetPoints (int count, float const * points) alter {
	if ((m_count = count) < 0)
		count = -count;

	if (count > m_allocated) {
		delete [] m_points;
		m_allocated = count + 16;
		m_points = new float [3*m_allocated];
	}
	if (points != null)
		memcpy (m_points, points, count*3*sizeof(float));
}



TK_NURBS_Curve::TK_NURBS_Curve()
	: BBaseOpcodeHandler (TKE_NURBS_Curve) {
	m_control_points = null;
	m_weights = null;
	m_knots = null;
	Reset();
}

TK_NURBS_Curve::~TK_NURBS_Curve() {
	delete [] m_control_points;
	delete [] m_knots;
	delete [] m_weights;
}

TK_Status TK_NURBS_Curve::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif


	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_optionals)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_degree)) != TK_Normal)
				return status;
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_control_point_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		
		case 4: {
			if ((status = PutData (tk, m_control_points, 3 * m_control_point_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		
		case 5: {
			if (m_optionals & NC_HAS_WEIGHTS) {
				if ((status = PutData (tk, m_weights, m_control_point_count)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 6: {
			if (m_optionals & NC_HAS_KNOTS) {
				if ((status = PutData (tk, m_knots, m_knot_count_implicit)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if (m_optionals & NC_HAS_START) {
				if ((status = PutData (tk, m_start)) != TK_Normal)
					return status;
			}
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 8: {
			if (m_optionals & NC_HAS_END) {
				if ((status = PutData (tk, m_end)) != TK_Normal)
					return status;
			}
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 9: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
} 


TK_Status TK_NURBS_Curve::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_optionals)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_degree)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_control_point_count)) != TK_Normal)
				return status;
			if (!validate_count (m_control_point_count))
				return tk.Error("bad NURBS Curve count");
			set_curve (m_degree, m_control_point_count);    
			m_stage++;
		}   nobreak;


		case 3: {
			if ((status = GetData (tk, m_control_points, 3 * m_control_point_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if (m_optionals & NC_HAS_WEIGHTS) {
				if ((status = GetData (tk, m_weights, m_control_point_count)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if (m_optionals & NC_HAS_KNOTS) {
				if ((status = GetData (tk, m_knots, m_knot_count_implicit)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if (m_optionals & NC_HAS_START) {
				if ((status = GetData (tk, m_start)) != TK_Normal)
					return status;
			}
			else
				m_start = 0.0;
			m_stage++;
		}   nobreak;

		case 7: {
			if (m_optionals & NC_HAS_END) {
				if ((status = GetData (tk, m_end)) != TK_Normal)
					return status;
			}
			else
				m_end = 1.0;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;

} 


void TK_NURBS_Curve::Reset () alter {
	delete [] m_control_points;     m_control_points = null;
	delete [] m_weights;            m_weights = null;
	delete [] m_knots;              m_knots = null;

	m_degree = 0;
	m_control_point_count = 0;
	m_knot_count_implicit = 0;
	m_optionals = 0;
	m_start = 0;
	m_end = 1;

	m_stage = 0;
	m_progress = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_NURBS_Curve::set_curve (int degree, int control_count,
								float const * points, float const * weights, float const * knots,
								float start, float end) alter {
	m_degree = (unsigned char)degree;
	m_control_point_count = control_count;

	m_knot_count_implicit = m_degree + m_control_point_count + 1;

	delete [] m_control_points;
	delete [] m_weights;
	delete [] m_knots;
	m_control_points = new float [3 * m_control_point_count];
	m_weights = new float [m_control_point_count];
	m_knots = new float [m_knot_count_implicit];

	if ((m_start = start) != 0.0f)
		m_optionals |= NC_HAS_START;
	if ((m_end = end) != 1.0f)
		m_optionals |= NC_HAS_END;

	if (points != null)
		memcpy (m_control_points, points, m_control_point_count*3*sizeof(float));
	if (weights != null) {
		memcpy (m_weights, weights, m_control_point_count*sizeof(float));
		m_optionals |= NC_HAS_WEIGHTS;
	}
	if (knots != null) {
		memcpy (m_knots, knots, m_knot_count_implicit*sizeof(float));
		m_optionals |= NC_HAS_KNOTS;
	}
}



HT_NURBS_Trim::HT_NURBS_Trim () : BBaseOpcodeHandler (0) {
	m_next = null;
	m_type = NS_TRIM_END;   
	m_options = 0;
	m_degree = 0;
	m_count = 0;
	m_points = null;
	m_weights = null;
	m_knots = null;
	m_list = null;
	m_substage = 0;
	m_progress = 0;
	m_current_trim = null;
}

HT_NURBS_Trim::~HT_NURBS_Trim() {
	delete [] m_points;
	delete [] m_weights;
	delete [] m_knots;
	delete m_next;
	delete m_list;
}


void HT_NURBS_Trim::SetPoly (int count, float const * points) {
	m_type = NS_TRIM_POLY;
	m_degree = 0;
	m_count = count;
	m_points = new float [2*count];
	m_weights = null;
	m_knots = null;
	m_list = null;
	if (points != null)
		memcpy (m_points, points, 2*count*sizeof(float));
}

void HT_NURBS_Trim::SetCurve (int degree, int count, float const * points, float const * weights, float const * knots, float start_u, float end_u) {
	m_type = NS_TRIM_CURVE;
	m_degree = (unsigned char)degree;
	m_count = count;
	m_points = new float [2*count];
	m_weights = new float [count];
	m_knots = new float [degree + count + 1];
	m_start_u = start_u;
	m_end_u = end_u;
	m_list = null;
	if (points != null)
		memcpy (m_points, points, 2*count*sizeof(float));
	if (weights != null) {
		m_options |= NS_TRIM_HAS_WEIGHTS;
		memcpy (m_weights, weights, count*sizeof(float));
	}
	if (knots != null) {
		m_options |= NS_TRIM_HAS_KNOTS;
		memcpy (m_knots, knots, (degree+count+1)*sizeof(float));
	}
}

void HT_NURBS_Trim::SetCollection () {
	m_type = NS_TRIM_COLLECTION;
	m_degree = 0;
	m_count = 0;
	m_points = null;
	m_weights = null;
	m_knots = null;
	m_list = null;
}

TK_Status HT_NURBS_Trim::write_collection (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;
	int             i;
	HT_NURBS_Trim   *ptr;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return write_collection_ascii (tk);
#endif

	ptr = m_list;
	for (i = 0; i < m_progress; i++) {
		ptr = ptr->m_next;
	}

	switch(m_substage) {

		case 0: {
			while (ptr != null) {
				if ((status = ptr->Write(tk)) != TK_Normal)
					return status;
				ptr = ptr->m_next;
				m_progress++;
			}
			m_progress = 0;
			m_substage++;
		} nobreak;

		case 1: {
			unsigned char       end = NS_TRIM_END;

			if ((status = PutData (tk, end)) != TK_Normal)
				return status;
			m_substage = 0;
		} break;

		default:
			return tk.Error();

	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status HT_NURBS_Trim::read_collection(BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
	HT_NURBS_Trim   *tail;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return read_collection_ascii (tk);
#endif

	tail = m_list;
	while (tail != null && tail->m_next != null) {
		tail = tail->m_next;
	}
	while (1) {
		if (m_current_trim == null) {
			unsigned char       type;

			if ((status = GetData (tk, type)) != TK_Normal)
				return status;

			if (type == NS_TRIM_END)
				break;
			else if (type > NS_TRIM_LAST_KNOWN_TYPE)
				return tk.Error ("Unknown trim type");

			m_current_trim = new HT_NURBS_Trim;
			m_current_trim->m_type = type;
		}
		if ((status = m_current_trim->Read (tk)) != TK_Normal)
			return status;
		if (m_list == null) {
			m_list = tail = m_current_trim;
		}
		else {
			tail->m_next = m_current_trim;
			tail = m_current_trim;
		}
		m_current_trim = null;
	}

	return status;
}


TK_Status HT_NURBS_Trim::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif


	switch (m_stage) {
		case 0: {
			if ((status = PutData (tk, m_type)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_options)) != TK_Normal)
				return status;
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_type == NS_TRIM_COLLECTION) {
				if ((status = write_collection(tk)) != TK_Normal)
					return status;
				m_stage = -1;
				return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = PutData (tk, m_points, 2*m_count)) != TK_Normal)
				return status;
			if (m_type == NS_TRIM_POLY) {
				m_stage = -1;
				return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((status = PutData (tk, m_degree)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 6: {
			if (m_options & NS_TRIM_HAS_WEIGHTS) {
				if ((status = PutData (tk, m_weights, m_count)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if (m_options & NS_TRIM_HAS_KNOTS) {
				if ((status = PutData (tk, m_knots, m_degree + m_count + 1)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}


TK_Status HT_NURBS_Trim::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_options)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_type == NS_TRIM_COLLECTION) {
				if ((status = read_collection(tk)) != TK_Normal)
					return status;
				m_stage = -1;
				return status;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_count)) != TK_Normal)
				return status;
			if (!validate_count (m_count))
				return tk.Error("bad NURBS Trim count");
			m_points = new float [2*m_count];
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = GetData (tk, m_points, 2*m_count)) != TK_Normal)
				return status;
			if (m_type == NS_TRIM_POLY) {
				m_stage = -1;
				return status;
			}
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = GetData (tk, m_degree)) != TK_Normal)
				return status;
			if ((m_options & NS_TRIM_HAS_WEIGHTS) != 0)
				m_weights = new float [m_count];
			if ((m_options & NS_TRIM_HAS_KNOTS) != 0)
				m_knots = new float [m_degree + m_count + 1];
			m_stage++;
		}   nobreak;

		case 5: {
			if (m_options & NS_TRIM_HAS_WEIGHTS) {
				if ((status = GetData (tk, m_weights, m_count)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if (m_options & NS_TRIM_HAS_KNOTS) {
				if ((status = GetData (tk, m_knots, m_degree + m_count + 1)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_NURBS_Surface::TK_NURBS_Surface()
	: BBaseOpcodeHandler (TKE_NURBS_Surface) {
	m_control_points = null;
	m_weights = null;
	m_u_knots = null;
	m_v_knots = null;
	m_trims = null;
	Reset();
}

TK_NURBS_Surface::~TK_NURBS_Surface() {
	delete [] m_control_points;
	delete [] m_weights;
	delete [] m_u_knots;
	delete [] m_v_knots;
	delete m_trims;
}

TK_Status TK_NURBS_Surface::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;


#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if (tk.GetTargetVersion() < 650)    
				return TK_Normal;
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_optionals)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_degree, 2)) != TK_Normal)
				return status;
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_size, 2)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		
		case 4: {
			if ((status = PutData (tk, m_control_points, 3 * m_size[0] * m_size[1])) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		
		case 5: {
			if (m_optionals & NS_HAS_WEIGHTS) {
				if ((status = PutData (tk, m_weights, m_size[0] * m_size[1])) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 6: {
			if (m_optionals & NS_HAS_KNOTS) {
				if ((status = PutData (tk, m_u_knots, m_degree[0] + m_size[0] + 1)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if (m_optionals & NS_HAS_KNOTS) {
				if ((status = PutData (tk, m_v_knots, m_degree[1] + m_size[1] + 1)) != TK_Normal)
					return status;
			}
			m_current_trim = m_trims;
			m_stage++;
		}   nobreak;

		case 8: {
			if (m_optionals & NS_HAS_TRIMS) {
				while (m_current_trim != null) {
					if ((status = m_current_trim->Write (tk)) != TK_Normal)
						return status;
					m_current_trim = m_current_trim->m_next;
				}
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if (m_optionals & NS_HAS_TRIMS) {
				unsigned char       end = NS_TRIM_END;

				if ((status = PutData (tk, end)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 10: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
} 


TK_Status TK_NURBS_Surface::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_optionals)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_degree, 2)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_size, 2)) != TK_Normal)
				return status;
			if (!validate_count (m_size[0]*m_size[1]))
				return tk.Error("bad NURBS Surface count");
			SetSurface (m_degree[0], m_degree[1], m_size[0], m_size[1]);    
			m_stage++;
		}   nobreak;


		case 3: {
			if ((status = GetData (tk, m_control_points, 3 * m_size[0] * m_size[1])) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if (m_optionals & NS_HAS_WEIGHTS) {
				if ((status = GetData (tk, m_weights, m_size[0] * m_size[1])) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if (m_optionals & NS_HAS_KNOTS) {
				if ((status = GetData (tk, m_u_knots, m_degree[0] + m_size[0] + 1)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if (m_optionals & NS_HAS_KNOTS) {
				if ((status = GetData (tk, m_v_knots, m_degree[1] + m_size[1] + 1)) != TK_Normal)
					return status;
			}
			m_current_trim = null;
			m_stage++;
		}   nobreak;

		case 7: {
			if (m_optionals & NS_HAS_TRIMS) {
				while (1) {
					if (m_current_trim == null) {
						unsigned char       type;

						if ((status = GetData (tk, type)) != TK_Normal)
							return status;

						if (type == NS_TRIM_END)
							break;
						else if (type > NS_TRIM_LAST_KNOWN_TYPE)
							return tk.Error ("Unknown trim type");

						m_current_trim = NewTrim (type);
					}

					if ((status = m_current_trim->Read (tk)) != TK_Normal)
						return status;
					m_current_trim = null;
				}
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;

} 


void TK_NURBS_Surface::Reset () alter {
	delete [] m_control_points;     m_control_points = null;
	delete [] m_weights;            m_weights = null;
	delete [] m_u_knots;            m_u_knots = null;
	delete [] m_v_knots;            m_v_knots = null;

	delete m_trims;                 m_trims = m_current_trim = null;

	m_degree[0] = m_degree[1] = 0;
	m_size[0] = m_size[1] = 0;
	m_optionals = 0;

	m_stage = 0;
	m_progress = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_NURBS_Surface::SetSurface (int u_degree, int v_degree,  int u_size, int v_size,
									float const * points,       float const * weights,
									float const * u_knots,      float const * v_knots) alter {

	m_degree[0] = (unsigned char)u_degree;
	m_degree[1] = (unsigned char)v_degree;
	m_size[0] = u_size;
	m_size[1] = v_size;

	delete [] m_control_points;
	delete [] m_weights;
	delete [] m_u_knots;
	delete [] m_v_knots;
	m_control_points = new float [3 * u_size * v_size];
	m_weights = new float [u_size * v_size];
	m_u_knots = new float [u_degree + u_size + 1];
	m_v_knots = new float [v_degree + v_size + 1];

	delete m_trims;
	m_trims = m_current_trim = null;

	if (points != null)
		memcpy (m_control_points, points, u_size * v_size*3*sizeof(float));
	if (weights != null) {
		memcpy (m_weights, weights, u_size * v_size*sizeof(float));
		m_optionals |= NC_HAS_WEIGHTS;
	}
	if (u_knots != null) {
		memcpy (m_u_knots, u_knots, (u_degree + u_size + 1)*sizeof(float));
		m_optionals |= NC_HAS_KNOTS;
	}
	if (v_knots != null) {
		memcpy (m_v_knots, v_knots, (v_degree + v_size + 1)*sizeof(float));
		m_optionals |= NC_HAS_KNOTS;
	}
}

HT_NURBS_Trim * TK_NURBS_Surface::NewTrim (int type) alter {
	HT_NURBS_Trim *     trim;

	
	trim = new HT_NURBS_Trim;
	trim->m_next = m_trims;
	m_trims = trim;
	trim->m_type = (unsigned char)type;

	return trim;
}



TK_Area_Light::~TK_Area_Light() {
	delete [] m_points;
}

TK_Status TK_Area_Light::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_points, 3*m_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_options)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Area_Light::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_count)) != TK_Normal)
				return status;
			if (!validate_count (m_count)) {
				char buffer[1024] = {""};
				sprintf(buffer, "bad Area Light count, %d", m_count);
                return tk.Error(buffer);
			}
			set_points (m_count);
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_points, 3*m_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_options)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Area_Light::Reset () alter {
	delete [] m_points;
	m_points = null;
	m_count = 0;
	m_options = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_Area_Light::set_points (int count, float const * points) alter {
	m_count = count;
	delete [] m_points;
	m_points = new float [3*m_count];
	if (points != null)
		memcpy (m_points, points, m_count*3*sizeof(float));
}




TK_Status TK_Spot_Light::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_position, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_target, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_options)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_options & TKO_Spot_Outer_Mask) != 0) {
				if ((status = PutData (tk, m_outer)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_options & TKO_Spot_Inner_Mask) != 0) {
				if ((status = PutData (tk, m_inner)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_options & TKO_Spot_Concentration) != 0) {
				if ((status = PutData (tk, m_concentration)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Spot_Light::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_position, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_target, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_options)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_options & TKO_Spot_Outer_Mask) != 0) {
				if ((status = GetData (tk, m_outer)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_options & TKO_Spot_Inner_Mask) != 0) {
				if ((status = GetData (tk, m_inner)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_options & TKO_Spot_Concentration) != 0) {
				if ((status = GetData (tk, m_concentration)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



void TK_Cutting_Plane::Reset () {
	delete [] m_planes;
	m_planes = null;
	m_count = 0;
	BBaseOpcodeHandler::Reset();
}


TK_Cutting_Plane::~TK_Cutting_Plane() {
	delete [] m_planes;
}

void TK_Cutting_Plane::SetPlanes (int count, float const * planes) alter {
	if (count != m_count) {
		delete [] m_planes;
		if ((m_count = count) > 0)
			m_planes = new float [4*m_count];
		else
			m_planes = null;
	}

	if (planes != null)
		memcpy (m_planes, planes, m_count*4*sizeof(float));
}

TK_Status TK_Cutting_Plane::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;
	int				count = m_count;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1305)
		count = 1;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if (tk.GetTargetVersion() >= 1305)
				if ((status = PutData (tk, count)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_planes, 4*count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Cutting_Plane::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			int			count;

			if (tk.GetVersion() >= 1305) {
				if ((status = GetData (tk, count)) != TK_Normal)
					return status;
			}
			else
				count = 1;
			SetPlanes (count);
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_planes, 4*m_count)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Status TK_Circle::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_start, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_middle, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_end, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if (tk.GetTargetVersion() >= 1215) {
				if ((status = PutData (tk, m_flags)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if (tk.GetTargetVersion() >= 1215 &&
				(m_flags & TKO_Circular_Center) != 0) {
				if ((status = PutData (tk, m_center, 3)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Circle::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_start, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_middle, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_end, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if (tk.GetVersion() >= 1215) {
				if ((status = GetData (tk, m_flags)) != TK_Normal)
					return status;
			}
			else
				m_flags = 0;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_flags & TKO_Circular_Center) != 0) {
				if ((status = GetData (tk, m_center, 3)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Circle::Reset () {
	m_flags = 0;
	BBaseOpcodeHandler::Reset();
}



TK_Status TK_Ellipse::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif


	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_center, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_major, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_minor, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if (Opcode() == TKE_Elliptical_Arc) {
				if ((status = PutData (tk, m_limits, 2)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Ellipse::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_center, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_major, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_minor, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if (Opcode() == TKE_Elliptical_Arc) {
				if ((status = GetData (tk, m_limits, 2)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}




TK_Status TK_Sphere::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if (tk.GetTargetVersion() < 1155)    
				return TK_Normal;
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_flags)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_center, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_radius)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if (!(m_flags & TKSPH_NULL_AXIS)) {
				if ((status = PutData (tk, m_axis, 3)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if (!(m_flags & TKSPH_NULL_AXIS)) {
				if ((status = PutData (tk, m_ortho, 3)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if (Tagging (tk))
				status = Tag (tk);
			m_stage = -1;
		}   break;

		default:
			return tk.Error("internal error in TK_Sphere::Write");
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Sphere::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_flags)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_center, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_radius)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if (!(m_flags & TKSPH_NULL_AXIS)) {
				if ((status = GetData (tk, m_axis, 3)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if (!(m_flags & TKSPH_NULL_AXIS)) {
				if ((status = GetData (tk, m_ortho, 3)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error("internal error in TK_Sphere::Read");
	}

	return status;
}

void TK_Sphere::Reset () alter {
	m_radius = -1;
	m_flags = TKSPH_NULL_AXIS;
	m_axis[0] = 0;
	m_axis[1] = 1;
	m_axis[2] = 0;
	m_ortho[0] = 1;
	m_ortho[1] = 0;
	m_ortho[2] = 0;
	BBaseOpcodeHandler::Reset();
}




TK_Status TK_Cylinder::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif 

	switch (m_stage) {
		case 0: {
			if (tk.GetTargetVersion() < 650)    
				return TK_Normal;
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_axis, 6)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_radius)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_flags)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Cylinder::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
  switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_axis, 6)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_radius)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_flags)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}





TK_PolyCylinder::~TK_PolyCylinder() {
	delete [] m_points;
	delete [] m_radii;
}

TK_Status TK_PolyCylinder::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;
	bool            do_attributes = (tk.GetTargetVersion() >= 805);

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if (tk.GetTargetVersion() < 705)    
				return TK_Normal;
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_points, 3*m_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_radius_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = PutData (tk, m_radii, m_radius_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			unsigned char       flags = m_flags;

			if (!do_attributes)
				flags &= 0x03;  

			if ((status = PutData (tk, flags)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 6: {
			if (do_attributes && (m_flags & TKCYL_NORMAL_FIRST))
				if ((status = PutData (tk, &m_normals[0], 3)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 7: {
			if (do_attributes && (m_flags & TKCYL_NORMAL_SECOND))
				if ((status = PutData (tk, &m_normals[3], 3)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 8: {
			if (do_attributes && (m_flags & TKCYL_OPTIONALS))
				if ((status = TK_Polyhedron::Write (tk)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 9: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_PolyCylinder::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_count)) != TK_Normal)
				return status;
			if (!validate_count (m_count))
				return tk.Error("bad TK_PolyCylinder count");
			SetPoints (m_count);   
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_points, 3*m_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_radius_count)) != TK_Normal)
				return status;
			if (!validate_count (m_radius_count))
				return tk.Error("bad TK_PolyCylinder radius_count");
			SetRadii (m_radius_count);   
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = GetData (tk, m_radii, m_radius_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = GetData (tk, m_flags)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			if (m_flags & TKCYL_NORMAL_FIRST)
				if ((status = GetData (tk, &m_normals[0], 3)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 6: {
			if (m_flags & TKCYL_NORMAL_SECOND)
				if ((status = GetData (tk, &m_normals[3], 3)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 7: {
			if (m_flags & TKCYL_OPTIONALS)
				if ((status = TK_Polyhedron::Read (tk)) != TK_Normal)
					return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}


void TK_PolyCylinder::Reset () alter {
	m_count = 0;
	delete [] m_points;
	m_points = null;
	m_radius_count = 0;
	delete [] m_radii;
	m_radii = null;

	TK_Polyhedron::Reset();
}

void TK_PolyCylinder::SetPoints (int count, float const * points) alter {
	m_count = count;
	delete [] m_points;     
	m_points = new float [3*m_count];
	if (points != null)
		memcpy (m_points, points, m_count*3*sizeof(float));

	TK_Polyhedron::SetPoints (count, null);
}

void TK_PolyCylinder::SetRadii (int count, float const * radii) alter {
	m_radius_count = count;
	delete [] m_radii;      
	m_radii = new float [m_radius_count];
	if (radii != null)
		memcpy (m_radii, radii, m_radius_count*sizeof(float));
}



TK_Status TK_Grid::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_type)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_origin, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_ref1, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = PutData (tk, m_ref2, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			if ((status = PutData (tk, m_counts, 2)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 6: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Grid::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_type)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_origin, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_ref1, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = GetData (tk, m_ref2, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = GetData (tk, m_counts, 2)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}




TK_Text::TK_Text(unsigned char opcode)
		: BBaseOpcodeHandler (opcode)
{
	m_allocated = 0;
	m_string = null;
	m_character_attributes = null;
	Reset();
}


TK_Text::~TK_Text() {
	delete [] m_string;
}


void TK_Text::SetTextRegion (int c, float const * p, int o) alter {
	if ((m_region_count = (unsigned char)c) != 0) {
		m_options |= TKO_Text_Option_Region;
		memcpy (m_region, p, c*3*sizeof(float));
	}
	else
		m_options &= ~TKO_Text_Option_Region;

	m_region_options = (unsigned char)o;
}


TK_Status TK_Text::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status			status = TK_Normal;
	unsigned char		options = m_options;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif


	if (tk.GetTargetVersion() < 1002)
		options &= ~TKO_Text_Option_Region;
	if (tk.GetTargetVersion() < 1210)
		options &= ~TKO_Text_Option_Character_Attributes;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_position, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (Opcode() == TKE_Text_With_Encoding) {
				if (m_encoding == TKO_Enc_WCS) {
					if (sizeof(wchar_t) == 4) {
						if ((status = PutData (tk, (char)TKO_Enc_UTF32)) != TK_Normal)
							return status;
					}
					else {
						if ((status = PutData (tk, (char)TKO_Enc_UTF16)) != TK_Normal)
							return status;
					}
				}
				else {
					if ((status = PutData (tk, m_encoding)) != TK_Normal)
						return status;
				}
			}

			m_stage++;
		}   nobreak;

		case 3: {
			unsigned char       byte;

			if (tk.GetTargetVersion() > 600) {  
				if (m_length > 65535)
					byte = 255;                 
				else if (m_length > 253)
					byte = 254;                 
				else
					byte = (unsigned char)m_length;
			}
			else {
				if (m_length > 255)
					byte = 255;                 
				else
					byte = (unsigned char)m_length;
			}

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;

			m_stage++;
		}   nobreak;

		case 4: {
			if (tk.GetTargetVersion() > 600) {
				if (m_length > 65535) {
					if ((status = PutData (tk, m_length)) != TK_Normal)
						return status;
				}
				else if (m_length > 253) {
					unsigned short      word = (unsigned short)m_length;

					if ((status = PutData (tk, word)) != TK_Normal)
						return status;
				}
			}

			m_stage++;
		}   nobreak;

		case 5: {
			switch (m_encoding) {
				case TKO_Enc_WCS: {
					if (sizeof(wchar_t) == 4) {
						if ((status = PutData (tk, (int const *)m_string, m_length/4)) != TK_Normal)
							return status;
					}
					else {
						if ((status = PutData (tk, (short const *)m_string, m_length/2)) != TK_Normal)
							return status;
					}
				}	break;

				case TKO_Enc_Unicode32:
				case TKO_Enc_UTF32: {
					if ((status = PutData (tk, (int const *)m_string, m_length/4)) != TK_Normal)
						return status;
				}	break;

				case TKO_Enc_Unicode:
				case TKO_Enc_UTF16: {
					if ((status = PutData (tk, (short const *)m_string, m_length/2)) != TK_Normal)
						return status;
				}	break;
				
				default: {
					if ((status = PutData (tk, m_string, m_length)) != TK_Normal)
						return status;
				}	break;
			}

			m_stage++;
		}   nobreak;

		case 6: {
			if (tk.GetTargetVersion() >= 1002) {
				if ((status = PutData (tk, options)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if ((options & TKO_Text_Option_Region) != 0) {
				if ((status = PutData (tk, m_region_options)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 8: {
			if ((options & TKO_Text_Option_Region) != 0) {
				if ((status = PutData (tk, m_region_count)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if ((options & TKO_Text_Option_Region) != 0) {
				if ((status = PutData (tk, m_region, 3*m_region_count)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 10: {
			if ((options & TKO_Text_Option_Character_Attributes) != 0) {
				if ((status = PutData (tk, m_count)) != TK_Normal)
					return status;
				m_progress = 0;
				m_substage = 0;
			}
			m_stage++;
		}   nobreak;

		case 11: {
			if ((options & TKO_Text_Option_Character_Attributes) != 0) {
				while (m_progress < m_count) {
					TK_Character_Attribute *	ca = &m_character_attributes[m_progress];

					switch (m_substage) {
						case 0: {
							if ((status = PutData (tk, ca->mask)) != TK_Normal)
								return status;
							m_substage++;
						}   nobreak;

						case 1: {
							if ((ca->mask & TKO_Character_Name) != 0) {
								int				len = (int)strlen (ca->name);

								if ((status = PutData (tk, len)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 2: {
							if ((ca->mask & TKO_Character_Name) != 0) {
								int				len = (int)strlen (ca->name);

								if ((status = PutData (tk, ca->name, len)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 3: {
							if ((ca->mask & TKO_Character_Size) != 0) {
								if ((status = PutData (tk, ca->size)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 4: {
							if ((ca->mask & TKO_Character_Size) != 0) {
								if ((status = PutData (tk, ca->size_units)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 5: {
							if ((ca->mask & TKO_Character_Vertical_Offset) != 0) {
								if ((status = PutData (tk, ca->vertical_offset)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 6: {
							if ((ca->mask & TKO_Character_Vertical_Offset) != 0) {
								if ((status = PutData (tk, ca->vertical_offset_units)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 7: {
							if ((ca->mask & TKO_Character_Slant) != 0) {
								if ((status = PutData (tk, ca->slant)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 8: {
							if ((ca->mask & TKO_Character_Rotation) != 0) {
								if ((status = PutData (tk, ca->rotation)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 9: {
							if ((ca->mask & TKO_Character_Width_Scale) != 0) {
								if ((status = PutData (tk, ca->width_scale)) != TK_Normal)
									return status;
							}
							m_substage = 0;
						}   break;
					}

					++m_progress;
				}
			}
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 12: {
			if (Tagging (tk))
				status = Tag (tk);

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Text::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_position, 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if (Opcode() == TKE_Text_With_Encoding) {
				if ((status = GetData (tk, m_encoding)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_length = byte;

			m_stage++;
		}   nobreak;

		case 3: {
			if (tk.GetVersion() > 600) {        
				if (m_length == 255) {                  
					if ((status = GetData (tk, m_length)) != TK_Normal)
						return status;
				}
				else if (m_length == 254) {             
					unsigned short      word;

					if ((status = GetData (tk, word)) != TK_Normal)
						return status;
					m_length = word;
				}
			}
            
			set_string (m_length);      
			m_stage++;
		}   nobreak;

		case 4: {
			switch (m_encoding) {
				case TKO_Enc_UTF32:
				case TKO_Enc_Unicode32: {
					if ((status = GetData (tk, (int alter *)m_string, m_length/4)) != TK_Normal)
						return status;
				}	break;

				case TKO_Enc_UTF16:
				case TKO_Enc_Unicode: {
					if ((status = GetData (tk, (short alter *)m_string, m_length/2)) != TK_Normal)
						return status;
				}	break;

				default: {
					if ((status = GetData (tk, m_string, m_length)) != TK_Normal)
						return status;
				}	break;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if (tk.GetVersion() >= 1002) {
				if ((status = GetData (tk, m_options)) != TK_Normal)
					return status;
			}
			else
				m_options = 0;
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_options & TKO_Text_Option_Region) != 0) {
				if ((status = GetData (tk, m_region_options)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if ((m_options & TKO_Text_Option_Region) != 0) {
				if ((status = GetData (tk, m_region_count)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 8: {
			if ((m_options & TKO_Text_Option_Region) != 0) {
				if ((status = GetData (tk, m_region, 3*m_region_count)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if ((m_options & TKO_Text_Option_Character_Attributes) != 0) {
				if ((status = GetData (tk, m_count)) != TK_Normal)
					return status;

				m_character_attributes = new TK_Character_Attribute [m_count];
				memset (m_character_attributes, 0, m_count*sizeof(TK_Character_Attribute));
				m_progress = 0;
				m_substage = 0;
			}
			m_stage++;
		}   nobreak;

		case 10: {
			if ((m_options & TKO_Text_Option_Character_Attributes) != 0) {
				while (m_progress < m_count) {
					TK_Character_Attribute *	ca = &m_character_attributes[m_progress];

					switch (m_substage) {
						case 0: {
							if ((status = GetData (tk, ca->mask)) != TK_Normal)
								return status;
							m_substage++;
						}   nobreak;

						case 1: {
							if ((ca->mask & TKO_Character_Name) != 0) {
								if ((status = GetData (tk, m_tmp)) != TK_Normal)
									return status;
								ca->name = new char [m_tmp+1];
								ca->name[m_tmp] = '\0';
							}
							m_substage++;
						}   nobreak;

						case 2: {
							if ((ca->mask & TKO_Character_Name) != 0) {
								if ((status = GetData (tk, ca->name, m_tmp)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 3: {
							if ((ca->mask & TKO_Character_Size) != 0) {
								if ((status = GetData (tk, ca->size)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 4: {
							if ((ca->mask & TKO_Character_Size) != 0) {
								if ((status = GetData (tk, ca->size_units)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 5: {
							if ((ca->mask & TKO_Character_Vertical_Offset) != 0) {
								if ((status = GetData (tk, ca->vertical_offset)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 6: {
							if ((ca->mask & TKO_Character_Vertical_Offset) != 0) {
								if ((status = GetData (tk, ca->vertical_offset_units)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 7: {
							if ((ca->mask & TKO_Character_Slant) != 0) {
								if ((status = GetData (tk, ca->slant)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 8: {
							if ((ca->mask & TKO_Character_Rotation) != 0) {
								if ((status = GetData (tk, ca->rotation)) != TK_Normal)
									return status;
							}
							m_substage++;
						}   nobreak;

						case 9: {
							if ((ca->mask & TKO_Character_Width_Scale) != 0) {
								if ((status = GetData (tk, ca->width_scale)) != TK_Normal)
									return status;
							}
							m_substage = 0;
						}   break;
					}

					++m_progress;
				}
			}
			m_progress = 0;
			m_stage = -1;
		}	break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Text::Reset () alter {
	m_length = 0;
	m_encoding = TKO_Enc_ISO_Latin_One;
	m_options = 0;
	m_region_count = 0;
	m_region_options = 0;
	m_position[0] = m_position[1] = m_position[2] = 0.0f;
	if (m_character_attributes != null) {
		while (m_count-- > 0) {
			delete [] m_character_attributes[m_count].name;
		}
		delete [] m_character_attributes;
		m_character_attributes = null;
	}
	BBaseOpcodeHandler::Reset();
}

void TK_Text::set_string (int length) alter {
	m_length = length;
	if (m_length > m_allocated-4) {
		delete [] m_string;
		m_string = new char [m_allocated = m_length + 64];
	}
	m_string[m_length] = '\0';
	m_string[m_length+1] = '\0';    
	m_string[m_length+2] = '\0';    
	m_string[m_length+3] = '\0';    
}

void TK_Text::set_string (char const * string) alter {
	set_string ((int) strlen (string));
	strcpy (m_string, string);
}

void TK_Text::SetString (unsigned short const * string) alter {
	unsigned short const *      sp = string;
	int                         bytes;

	while (*sp++ != 0);
	bytes = sizeof(unsigned short) * (int)(sp - string);  
	if (bytes > m_allocated) {
		delete [] m_string;
		m_string = new char [m_allocated = bytes + 16];
	}
	memcpy (m_string, string, bytes);

	m_length = bytes;
	m_encoding = TKO_Enc_Unicode;
}

void TK_Text::SetString (unsigned int const * string) alter {
	unsigned int const *      sp = string;
	int                         bytes;

	while (*sp++ != 0);
	bytes = sizeof(unsigned int) * (int)(sp - string);  
	if (bytes > m_allocated) {
		delete [] m_string;
		m_string = new char [m_allocated = bytes + 16];
	}
	memcpy (m_string, string, bytes);

	m_length = bytes;
	m_encoding = TKO_Enc_Unicode32;
}



TK_Font::~TK_Font() {
	delete [] m_bytes;
	delete [] m_name;
	delete [] m_lookup;
}


TK_Status TK_Font::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (m_encoding == TKO_Enc_Unicode32 && tk.GetTargetVersion() < 1210)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_type)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			unsigned char       byte = (unsigned char)m_name_length;
			
			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_name, m_name_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = PutData (tk, m_encoding)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			unsigned char       byte = (unsigned char)m_lookup_length;
			
			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 6: {
			if (m_lookup_length > 0) {
				if ((status = PutData (tk, m_lookup, m_lookup_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if ((status = PutData (tk, m_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 8: {
			if ((status = PutData (tk, m_bytes, m_length)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Font::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_type)) != TK_Normal)
				return status;
			if (m_type != TKO_Font_HOOPS_Stroked)
				return tk.Error ("Unknown font type");
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte;
			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			set_name ((int)byte);
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_name, m_name_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = GetData (tk, m_encoding)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			unsigned char       byte;
			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			if ((m_lookup_length = (int)byte) > 0)
				set_lookup ((int)byte);
			m_stage++;
		}   nobreak;

		case 5: {
			if (m_lookup_length > 0) {
				if ((status = GetData (tk, m_lookup, m_lookup_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if ((status = GetData (tk, m_length)) != TK_Normal)
				return status;
			set_bytes (m_length);
			m_stage++;
		}   nobreak;

		case 7: {
			if ((status = GetData (tk, m_bytes, m_length)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}


void TK_Font::Reset () alter {
	delete [] m_bytes;
	m_bytes = null;
	delete [] m_name;
	m_name = null;
	delete [] m_lookup;
	m_lookup = null;

	m_name_length = 0;
	m_lookup_length = 0;
	m_encoding = 0;

	BBaseOpcodeHandler::Reset();
}


void TK_Font::set_bytes (int count, char const * bytes) alter {
	delete [] m_bytes;
	m_bytes = new char [count];
	if (bytes != null)
		memcpy (m_bytes, bytes, count*sizeof(char));
}

void TK_Font::set_name (int length) alter {
	m_name_length = length;
	delete [] m_name;
	m_name = new char [m_name_length + 1];
	m_name[m_name_length] = '\0';
}

void TK_Font::set_name (char const * name) alter {
	set_name ((int) strlen (name));
	strcpy (m_name, name);
}

void TK_Font::set_lookup (int length) alter {
	m_lookup_length = length;
	delete [] m_lookup;
	m_lookup = new char [m_lookup_length + 1];
	m_lookup[m_lookup_length] = '\0';
}

void TK_Font::set_lookup (char const * lookup) alter {
	set_lookup ((int) strlen (lookup));
	strcpy (m_lookup, lookup);
}



TK_Image_Data_Buffer::~TK_Image_Data_Buffer() { delete [] m_buffer; }

void TK_Image_Data_Buffer::Resize (unsigned int size) alter {
	unsigned char *                 old_buffer = m_buffer;

	if (size == Size())
		return;

	m_buffer = new unsigned char [size];
	if (old_buffer != null) {
		if (size > m_allocated)
			memcpy (m_buffer, old_buffer, m_allocated);
		else
			memcpy (m_buffer, old_buffer, size);
	}
	m_allocated = size;
	delete [] old_buffer;
}

void TK_Image_Data_Buffer::Reset() {
	delete [] m_buffer;
	m_buffer = null;
	m_allocated = m_used = 0;
}


int const TK_Image_Bytes_Per_Pixel[] = {
	1, 2, 3, 4, 4, 4
};




TK_Color::TK_Color ()
	: BBaseOpcodeHandler (TKE_Color)
	, m_mask (0), m_channels (0) {
}

TK_Color::~TK_Color ()
{
	Reset ();
}

TK_Status TK_Color::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;
	int				length;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1550)
		m_mask &= ~TKO_Geo_Simple_Reflection;
	if (tk.GetTargetVersion() < 1650)
		m_mask &= ~TKO_Geo_Ambient_Down;

	if ((m_mask & TKO_Geo_Extended_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended;
	if ((m_mask & TKO_Geo_Extended_Colors_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended_Colors;
	if ((m_mask & TKO_Geo_Extended_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended;
	if (!m_mask)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte = (unsigned char)(m_mask & 0x000000FF);

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended_Colors) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended_Colors_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended2_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			unsigned char       byte = (unsigned char)(m_channels & 0x000000FF);

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_channels & (1 << TKO_Channel_Extended)) != 0) {
				unsigned char       byte = (unsigned char)((m_channels>>TKO_Channel_Extended_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_progress = 0; 
			m_stage++;
		}   nobreak;

		case 7: {
			if ((m_channels & (1 << TKO_Channel_Diffuse)) != 0) {

				if (m_diffuse.m_name) {
					int length;
					length = (int) strlen (m_diffuse.m_name);
					if (length < 255) {
						if ((status = PutData (tk, (unsigned char)length)) != TK_Normal)
							return status;
					}
					else {
						if (m_progress == 0) {
							if ((status = PutData (tk, (unsigned char)255)) != TK_Normal)
								return status;
							m_progress++;
						}
						if ((status = PutData (tk, length)) != TK_Normal)
							return status;
					}
				}
				else {
					
					if ((status = PutData (tk, (unsigned char)0)) != TK_Normal)
						return status;
				}
			}
			m_stage++;
		}   nobreak;

		case 8: {
			if ((m_channels & (1 << TKO_Channel_Diffuse)) != 0) {
				if (m_diffuse.m_name != null) {
					if ((status = PutData (tk, m_diffuse.m_name, (int) strlen (m_diffuse.m_name))) != TK_Normal)
						return status;
				}
				else {
					unsigned char       rgb[3];

					floats_to_bytes (m_diffuse.m_rgb, rgb, 3);

					if ((status = PutData (tk, rgb, 3)) != TK_Normal)
						return status;
				}
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if ((m_channels & (1 << TKO_Channel_Specular)) != 0) {
				unsigned char       byte = 0;   

				if (m_specular.m_name != null) {
					length = (int)strlen (m_specular.m_name);
					if (length > 255)
						return tk.Error(">255 specular texture not yet supported");
					byte = (unsigned char) length;
				}
				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 10: {
			if ((m_channels & (1 << TKO_Channel_Specular)) != 0) {
				if (m_specular.m_name != null) {
					if ((status = PutData (tk, m_specular.m_name, (int) strlen (m_specular.m_name))) != TK_Normal)
						return status;
				}
				else {
					unsigned char       rgb[3];

					floats_to_bytes (m_specular.m_rgb, rgb, 3);

					if ((status = PutData (tk, rgb, 3)) != TK_Normal)
						return status;
				}
			}
			m_stage++;
		}   nobreak;

		case 11: {
			if ((m_channels & (1 << TKO_Channel_Mirror)) != 0) {
				unsigned char       byte = 0;   

				if (m_mirror.m_name != null) {
					length = (int) strlen (m_mirror.m_name);
					if (length > 255)
						return tk.Error(">255 mirror texture not yet supported");
					byte = (unsigned char) length;
				}

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 12: {
			if ((m_channels & (1 << TKO_Channel_Mirror)) != 0) {
				if (m_mirror.m_name != null) {
					if ((status = PutData (tk, m_mirror.m_name, (int) strlen (m_mirror.m_name))) != TK_Normal)
						return status;
				}
				else {
					unsigned char       rgb[3];

					floats_to_bytes (m_mirror.m_rgb, rgb, 3);

					if ((status = PutData (tk, rgb, 3)) != TK_Normal)
						return status;
				}
			}
			m_stage++;
		}   nobreak;

		case 13: {
			if ((m_channels & (1 << TKO_Channel_Transmission)) != 0) {
				unsigned char       byte = 0;   

				if (m_transmission.m_name != null) {
					length = (int) strlen (m_transmission.m_name);
					if (length > 255)
						return tk.Error(">255 transmission texture not yet supported");
					byte = (unsigned char) length;
				}

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 14: {
			if ((m_channels & (1 << TKO_Channel_Transmission)) != 0) {
				if (m_transmission.m_name != null) {
					if ((status = PutData (tk, m_transmission.m_name, (int) strlen (m_transmission.m_name))) != TK_Normal)
						return status;
				}
				else {
					unsigned char       rgb[3];

					floats_to_bytes (m_transmission.m_rgb, rgb, 3);

					if ((status = PutData (tk, rgb, 3)) != TK_Normal)
						return status;
				}
			}
			m_stage++;
		}   nobreak;

		case 15: {
			if ((m_channels & (1 << TKO_Channel_Emission)) != 0) {
				unsigned char       byte = 0;   

				if (m_emission.m_name != null) {
					length = (int) strlen (m_emission.m_name);
					if (length > 255)
						return tk.Error(">255 emission texture not yet supported");
					byte = (unsigned char) length;
				}
				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 16: {
			if ((m_channels & (1 << TKO_Channel_Emission)) != 0) {
				if (m_emission.m_name != null) {
					if ((status = PutData (tk, m_emission.m_name, (int) strlen (m_emission.m_name))) != TK_Normal)
						return status;
				}
				else {
					unsigned char       rgb[3];

					floats_to_bytes (m_emission.m_rgb, rgb, 3);

					if ((status = PutData (tk, rgb, 3)) != TK_Normal)
						return status;
				}
			}
			m_stage++;
		}   nobreak;

		case 17: {
			if ((m_channels & (1 << TKO_Channel_Gloss)) != 0) {
				if ((status = PutData (tk, m_gloss)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 18: {
			if ((m_channels & (1 << TKO_Channel_Index)) != 0) {
				if ((status = PutData (tk, m_index)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 19: {
			if ((m_channels & (1 << TKO_Channel_Environment)) != 0) {
				unsigned char       byte = 0;

				if (m_environment.m_name != null) {
					length = (int) strlen (m_environment.m_name);
					if (length > 255)
						return tk.Error(">255 environment texture not yet supported");
					byte = (unsigned char) length;
				}
				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 20: {
			if ((m_channels & (1 << TKO_Channel_Environment)) != 0) {
				if ((status = PutData (tk, m_environment.m_name, (int) strlen (m_environment.m_name))) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 21: {
			if ((m_channels & (1 << TKO_Channel_Bump)) != 0) {
				unsigned char       byte = 0;

				if (m_bump.m_name != null) {
					length = (int) strlen (m_bump.m_name);
					if (length > 255)
						return tk.Error(">255 bump texture not yet supported");
					byte = (unsigned char) length;
				}

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 22: {
			if ((m_channels & (1 << TKO_Channel_Bump)) != 0) {
				if ((status = PutData (tk, m_bump.m_name, (int) strlen (m_bump.m_name))) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Color::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_mask = byte;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended_Colors) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Colors_Shift;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended2_Shift;
			}

			if (tk.GetVersion() < 1650) {
				if ((m_mask & TKO_Geo_Ambient_Up) != 0)
					m_mask |= TKO_Geo_Ambient_Down;
			}

			m_stage++;
		}   nobreak;

		case 4: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_channels = byte;
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_channels & (1 << TKO_Channel_Extended)) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_channels |= byte << TKO_Channel_Extended_Shift;
			}
			m_progress = -1;
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_channels & (1 << TKO_Channel_Diffuse)) != 0) {
				unsigned char		byte;
				if (m_progress == -1) {
					if ((status = GetData (tk, byte)) != TK_Normal)
						return status;
					m_progress = byte;
				}
				if (m_progress == 255) {
					
					if ((status = GetData (tk, m_progress)) != TK_Normal)
						return status;
				}
				
				if (m_progress != 0)
            		set_channel_name (m_diffuse, m_progress);  
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if ((m_channels & (1 << TKO_Channel_Diffuse)) != 0) {
				if (m_progress != 0) {
					if ((status = GetData (tk, m_diffuse.m_name, m_progress)) != TK_Normal)
						return status;
				}
				else {
					unsigned char       rgb[3];

					if ((status = GetData (tk, rgb, 3)) != TK_Normal)
						return status;

					bytes_to_floats (rgb, m_diffuse.m_rgb, 3);
				}
			}
			m_stage++;
		}   nobreak;

		case 8: {
			if ((m_channels & (1 << TKO_Channel_Specular)) != 0) {
				unsigned char		byte;
				
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;

				if ((m_progress = byte) != 0)
					set_channel_name (m_specular, m_progress);
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if ((m_channels & (1 << TKO_Channel_Specular)) != 0) {
				if (m_progress != 0) {
					if ((status = GetData (tk, m_specular.m_name, m_progress)) != TK_Normal)
						return status;
				}
				else {
					unsigned char       rgb[3];

					if ((status = GetData (tk, rgb, 3)) != TK_Normal)
						return status;

					bytes_to_floats (rgb, m_specular.m_rgb, 3);
				}
			}
			m_stage++;
		}   nobreak;

		case 10: {
			if ((m_channels & (1 << TKO_Channel_Mirror)) != 0) {
				unsigned char		byte;
				
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;

				if ((m_progress = byte) != 0)
					set_channel_name (m_mirror, m_progress);
			}
			m_stage++;
		}   nobreak;

		case 11: {
			if ((m_channels & (1 << TKO_Channel_Mirror)) != 0) {
				if (m_progress != 0) {
					if ((status = GetData (tk, m_mirror.m_name, m_progress)) != TK_Normal)
						return status;
				}
				else {
					unsigned char       rgb[3];

					if ((status = GetData (tk, rgb, 3)) != TK_Normal)
						return status;

					bytes_to_floats (rgb, m_mirror.m_rgb, 3);
				}
			}
			m_stage++;
		}   nobreak;

		case 12: {
			if ((m_channels & (1 << TKO_Channel_Transmission)) != 0) {
				unsigned char		byte;
				
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;

				if ((m_progress = byte) != 0)
					set_channel_name (m_transmission, m_progress);
			}
			m_stage++;
		}   nobreak;

		case 13: {
			if ((m_channels & (1 << TKO_Channel_Transmission)) != 0) {
				if (m_progress != 0) {
					if ((status = GetData (tk, m_transmission.m_name, m_progress)) != TK_Normal)
						return status;
				}
				else {
					unsigned char       rgb[3];

					if ((status = GetData (tk, rgb, 3)) != TK_Normal)
						return status;

					bytes_to_floats (rgb, m_transmission.m_rgb, 3);
				}
			}
			m_stage++;
		}   nobreak;

		case 14: {
			if ((m_channels & (1 << TKO_Channel_Emission)) != 0) {
				unsigned char		byte;
				
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;

				if ((m_progress = byte) != 0)
					set_channel_name (m_emission, m_progress);
			}
			m_stage++;
		}   nobreak;

		case 15: {
			if ((m_channels & (1 << TKO_Channel_Emission)) != 0) {
				if (m_progress != 0) {
					if ((status = GetData (tk, m_emission.m_name, m_progress)) != TK_Normal)
						return status;
				}
				else {
					unsigned char       rgb[3];

					if ((status = GetData (tk, rgb, 3)) != TK_Normal)
						return status;

					bytes_to_floats (rgb, m_emission.m_rgb, 3);
				}
			}
			m_stage++;
		}   nobreak;

		case 16: {
			if ((m_channels & (1 << TKO_Channel_Gloss)) != 0) {
				if ((status = GetData (tk, m_gloss)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 17: {
			if ((m_channels & (1 << TKO_Channel_Index)) != 0) {
				if ((status = GetData (tk, m_index)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 18: {
			if ((m_channels & (1 << TKO_Channel_Environment)) != 0) {
				unsigned char		byte;
				
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;

				m_progress = byte;
				set_channel_name (m_environment, m_progress);
			}
			m_stage++;
		}   nobreak;

		case 19: {
			if ((m_channels & (1 << TKO_Channel_Environment)) != 0) {
				if ((status = GetData (tk, m_environment.m_name, m_progress)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 20: {
			if ((m_channels & (1 << TKO_Channel_Bump)) != 0) {
				unsigned char		byte;
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;

				m_progress = byte;
				set_channel_name (m_bump, m_progress);
			}
			m_stage++;
		}   nobreak;

		case 21: {
			if ((m_channels & (1 << TKO_Channel_Bump)) != 0) {
				if ((status = GetData (tk, m_bump.m_name, m_progress)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Color::Reset () alter {
	m_mask = 0;
	m_channels = 0;

	m_diffuse.Reset();
	m_specular.Reset();
	m_mirror.Reset();
	m_transmission.Reset();
	m_emission.Reset();
	m_environment.Reset();
	m_bump.Reset();

	BBaseOpcodeHandler::Reset();
}


void TK_Color::set_channel_name (channel alter & c, int length, int which_channel) alter {
	::delete [] c.m_name;
	c.m_name = ::new char [length + 1];
	c.m_name[length] = '\0';
	if (which_channel != -1)
		m_channels |= (1 << which_channel);
}
void TK_Color::set_channel_name (channel alter & c, char const * name, int which_channel) alter {
	if (name != null) {
		set_channel_name (c, (int) strlen (name), which_channel);
		strcpy (c.m_name, name);
	}
	else {
		delete [] c.m_name;
		if (which_channel != -1)
			m_channels &= ~(1 << which_channel);
		c.m_name = null;
	}
}




TK_Status TK_Color_RGB::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1550)
		m_mask &= ~TKO_Geo_Simple_Reflection;
	if (tk.GetTargetVersion() < 1650)
		m_mask &= ~TKO_Geo_Ambient_Down;

	if ((m_mask & TKO_Geo_Extended_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended;
	if ((m_mask & TKO_Geo_Extended_Colors_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended_Colors;
	if ((m_mask & TKO_Geo_Extended_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended;
	if (!m_mask)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte = (unsigned char)(m_mask & 0x000000FF);

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended_Colors) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended_Colors_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended2_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			unsigned char       rgb[3];

			floats_to_bytes (m_rgb, rgb, 3);

			if ((status = PutData (tk, rgb, 3)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Color_RGB::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_mask = byte;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended_Colors) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Colors_Shift;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended2_Shift;
			}

			if (tk.GetVersion() < 1650) {
				if ((m_mask & TKO_Geo_Ambient_Up) != 0)
					m_mask |= TKO_Geo_Ambient_Down;
			}

			m_stage++;
		}   nobreak;

		case 4: {
			unsigned char       rgb[3];

			if ((status = GetData (tk, rgb, 3)) != TK_Normal)
				return status;

			bytes_to_floats (rgb, m_rgb, 3);
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}





TK_Status TK_Color_By_Value::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1550)
		m_mask &= ~TKO_Geo_Simple_Reflection;
	if (tk.GetTargetVersion() < 1650)
		m_mask &= ~TKO_Geo_Ambient_Down;

	if ((m_mask & TKO_Geo_Extended_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended;
	if ((m_mask & TKO_Geo_Extended_Colors_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended_Colors;
	if ((m_mask & TKO_Geo_Extended_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended;
	if (!m_mask)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte = (unsigned char)(m_mask & 0x000000FF);

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended_Colors) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended_Colors_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended2_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((status = PutData (tk, m_space)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 6: {
			if ((status = PutData (tk, m_value, 3)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Color_By_Value::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_mask = byte;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended_Colors) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Colors_Shift;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended2_Shift;
			}

			if (tk.GetVersion() < 1650) {
				if ((m_mask & TKO_Geo_Ambient_Up) != 0)
					m_mask |= TKO_Geo_Ambient_Down;
			}

			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = GetData (tk, m_space)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			if ((status = GetData (tk, m_value, 3)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}




TK_Status TK_Color_By_Index::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1550)
		m_mask &= ~TKO_Geo_Simple_Reflection;
	if (tk.GetTargetVersion() < 1650)
		m_mask &= ~TKO_Geo_Ambient_Down;

	if ((m_mask & TKO_Geo_Extended_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended;
	if ((m_mask & TKO_Geo_Extended_Colors_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended_Colors;
	if ((m_mask & TKO_Geo_Extended_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended;
	if (!m_mask)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte = (unsigned char)(m_mask & 0x000000FF);

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended_Colors) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended_Colors_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended2_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			switch (Opcode()) {
				case TKE_Color_By_Index: {
					unsigned char       byte = (unsigned char)m_index;

					if ((status = PutData (tk, byte)) != TK_Normal)
						return status;

					m_index = byte;
				}   break;

				case TKE_Color_By_Index_16: {
					unsigned short      word = (unsigned short)m_index;

					if ((status = PutData (tk, word)) != TK_Normal)
						return status;

					m_index = word;
				}   break;

				default:
					return tk.Error();
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Color_By_Index::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_mask = byte;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended_Colors) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Colors_Shift;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended2_Shift;
			}

			if (tk.GetVersion() < 1650) {
				if ((m_mask & TKO_Geo_Ambient_Up) != 0)
					m_mask |= TKO_Geo_Ambient_Down;
			}

			m_stage++;
		}   nobreak;

		case 4: {
			switch (Opcode()) {
				case TKE_Color_By_Index: {
					unsigned char       byte;

					if ((status = GetData (tk, byte)) != TK_Normal)
						return status;

					m_index = byte;
				}   break;

				case TKE_Color_By_Index_16: {
					unsigned short      word;

					if ((status = GetData (tk, word)) != TK_Normal)
						return status;

					m_index = word;
				}   break;

				default:
					return tk.Error();
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}




TK_Status TK_Color_By_FIndex::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1550)
		m_mask &= ~TKO_Geo_Simple_Reflection;
	if (tk.GetTargetVersion() < 1650)
		m_mask &= ~TKO_Geo_Ambient_Down;

	if ((m_mask & TKO_Geo_Extended_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended;
	if ((m_mask & TKO_Geo_Extended_Colors_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended_Colors;
	if ((m_mask & TKO_Geo_Extended_Mask) == 0)
		m_mask &= ~TKO_Geo_Extended;
	if (!m_mask)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte = (unsigned char)(m_mask & 0x000000FF);

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended_Colors) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended_Colors_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char       byte = (unsigned char)((m_mask>>TKO_Geo_Extended2_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((status = PutData (tk, m_index)) != TK_Normal)
				return status;

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Color_By_FIndex::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_mask = byte;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended_Colors) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Colors_Shift;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended2_Shift;
			}

			if (tk.GetVersion() < 1650) {
				if ((m_mask & TKO_Geo_Ambient_Up) != 0)
					m_mask |= TKO_Geo_Ambient_Down;
			}

			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = GetData (tk, m_index)) != TK_Normal)
				return status;

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}




TK_Status TK_Color_Map::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

	if (m_format == TKO_Map_String && tk.GetTargetVersion() < 1345)
		return tk.Error ("String Color Maps not supported in this file version");

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_format)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if (m_format == TKO_Map_RGB_Values) {
				unsigned char *		values = new unsigned char [3*m_length];

				floats_to_bytes (m_values, values, 3*m_length);

				if ((status = PutData (tk, values, 3*m_length)) != TK_Normal)
					return status;

				delete [] values;
			}
			else {
				if ((status = PutData (tk, m_string, m_length)) != TK_Normal)
					return status;
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Color_Map::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_format)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_length)) != TK_Normal)
				return status;

			if (m_format == TKO_Map_String)
				SetString (m_length);
			else {
				
				
				SetValues (m_length);
			}

			m_stage++;
		}   nobreak;

		case 2: {
			if (m_format == TKO_Map_RGB_Values) {
				unsigned char *		values = new unsigned char [3*m_length];

				if ((status = GetData (tk, values, 3*m_length)) != TK_Normal)
					return status;

				bytes_to_floats (values, m_values, 3*m_length);

				delete [] values;
			}
			else {
				if ((status = GetData (tk, m_string, m_length)) != TK_Normal)
					return status;
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Color_Map::set_values (int length, float const * values) alter {
	delete [] m_values;
	m_length = length;
	m_values = new float [3*m_length];
	if (values != null)
		memcpy (m_values, values, length*3*sizeof(float));
}

void TK_Color_Map::SetString (int length) alter {
	delete [] m_string;
	m_length = length;
	m_string = new char [m_length + 1];
	m_string[m_length] = '\0';
}

void TK_Color_Map::SetString (char const * string) alter {
	SetString ((int) strlen (string));
	strcpy (m_string, string);
}

TK_Color_Map::~TK_Color_Map () {
	delete [] m_string;
}

void TK_Color_Map::Reset () {
	delete [] m_values;
	m_values = null;
	delete [] m_string;
	m_string = null;
	m_format = TKO_Map_RGB_Values;
	BBaseOpcodeHandler::Reset();
}



TK_Callback::~TK_Callback() {
	delete [] m_string;
}

TK_Status TK_Callback::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if (m_length > 0) {
				if ((status = PutOpcode (tk)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_length > 0) {
				
				short               word = (short)m_length;

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_length > 0) {
				if ((status = PutData (tk, m_string, m_length)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Callback::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
	short           word;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, word)) != TK_Normal)
				return status;
			set_callback ((int)word);
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_string, m_length)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Callback::set_callback (int length) alter {
	m_length = length;
	delete [] m_string;
	m_string = new char [m_length + 1];
	m_string[m_length] = '\0';
}

void TK_Callback::set_callback (char const * callback) alter {
	set_callback ((int) strlen (callback));
	strcpy (m_string, callback);
}

void TK_Callback::Reset () alter {
	delete [] m_string;
	m_string = null;
	m_length = 0;

	BBaseOpcodeHandler::Reset();
}



#if 0
TK_Status TK_Radiosity_RayTrace_Options::Write (BStreamFileToolkit & tk) alter {
	return TK_Normal;
}


TK_Status TK_Radiosity_RayTrace_Options::Read (BStreamFileToolkit & tk) alter {
	return TK_Normal;
}
#endif



TK_Rendering_Options::TK_Rendering_Options () 
	: BBaseOpcodeHandler (TKE_Rendering_Options) 
{ 
	m_gooch_color_map_segment=0;
	Reset();
}
                                


TK_Rendering_Options::~TK_Rendering_Options () {
	
	delete [] m_gooch_color_map_segment;
#if 0
	delete m_rrt;
#endif
}

void TK_Rendering_Options::Reset () alter {
#if 0
	delete m_rrt;
	m_rrt = null;
#endif
	m_mask[0] = m_mask[1] = m_value[0] = m_value[1] = 0;
	m_nurbs_options_mask = m_nurbs_options_value = 0;
	m_tessellations = 0;
	m_hlr_options = 0;
	m_general_displacement=0;
	m_join_cutoff_angle=0;
	delete [] m_gooch_color_map_segment;
	m_gooch_color_map_segment = 0;
	m_gooch_color_map_segment_length = 0;
	
	BBaseOpcodeHandler::Reset();
}

TK_Status TK_Rendering_Options::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;
	int             mask = m_mask[0], emask = m_mask[1], e2mask = m_mask[2];
	int             nurbs_mask = m_nurbs_options_mask;
	int             hlr_mask = m_hlr_options;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 650) {
		
		mask &= ~(TKO_Rendo_Buffer_Options          | TKO_Rendo_Hidden_Line_Options     |
				  TKO_Rendo_LOD                     | TKO_Rendo_LOD_Options             |
				  TKO_Rendo_NURBS_Curve_Options     | TKO_Rendo_NURBS_Surface_Options   |
				  TKO_Rendo_Stereo                  | TKO_Rendo_Stereo_Separation       |
				  TKO_Rendo_Extended);
		emask = 0;
	}
	if (tk.GetTargetVersion() < 710)
		emask &= ~(TKO_Rendo_Transparency_Style | TKO_Rendo_Transparency_Hardware);
	if (tk.GetTargetVersion() < 806) {
		emask &= ~(TKO_Rendo_Cut_Geometry|TKO_Rendo_Depth_Range);
		nurbs_mask &= ~(TKO_NURBS_Surface_Max_Facet_Width|
						TKO_NURBS_Extended|
						TKO_NURBS_Surface_Max_Facet_Angle|
						TKO_NURBS_Surface_Max_Facet_Deviation|
						TKO_NURBS_Surface_Max_Trim_Curve_Deviation);
	}
	if (tk.GetTargetVersion() < 1001)
		emask &= ~(TKO_Rendo_Mask_Transform|TKO_Rendo_Image_Scale);
	if (tk.GetTargetVersion() < 1002)
		emask &= ~(TKO_Rendo_Mask_Transform|TKO_Rendo_Simple_Shadow);
	if (tk.GetTargetVersion() < 1150)
		hlr_mask &= ~(TKO_Hidden_Line_Extended2_Mask|TKO_Hidden_Line_Extended2);
	if (tk.GetTargetVersion() < 1205)
		emask &= ~(TKO_Rendo_Geometry_Options);
	if (tk.GetTargetVersion() < 1210)
		emask &= ~(TKO_Rendo_Image_Tint);
	if (tk.GetTargetVersion() < 1310)
		emask &= ~(TKO_Rendo_Transparency_Options);
	if (tk.GetTargetVersion() < 1335)
		emask &= ~(TKO_Rendo_General_Displacement);
	if (tk.GetTargetVersion() < 1405)
		emask &= ~(TKO_Rendo_Join_Cutoff_Angle);
	if (tk.GetTargetVersion() < 1505)
		emask &= ~(TKO_Rendo_Screen_Range);
	if (tk.GetTargetVersion() < 1510)
		emask &= ~(TKO_Rendo_Stereo_Distance);
	if (tk.GetTargetVersion() < 1515)
		emask &= ~(TKO_Rendo_Shadow_Map|TKO_Rendo_Simple_Reflection);
	if (tk.GetTargetVersion() < 1615)
		emask &= ~TKO_Rendo_Antialias;
	if (tk.GetTargetVersion() < 1650)
		emask &= ~(TKO_Rendo_Ambient_Up_Vector|TKO_Rendo_Gooch_Color_Map|
					TKO_Rendo_Gooch_Color_Range|TKO_Rendo_Gooch_Diffuse_Weight|
					TKO_Interp_Index_Markers);
	if (tk.GetTargetVersion() < 1650)
		m_shadow_map &= ~TKO_Shadow_Map_Extended_Mask;
	if (tk.GetTargetVersion() < 1770) {
		
		if (tk.GetTargetVersion() < 1715 || tk.GetTargetVersion() >= 1750)
			emask &= ~TKO_Rendo_Vertex_Displacement;
	}
	if (tk.GetTargetVersion() < 1710)
		e2mask &= ~TKO_Rendo_Forced_Lock;

	if (e2mask == 0)
		emask &= ~TKO_Rendo_Extended2;
	else
		emask |= TKO_Rendo_Extended2;

	if (emask == 0)
		mask &= ~TKO_Rendo_Extended;
	else
		mask |= TKO_Rendo_Extended;

	if (tk.GetTargetVersion() < 1550)
		m_lock_color_value &= ~TKO_Geo_Simple_Reflection;
	if (tk.GetTargetVersion() < 1650)
		m_lock_color_value &= ~TKO_Geo_Ambient_Down;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_progress = 0;     
			m_stage++;
		}   nobreak;


		case 1: {
			if ((status = PutData (tk, mask)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((mask & TKO_Rendo_Extended) != 0) {
				if ((status = PutData (tk, emask)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((emask & TKO_Rendo_Extended2) != 0) {
				if ((status = PutData (tk, e2mask)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = PutData (tk, m_value[0])) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			if ((mask & TKO_Rendo_Extended) != 0) {
				if ((status = PutData (tk, m_value[1])) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if ((emask & TKO_Rendo_Extended2) != 0) {
				if ((status = PutData (tk, m_value[2])) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;


		case 7: {
			if ((mask & TKO_Rendo_Any_HSR) != 0) {
				if ((status = PutData (tk, m_hsr)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 8: {
			if ((mask & TKO_Rendo_TQ) != 0) {
				if ((status = PutData (tk, m_tq)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if ((mask & m_value[0] & TKO_Rendo_Face_Displacement) != 0) {
				if ((status = PutData (tk, m_face_displacement)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 10: {
			if ((mask & m_value[0] & TKO_Rendo_Attribute_Lock) != 0) {
				if ((status = PutData (tk, m_lock_mask)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 11: {
			if ((mask & m_value[0] & TKO_Rendo_Attribute_Lock) != 0) {
				if ((status = PutData (tk, m_lock_value)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 12: {
			if ((mask & m_value[0] & TKO_Rendo_Attribute_Lock) != 0 &&
				(m_lock_mask & m_lock_value & TKO_Lock_Color) != 0) {
				if ((status = PutData (tk, m_lock_color_mask)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 13: {
			if ((mask & m_value[0] & TKO_Rendo_Attribute_Lock) != 0 &&
				(m_lock_mask & m_lock_value & TKO_Lock_Color) != 0) {
				if (tk.GetTargetVersion() <= 1100) {
					if ((status = PutData (tk, m_lock_color_value)) != TK_Normal)
						return status;
				}
				else switch (m_progress) {
					case 0:
						if ((status = PutData (tk, m_lock_color_value)) != TK_Normal)
							return status;
						m_progress++;

					case 1:
						if ((m_lock_color_value & (TKO_Geo_Face|TKO_Geo_Front)) != 0)
							if ((status = PutData (tk, m_lock_color_face_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 2:
						if ((m_lock_color_value & (TKO_Geo_Face|TKO_Geo_Front)) != 0)
							if ((status = PutData (tk, m_lock_color_face_value)) != TK_Normal)
								return status;
						m_progress++;

					case 3:
						if ((m_lock_color_value & TKO_Geo_Back) != 0)
							if ((status = PutData (tk, m_lock_color_back_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 4:
						if ((m_lock_color_value & TKO_Geo_Back) != 0)
							if ((status = PutData (tk, m_lock_color_back_value)) != TK_Normal)
								return status;
						m_progress++;

					case 5:
						if ((m_lock_color_value & TKO_Geo_Edge) != 0)
							if ((status = PutData (tk, m_lock_color_edge_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 6:
						if ((m_lock_color_value & TKO_Geo_Edge) != 0)
							if ((status = PutData (tk, m_lock_color_edge_value)) != TK_Normal)
								return status;
						m_progress++;

					case 7:
						if ((m_lock_color_value & TKO_Geo_Line) != 0)
							if ((status = PutData (tk, m_lock_color_line_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 8:
						if ((m_lock_color_value & TKO_Geo_Line) != 0)
							if ((status = PutData (tk, m_lock_color_line_value)) != TK_Normal)
								return status;
						m_progress++;

					case 9:
						if ((m_lock_color_value & TKO_Geo_Text) != 0)
							if ((status = PutData (tk, m_lock_color_text_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 10:
						if ((m_lock_color_value & TKO_Geo_Text) != 0)
							if ((status = PutData (tk, m_lock_color_text_value)) != TK_Normal)
								return status;
						m_progress++;

					case 11:
						if ((m_lock_color_value & TKO_Geo_Marker) != 0)
							if ((status = PutData (tk, m_lock_color_marker_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 12:
						if ((m_lock_color_value & TKO_Geo_Marker) != 0)
							if ((status = PutData (tk, m_lock_color_marker_value)) != TK_Normal)
								return status;
						m_progress++;

					case 13:
						if ((m_lock_color_value & TKO_Geo_Vertex) != 0)
							if ((status = PutData (tk, m_lock_color_vertex_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 14:
						if ((m_lock_color_value & TKO_Geo_Vertex) != 0)
							if ((status = PutData (tk, m_lock_color_vertex_value)) != TK_Normal)
								return status;
						m_progress++;

					case 15:
						if ((m_lock_color_value & TKO_Geo_Window) != 0)
							if ((status = PutData (tk, m_lock_color_window_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 16:
						if ((m_lock_color_value & TKO_Geo_Window) != 0)
							if ((status = PutData (tk, m_lock_color_window_value)) != TK_Normal)
								return status;
						m_progress++;

					case 17:
						if ((m_lock_color_value & TKO_Geo_Face_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_face_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 18:
						if ((m_lock_color_value & TKO_Geo_Face_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_face_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 19:
						if ((m_lock_color_value & TKO_Geo_Edge_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_edge_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 20:
						if ((m_lock_color_value & TKO_Geo_Edge_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_edge_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 21:
						if ((m_lock_color_value & TKO_Geo_Line_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_line_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 22:
						if ((m_lock_color_value & TKO_Geo_Line_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_line_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 23:
						if ((m_lock_color_value & TKO_Geo_Text_Contrast) != 0 ||
							tk.GetTargetVersion() < 1555 && (m_lock_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = PutData (tk, m_lock_color_text_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 24:
						if ((m_lock_color_value & TKO_Geo_Text_Contrast) != 0 ||
							tk.GetTargetVersion() < 1555 && (m_lock_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = PutData (tk, m_lock_color_text_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 25:
						if ((m_lock_color_value & TKO_Geo_Marker_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_marker_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 26:
						if ((m_lock_color_value & TKO_Geo_Marker_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_marker_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 27:
						if ((m_lock_color_value & TKO_Geo_Vertex_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_vertex_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 28:
						if ((m_lock_color_value & TKO_Geo_Vertex_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_vertex_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 29:
						if ((m_lock_color_value & TKO_Geo_Window_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_window_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 30:
						if ((m_lock_color_value & TKO_Geo_Window_Contrast) != 0)
							if ((status = PutData (tk, m_lock_color_window_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 31:
						if ((m_lock_color_value & TKO_Geo_Simple_Reflection) != 0)
							if ((status = PutData (tk, m_lock_color_simple_reflection_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 32:
						if ((m_lock_color_value & TKO_Geo_Simple_Reflection) != 0)
							if ((status = PutData (tk, m_lock_color_simple_reflection_value)) != TK_Normal)
								return status;
						m_progress++;

					case 33:
						if ((m_lock_color_value & TKO_Geo_Cut_Face) != 0)
							if ((status = PutData (tk, m_lock_color_cut_face_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 34:
						if ((m_lock_color_value & TKO_Geo_Cut_Face) != 0)
							if ((status = PutData (tk, m_lock_color_cut_face_value)) != TK_Normal)
								return status;
						m_progress++;

					case 35:
						if ((m_lock_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = PutData (tk, m_lock_color_cut_edge_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 36:
						if ((m_lock_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = PutData (tk, m_lock_color_cut_edge_value)) != TK_Normal)
								return status;
						m_progress = 0;
						break;

					default:
						return tk.Error ("lost track processing color locks");
				}
			}
			m_stage++;
		}   nobreak;

		case 14: {
			if ((mask & m_value[0] & TKO_Rendo_Attribute_Lock) != 0 &&
				(m_lock_mask & m_lock_value & TKO_Lock_Visibility) != 0) {
				switch (m_progress) {
					case 0:
						if ((status = PutData (tk, m_lock_visibility_mask)) != TK_Normal)
							return status;
						m_progress++;

					case 1:
						if ((status = PutData (tk, m_lock_visibility_value)) != TK_Normal)
							return status;
						m_progress = 0;
						break;

					default:
						return tk.Error ("lost track processing visibility locks");
				}
			}
			m_stage++;
		}   nobreak;


		
		case 15: {
			if ((e2mask & m_value[2] & TKO_Rendo_Forced_Lock) != 0) {
				if ((status = PutData (tk, m_forced_mask)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 16: {
			if ((e2mask & m_value[2] & TKO_Rendo_Forced_Lock) != 0) {
				if ((status = PutData (tk, m_forced_value)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 17: {
			if ((e2mask & m_value[2] & TKO_Rendo_Forced_Lock) != 0 &&
				(m_forced_mask & m_forced_value & TKO_Lock_Color) != 0) {
				if ((status = PutData (tk, m_forced_color_mask)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 18: {
			if ((e2mask & m_value[2] & TKO_Rendo_Forced_Lock) != 0 &&
				(m_forced_mask & m_forced_value & TKO_Lock_Color) != 0) {
				if (tk.GetTargetVersion() <= 1100) {
					if ((status = PutData (tk, m_forced_color_value)) != TK_Normal)
						return status;
				}
				else switch (m_progress) {
					case 0:
						if ((status = PutData (tk, m_forced_color_value)) != TK_Normal)
							return status;
						m_progress++;

					case 1:
						if ((m_forced_color_value & (TKO_Geo_Face|TKO_Geo_Front)) != 0)
							if ((status = PutData (tk, m_forced_color_face_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 2:
						if ((m_forced_color_value & (TKO_Geo_Face|TKO_Geo_Front)) != 0)
							if ((status = PutData (tk, m_forced_color_face_value)) != TK_Normal)
								return status;
						m_progress++;

					case 3:
						if ((m_forced_color_value & TKO_Geo_Back) != 0)
							if ((status = PutData (tk, m_forced_color_back_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 4:
						if ((m_forced_color_value & TKO_Geo_Back) != 0)
							if ((status = PutData (tk, m_forced_color_back_value)) != TK_Normal)
								return status;
						m_progress++;

					case 5:
						if ((m_forced_color_value & TKO_Geo_Edge) != 0)
							if ((status = PutData (tk, m_forced_color_edge_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 6:
						if ((m_forced_color_value & TKO_Geo_Edge) != 0)
							if ((status = PutData (tk, m_forced_color_edge_value)) != TK_Normal)
								return status;
						m_progress++;

					case 7:
						if ((m_forced_color_value & TKO_Geo_Line) != 0)
							if ((status = PutData (tk, m_forced_color_line_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 8:
						if ((m_forced_color_value & TKO_Geo_Line) != 0)
							if ((status = PutData (tk, m_forced_color_line_value)) != TK_Normal)
								return status;
						m_progress++;

					case 9:
						if ((m_forced_color_value & TKO_Geo_Text) != 0)
							if ((status = PutData (tk, m_forced_color_text_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 10:
						if ((m_forced_color_value & TKO_Geo_Text) != 0)
							if ((status = PutData (tk, m_forced_color_text_value)) != TK_Normal)
								return status;
						m_progress++;

					case 11:
						if ((m_forced_color_value & TKO_Geo_Marker) != 0)
							if ((status = PutData (tk, m_forced_color_marker_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 12:
						if ((m_forced_color_value & TKO_Geo_Marker) != 0)
							if ((status = PutData (tk, m_forced_color_marker_value)) != TK_Normal)
								return status;
						m_progress++;

					case 13:
						if ((m_forced_color_value & TKO_Geo_Vertex) != 0)
							if ((status = PutData (tk, m_forced_color_vertex_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 14:
						if ((m_forced_color_value & TKO_Geo_Vertex) != 0)
							if ((status = PutData (tk, m_forced_color_vertex_value)) != TK_Normal)
								return status;
						m_progress++;

					case 15:
						if ((m_forced_color_value & TKO_Geo_Window) != 0)
							if ((status = PutData (tk, m_forced_color_window_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 16:
						if ((m_forced_color_value & TKO_Geo_Window) != 0)
							if ((status = PutData (tk, m_forced_color_window_value)) != TK_Normal)
								return status;
						m_progress++;

					case 17:
						if ((m_forced_color_value & TKO_Geo_Face_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_face_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 18:
						if ((m_forced_color_value & TKO_Geo_Face_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_face_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 19:
						if ((m_forced_color_value & TKO_Geo_Edge_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_edge_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 20:
						if ((m_forced_color_value & TKO_Geo_Edge_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_edge_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 21:
						if ((m_forced_color_value & TKO_Geo_Line_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_line_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 22:
						if ((m_forced_color_value & TKO_Geo_Line_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_line_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 23:
						if ((m_forced_color_value & TKO_Geo_Text_Contrast) != 0 ||
							tk.GetTargetVersion() < 1555 && (m_forced_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = PutData (tk, m_forced_color_text_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 24:
						if ((m_forced_color_value & TKO_Geo_Text_Contrast) != 0 ||
							tk.GetTargetVersion() < 1555 && (m_forced_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = PutData (tk, m_forced_color_text_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 25:
						if ((m_forced_color_value & TKO_Geo_Marker_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_marker_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 26:
						if ((m_forced_color_value & TKO_Geo_Marker_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_marker_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 27:
						if ((m_forced_color_value & TKO_Geo_Vertex_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_vertex_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 28:
						if ((m_forced_color_value & TKO_Geo_Vertex_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_vertex_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 29:
						if ((m_forced_color_value & TKO_Geo_Window_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_window_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 30:
						if ((m_forced_color_value & TKO_Geo_Window_Contrast) != 0)
							if ((status = PutData (tk, m_forced_color_window_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 31:
						if ((m_forced_color_value & TKO_Geo_Simple_Reflection) != 0)
							if ((status = PutData (tk, m_forced_color_simple_reflection_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 32:
						if ((m_forced_color_value & TKO_Geo_Simple_Reflection) != 0)
							if ((status = PutData (tk, m_forced_color_simple_reflection_value)) != TK_Normal)
								return status;
						m_progress++;

					case 33:
						if ((m_forced_color_value & TKO_Geo_Cut_Face) != 0)
							if ((status = PutData (tk, m_forced_color_cut_face_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 34:
						if ((m_forced_color_value & TKO_Geo_Cut_Face) != 0)
							if ((status = PutData (tk, m_forced_color_cut_face_value)) != TK_Normal)
								return status;
						m_progress++;

					case 35:
						if ((m_forced_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = PutData (tk, m_forced_color_cut_edge_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 36:
						if ((m_forced_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = PutData (tk, m_forced_color_cut_edge_value)) != TK_Normal)
								return status;
						m_progress = 0;
						break;

					default:
						return tk.Error ("lost track processing color forceds");
				}
			}
			m_stage++;
		}   nobreak;

		case 19: {
			if ((e2mask & m_value[2] & TKO_Rendo_Forced_Lock) != 0 &&
				(m_forced_mask & m_forced_value & TKO_Lock_Visibility) != 0) {
				switch (m_progress) {
					case 0:
						if ((status = PutData (tk, m_forced_visibility_mask)) != TK_Normal)
							return status;
						m_progress++;

					case 1:
						if ((status = PutData (tk, m_forced_visibility_value)) != TK_Normal)
							return status;
						m_progress = 0;
						break;

					default:
						return tk.Error ("lost track processing visibility forceds");
				}
			}
			m_stage++;
		}   nobreak;


		case 20: {
			if ((mask & m_value[0] & TKO_Rendo_Fog) != 0) {
				if ((status = PutData (tk, m_fog_limits, 2)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 21: {
			if ((mask & m_value[0] & TKO_Rendo_Debug) != 0) {
				if ((status = PutData (tk, m_debug)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;


		
		case 22: {
			if ((mask & m_value[0] & TKO_Rendo_Stereo_Separation) != 0) {
				if ((status = PutData (tk, m_stereo_separation)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 23: {
			if ((emask & m_value[1] & (TKO_Rendo_Stereo_Distance)) != 0) {
				if ((status = PutData (tk, m_stereo_distance)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 24: {
			if ((mask & m_value[0] & TKO_Rendo_Buffer_Options) != 0) {
				switch (m_progress) {
					case 0:
						if ((status = PutData (tk, m_buffer_options_mask)) != TK_Normal)
							return status;
						m_progress++;
					case 1:
						if ((status = PutData (tk, m_buffer_options_value)) != TK_Normal)
							return status;
						m_progress++;
					case 2:
						if ((m_buffer_options_mask & m_buffer_options_value & TKO_Buffer_Size_Limit) != 0) {
							if ((status = PutData (tk, m_buffer_size_limit)) != TK_Normal)
								return status;
						}
						m_progress = 0;
				}
			}
			m_stage++;
		}   nobreak;

		
		case 25: {
			if ((mask & m_value[0] & TKO_Rendo_Hidden_Line_Options) != 0) {
				switch (m_progress) {
					case 0: {
						unsigned char       byte = (unsigned char)(hlr_mask & 0xff);

						if ((status = PutData (tk, byte)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((hlr_mask & TKO_Hidden_Line_Extended) != 0) {
							unsigned char       byte = (unsigned char)(hlr_mask >> TKO_Hidden_Line_Extended_Shift);

							if ((status = PutData (tk, byte)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 2: {
						if ((hlr_mask & TKO_Hidden_Line_Extended2) != 0) {
							unsigned short      word = (unsigned short)(hlr_mask >> TKO_Hidden_Line_Extended2_Shift);

							if ((status = PutData (tk, word)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((hlr_mask & TKO_Hidden_Line_Pattern) != 0) {
							if ((status = PutData (tk, m_hlr_line_pattern)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 4: {
						if ((hlr_mask & TKO_Hidden_Line_Dim_Factor) != 0) {
							if ((status = PutData (tk, m_hlr_dim_factor)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 5: {
						if ((hlr_mask & TKO_Hidden_Line_Face_Displacement) != 0) {
							if ((status = PutData (tk, m_hlr_face_displacement)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 6: {
						if ((hlr_mask & TKO_Hidden_Line_Color) != 0) {
							if ((status = PutData (tk, m_hlr_color, 3)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 7: {
						if ((hlr_mask & TKO_Hidden_Line_Weight) != 0) {
							if ((status = PutData (tk, m_hlr_weight)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 8: {
						if ((hlr_mask & TKO_Hidden_Line_Weight) != 0 &&
							m_hlr_weight != -1.0f) {
							if ((status = PutData (tk, m_hlr_weight_units)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 9: {
						if ((hlr_mask & TKO_Hidden_Line_HSR_Algorithm) != 0) {
							if ((status = PutData (tk, m_hlr_hsr_algorithm)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;


		
		case 26: {
			if (mask & TKO_Rendo_NURBS_Options) {
				switch (m_progress) {
					case 0: {
						unsigned char       byte = (unsigned char)(nurbs_mask & 0xff);

						if ((status = PutData (tk, byte)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((nurbs_mask & TKO_NURBS_Extended)) {
							unsigned char       byte = (unsigned char)((m_nurbs_options_mask>>TKO_NURBS_Extended_Shift) & 0xff);

							if ((status = PutData (tk, byte)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 2: {
						unsigned char       byte = (unsigned char)(m_nurbs_options_value & 0xff);

						if ((status = PutData (tk, byte)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 3: {
						if ((nurbs_mask & TKO_NURBS_Extended)) {
							unsigned char       byte = (unsigned char)((m_nurbs_options_value>>TKO_NURBS_Extended_Shift) & 0xff);

							if ((status = PutData (tk, byte)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 4: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Curve_Options) &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Curve_Budget)) {
							if ((status = PutData (tk, m_curve_budget)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 5: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Curve_Options) != 0 &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Curve_Continued_Budget) != 0) {
							if ((status = PutData (tk, m_curve_continued_budget)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 6: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Surface_Budget) != 0) {
							if ((status = PutData (tk, m_surface_budget)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 7: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Surface_Trim_Budget) != 0) {
							if ((status = PutData (tk, m_surface_trim_budget)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 8: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Surface_Max_Trim_Curve_Deviation) != 0) {
							if ((status = PutData (tk, m_surface_max_trim_curve_deviation)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 9: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Surface_Max_Facet_Angle) != 0) {
							if ((status = PutData (tk, m_surface_max_facet_angle)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 10: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Surface_Max_Facet_Deviation) != 0) {
							if ((status = PutData (tk, m_surface_max_facet_deviation)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 11: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Surface_Max_Facet_Width) != 0) {
							if ((status = PutData (tk, m_surface_max_facet_width)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 12: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Curve_Options) != 0 &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Curve_Max_Angle) != 0) {
							if ((status = PutData (tk, m_curve_max_angle)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 13: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Curve_Options) != 0 &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Curve_Max_Deviation) != 0) {
							if ((status = PutData (tk, m_curve_max_deviation)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 14: {
						if ((mask & m_value[0] & TKO_Rendo_NURBS_Curve_Options) != 0 &&
							(nurbs_mask & m_nurbs_options_value & TKO_NURBS_Curve_Max_Length) != 0) {
							if ((status = PutData (tk, m_curve_max_length)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;


		
		case 27: {
			if ((mask & m_value[0] & TKO_Rendo_LOD_Options) != 0) {
				switch (m_progress) {
					case 0: {
						if ((status = PutData (tk, m_lod_options_mask)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((status = PutData (tk, m_lod_options_value)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 2: {
						if ((m_lod_options_mask & TKO_LOD_Algorithm) != 0) {
							if ((status = PutData (tk, m_lod_algorithm)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Bounding_Explicit) != 0) {
							if ((status = PutData (tk, m_bounding, 6)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 4: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Calculation_Cutoff) != 0) {
							if ((status = PutData (tk, m_num_cutoffs)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 5: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Calculation_Cutoff) != 0) {
							if ((status = PutData (tk, m_cutoff, m_num_cutoffs)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 6: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Clamp) != 0) {
							if ((status = PutData (tk, m_clamp)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 7: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Fallback) != 0) {
							if (tk.GetTargetVersion() < 1315 && 
								m_fallback == TKO_LOD_Fallback_Bounding_None)
								m_fallback = TKO_LOD_Fallback_Bounding;
							if ((status = PutData (tk, m_fallback)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 8: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Max_Degree) != 0) {
							if ((status = PutData (tk, m_max_degree)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 9: {
						if ((m_lod_options_mask & TKO_LOD_Min_Triangle_Count) != 0) {
							if ((status = PutData (tk, m_min_triangle_count)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 10: {
						if ((m_lod_options_mask & TKO_LOD_Num_Levels) != 0) {
							if ((status = PutData (tk, m_num_levels)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 11: {
						if ((m_lod_options_mask & TKO_LOD_Ratio) != 0) {
							if ((status = PutData (tk, m_num_ratios)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 12: {
						if ((m_lod_options_mask & TKO_LOD_Ratio) != 0) {
							if ((status = PutData (tk, m_ratio, m_num_ratios)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 13: {
						if (tk.GetTargetVersion() < 1315 && 
							m_threshold_type == TKO_LOD_Threshold_Distance)
							m_lod_options_mask &= ~TKO_LOD_Threshold;

						if ((m_lod_options_mask & TKO_LOD_Threshold) != 0) {
							if ((status = PutData (tk, m_num_thresholds)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 14: {
						if ((m_lod_options_mask & TKO_LOD_Threshold) != 0) {
							if ((status = PutData (tk, m_threshold, m_num_thresholds)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 15: {
						if ((m_lod_options_mask & TKO_LOD_Threshold) != 0 &&
							tk.GetTargetVersion() >= 908) {
							if ((status = PutData (tk, m_threshold_type)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 16: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Usefulness_Heuristic) != 0) {
							if ((status = PutData (tk, m_heuristic)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;

		
		case 28: {
			if ((emask & m_value[1] & TKO_Rendo_Tessellation) != 0) {
				switch (m_progress) {
					case 0: {
						if ((status = PutData (tk, m_tessellations)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((m_tessellations & TKO_Tessellation_Cylinder) != 0) {
							if ((status = PutData (tk, m_num_cylinder)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 2: {
						if ((m_tessellations & TKO_Tessellation_Cylinder) != 0) {
							if ((status = PutData (tk, m_cylinder, m_num_cylinder)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_tessellations & TKO_Tessellation_Sphere) != 0) {
							if ((status = PutData (tk, m_num_sphere)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 4: {
						if ((m_tessellations & TKO_Tessellation_Sphere) != 0) {
							if ((status = PutData (tk, m_sphere, m_num_sphere)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;

		
		case 29: {
			if ((emask & m_value[1] & (TKO_Rendo_Transparency_Style|TKO_Rendo_Transparency_Options)) != 0) {
				switch (m_progress) {
					case 0: {
						unsigned char	byte = (unsigned char)m_transparency_options;

						if (tk.GetTargetVersion() < 1605)
							byte &= ~TKO_Transparency_Extended;

						if ((status = PutData (tk, byte)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if (tk.GetTargetVersion() >= 1605 &&
							(m_transparency_options & TKO_Transparency_Extended) != 0) {
							unsigned char       byte = (unsigned char)(m_transparency_options >> TKO_Transparency_Extended_Shift);

							if ((status = PutData (tk, byte)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 2: {
						if ((m_transparency_options & TKO_Transparency_Peeling_Layers) != 0) {
							if ((status = PutData (tk, m_depth_peeling_layers)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_transparency_options & TKO_Transparency_Peeling_Min_Area) != 0) {
							if ((status = PutData (tk, m_depth_peeling_min_area)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;

		
		case 30: {
			if ((emask & m_value[1] & TKO_Rendo_Cut_Geometry) != 0) {
				switch (m_progress) {
					case 0: {
						if ((status = PutData (tk, m_cut_geometry)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((m_cut_geometry & TKO_Cut_Geometry_Level) != 0) {
							if ((status = PutData (tk, m_cut_geometry_level)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 2: {
						if ((m_cut_geometry & TKO_Cut_Geometry_Tolerance) != 0) {
							if ((status = PutData (tk, m_cut_geometry_tolerance)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_cut_geometry & TKO_Cut_Geometry_Match_Color) != 0) {
							if ((status = PutData (tk, m_cut_geometry_match)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;

		
		case 31: {
			if ((emask & m_value[1] & TKO_Rendo_Depth_Range) != 0) {
				if ((status = PutData (tk, m_depth_range, 2)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 32: {
			if ((emask & m_value[1] & TKO_Rendo_Mask_Transform) != 0) {
				if ((status = PutData (tk, m_mask_transform)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 33: {
			if ((emask & m_value[1] & TKO_Rendo_Image_Scale) != 0) {
				if ((status = PutData (tk, m_image_scale, 2)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 34: {
			if ((emask & m_value[1] & TKO_Rendo_Simple_Shadow) != 0) {
				unsigned short          smask = m_simple_shadow;

				if (tk.GetTargetVersion() < 1170) {
					smask &= ~(TKO_Simple_Shadow_Extended|TKO_Simple_Shadow_Auto);
				}
				if (tk.GetTargetVersion() < 1410) {
					smask &= ~(TKO_Simple_Shadow_Opacity);
				}
				switch (m_progress) {
					case 0: {
						unsigned char       byte = (unsigned char)smask;

						if ((status = PutData (tk, byte)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((smask & TKO_Simple_Shadow_Extended) != 0) {
							unsigned char       byte = (unsigned char)(smask >> TKO_Simple_Shadow_Extended_Shift);

							if ((status = PutData (tk, byte)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 2: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Plane) != 0) {
							if ((status = PutData (tk, m_simple_shadow_plane, 4)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Light_Direction) != 0) {
							if ((status = PutData (tk, m_simple_shadow_light, 3)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 4: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Color) != 0) {
							unsigned char       rgb[3];

							floats_to_bytes (m_simple_shadow_color, rgb, 3);

							if ((status = PutData (tk, rgb, 3)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 5: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Blur) != 0) {
							if ((status = PutData (tk, m_simple_shadow_blur)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 6: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Resolution) != 0) {
							if ((status = PutData (tk, m_simple_shadow_resolution)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 7: {
						if ((smask & TKO_Simple_Shadow_Opacity) != 0) {
							if ((status = PutData (tk, m_simple_shadow_opacity)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
		   }
			m_stage++;
		}   nobreak;

		
		case 35: {
			if ((emask & m_value[1] & TKO_Rendo_Shadow_Map) != 0) {
				switch (m_progress) {
					case 0: {
						if (m_shadow_map & TKO_Shadow_Map_Extended_Mask)
							m_shadow_map |= TKO_Shadow_Map_Extended;
						unsigned char byte = (unsigned char) m_shadow_map & 0xFF;
						if ((status = PutData (tk, byte)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((m_shadow_map & TKO_Shadow_Map_Extended) != 0) {
							unsigned char byte = (unsigned char)
									(m_shadow_map >> 8) & 0xFF;
							if ((status = PutData (tk, byte)) != TK_Normal)
								return status;
						}
						m_progress++;
					}   nobreak;


					case 2: {
						if ((m_shadow_map & TKO_Shadow_Map_Resolution) != 0) {
							if ((status = PutData (tk, m_shadow_map_resolution)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_shadow_map & TKO_Shadow_Map_Samples) != 0) {
							if ((status = PutData (tk, m_shadow_map_samples)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
		   }
			m_stage++;
		}   nobreak;

		
		case 36: {
			if ((emask & m_value[1] & TKO_Rendo_Simple_Reflection) != 0) {
				unsigned short		sr = m_simple_reflection;

				if (tk.GetTargetVersion() < 1655)
					sr &= ~TKO_Simple_Reflection_Visibility;

				if ((sr & TKO_Simple_Reflection_Extended_Mask) != 0)
					sr |= TKO_Simple_Reflection_Extended;
				else
					sr &= ~TKO_Simple_Reflection_Extended;

				switch (m_progress) {
					case 0: {
						unsigned char byte = (unsigned char) sr & 0xFF;
						if ((status = PutData (tk, byte)) != TK_Normal)
							return status;
						m_progress++;
					}   nobreak;

					case 1: {
						if ((sr & TKO_Simple_Reflection_Extended) != 0) {
							unsigned char byte = (unsigned char)
									(sr >> TKO_Simple_Reflection_Extended_Shift) & 0xFF;
							if ((status = PutData (tk, byte)) != TK_Normal)
								return status;
						}
						m_progress++;
					}   nobreak;

					case 2: {
						if ((sr & TKO_Simple_Reflection_Plane) != 0) {
							if ((status = PutData (tk, m_simple_reflection_plane, 4)) != TK_Normal)
								return status;
						}
						m_progress++;
					}   nobreak;

					case 3: {
						if ((sr & TKO_Simple_Reflection_Opacity) != 0) {
							if ((status = PutData (tk, m_simple_reflection_opacity)) != TK_Normal)
								return status;
						}
						m_progress++;
					}   nobreak;

					case 4: {
						if ((sr & TKO_Simple_Reflection_Blur) != 0) {
							if ((status = PutData (tk, m_simple_reflection_blur)) != TK_Normal)
								return status;
						}
						m_progress++;
					}   nobreak;

					case 5: {
						if ((sr & TKO_Simple_Reflection_Attenuation) != 0) {
							if ((status = PutData (tk, m_simple_reflection_hither)) != TK_Normal)
								return status;
						}
						m_progress++;
					}   nobreak;

					case 6: {
						if ((sr & TKO_Simple_Reflection_Attenuation) != 0) {
							if ((status = PutData (tk, m_simple_reflection_yon)) != TK_Normal)
								return status;
						}
						m_progress++;
					}   nobreak;

					case 7: {
						if ((sr & TKO_Simple_Reflection_Visibility) != 0) {
							if ((status = PutData (tk, m_simple_reflection_visibility_mask)) != TK_Normal)
								return status;
						}
						m_progress++;
					}   nobreak;

					case 8: {
						if ((sr & TKO_Simple_Reflection_Visibility) != 0) {
							if ((status = PutData (tk, m_simple_reflection_visibility_value)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}   nobreak;
				}
		   }
			m_stage++;
		}   nobreak;

		
		case 37: {
			if ((emask & m_value[1] & TKO_Rendo_Geometry_Options) != 0 &&
				tk.GetTargetVersion() > 1200) {
				switch (m_progress) {
					case 0: {
						if ((status = PutData (tk, m_geometry_options)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((m_geometry_options & TKO_Geometry_Options_Dihedral) != 0) {
							if ((status = PutData (tk, m_dihedral)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}

			m_stage++;
		}   nobreak;

		
		case 38: {
			if ((emask & m_value[1] & TKO_Rendo_Image_Tint) != 0 &&
				tk.GetTargetVersion() >= 1210) {
				unsigned char       rgb[3];

				floats_to_bytes (m_image_tint_color, rgb, 3);

				if ((status = PutData (tk, rgb, 3)) != TK_Normal)
					return status;
			}
			m_stage++;

		} nobreak;

		
		case 39: {
			if ((emask & m_value[1] & TKO_Rendo_General_Displacement) != 0 &&
				tk.GetTargetVersion() >= 1335) {

				if ((status = PutData (tk, m_general_displacement)) != TK_Normal)
					return status;
			}
			m_stage++;

		} nobreak;
		
		
		case 40: {
			if ((emask & m_value[1] & TKO_Rendo_Join_Cutoff_Angle) != 0 &&
				tk.GetTargetVersion() >= 1405) {

				if ((status = PutData (tk, m_join_cutoff_angle)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 41: {
			if ((emask & m_value[1] & TKO_Rendo_Screen_Range) != 0) {
				if ((status = PutData (tk, m_screen_range, 4)) != TK_Normal)
					return status;
			}
			m_stage++;
		} nobreak;

		
		case 42: {
			if ((mask & m_value[0] & TKO_Rendo_Display_Lists) != 0 &&
				tk.GetTargetVersion() >= 1515) {
				if ((status = PutData (tk, m_display_list_level)) != TK_Normal)
					return status;
			}
			m_progress = 0;
			m_stage++;
		} nobreak;

		
		case 43: {
			if ((emask & m_value[1] & TKO_Rendo_Ambient_Up_Vector) != 0) {
				while (m_progress < 3) {
					if ((status = PutData (tk, m_ambient_up_vector[m_progress])) != TK_Normal)
						return status;
					m_progress++;
				}
			}
			m_progress = 0;
			m_stage++;
		} nobreak;

		
		case 44: {
			switch (m_progress)  {
				case 0: 
				case 1: {
					if ((emask & m_value[1] & TKO_Rendo_Gooch_Color_Range) != 0) {
						while (m_progress < 2) {
							if ((status = PutData (tk, m_gooch_color_range[m_progress])) != TK_Normal)
								return status;
							m_progress++;
						}
					}
					else 
						m_progress += 2;
				} nobreak;
				case 2: {
					if ((emask & m_value[1] & TKO_Rendo_Gooch_Diffuse_Weight) != 0) {
						if ((status = PutData (tk, m_gooch_diffuse_weight)) != TK_Normal)
							return status;
					}
					m_progress++;
				} nobreak;
				case 3: {
					if ((emask & m_value[1] & TKO_Rendo_Gooch_Color_Map) != 0) {
						if ((status = PutData (tk, m_gooch_color_map_segment_length)) != TK_Normal)
								return status;
					}
					m_progress++;
				} nobreak;
				case 4: {
					if ((emask & m_value[1] & TKO_Rendo_Gooch_Color_Map) != 0) {
						if ((status = PutData (tk, m_gooch_color_map_segment, m_gooch_color_map_segment_length)) != TK_Normal)
								return status;
					}
					m_progress++;
				} 
			}
			m_stage++;
		} nobreak;

		
		case 45: {
			if ((emask & m_value[1] & TKO_Rendo_Antialias) != 0) {
				if ((status = PutData (tk, m_antialias)) != TK_Normal)
					return status;
			}
			m_stage++;
		} nobreak;


		
		case 46: {
			
			
			
			
			m_stage++;
		} nobreak;

		
		case 47: {
			if ((emask & m_value[1] & TKO_Rendo_Vertex_Displacement) != 0) {
				if ((status = PutData (tk, m_vertex_displacement)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 48: {
			
#if 0
			if (m_rrt != null) {
				if ((status = m_rrt->Write (tk)) != TK_Normal)
					return status;
			}
#endif
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Rendering_Options::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			m_progress = 0; 
			m_stage++;
		}   nobreak;


		case 1: {
			if ((status = GetData (tk, m_mask[0])) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask[0] & TKO_Rendo_Extended) != 0) {
				if ((status = GetData (tk, m_mask[1])) != TK_Normal)
					return status;
			}
			else
				m_mask[1] = 0;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask[1] & TKO_Rendo_Extended2) != 0) {
				if ((status = GetData (tk, m_mask[2])) != TK_Normal)
					return status;
			}
			else
				m_mask[2] = 0;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = GetData (tk, m_value[0])) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_mask[0] & TKO_Rendo_Extended) != 0) {
				if ((status = GetData (tk, m_value[1])) != TK_Normal)
					return status;
			}
			else
				m_value[1] = 0;
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_mask[1] & TKO_Rendo_Extended2) != 0) {
				if ((status = GetData (tk, m_value[2])) != TK_Normal)
					return status;
			}
			else
				m_value[2] = 0;
			m_stage++;
		}   nobreak;


		case 7: {
			if ((m_mask[0] & TKO_Rendo_Any_HSR) != 0) {
				if ((status = GetData (tk, m_hsr)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 8: {
			if ((m_mask[0] & TKO_Rendo_TQ) != 0) {
				if ((status = GetData (tk, m_tq)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Face_Displacement) != 0) {
				if ((status = GetData (tk, m_face_displacement)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;


		
		case 10: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Attribute_Lock) != 0) {
				if ((status = GetData (tk, m_lock_mask)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 11: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Attribute_Lock) != 0) {
				if ((status = GetData (tk, m_lock_value)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 12: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Attribute_Lock) != 0 &&
				(m_lock_mask & m_lock_value & TKO_Lock_Color) != 0) {
				if ((status = GetData (tk, m_lock_color_mask)) != TK_Normal)
					return status;
			}
			m_progress = 0; 
			m_stage++;
		}   nobreak;

		case 13: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Attribute_Lock) != 0 &&
				(m_lock_mask & m_lock_value & TKO_Lock_Color) != 0) {
				if (tk.GetVersion() <= 1105) {
					if ((status = GetData (tk, m_lock_color_value)) != TK_Normal)
						return status;

					if ((m_lock_color_mask & (TKO_Geo_Face|TKO_Geo_Front)) != 0) {
						m_lock_color_face_mask = ~0;
						if ((m_lock_color_value & (TKO_Geo_Face|TKO_Geo_Front)) != 0)
							m_lock_color_face_value = ~0;
						else
							m_lock_color_face_value = 0;
					}
					else
						m_lock_color_face_mask = m_lock_color_face_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Back) != 0) {
						m_lock_color_back_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Back) != 0)
							m_lock_color_back_value = ~0;
						else
							m_lock_color_back_value = 0;
					}
					else
						m_lock_color_back_mask = m_lock_color_back_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Edge) != 0) {
						m_lock_color_edge_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Edge) != 0)
							m_lock_color_edge_value = ~0;
						else
							m_lock_color_edge_value = 0;
					}
					else
						m_lock_color_edge_mask = m_lock_color_edge_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Line) != 0) {
						m_lock_color_line_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Line) != 0)
							m_lock_color_line_value = ~0;
						else
							m_lock_color_line_value = 0;
					}
					else
						m_lock_color_line_mask = m_lock_color_line_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Text) != 0) {
						m_lock_color_text_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Text) != 0)
							m_lock_color_text_value = ~0;
						else
							m_lock_color_text_value = 0;
					}
					else
						m_lock_color_text_mask = m_lock_color_text_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Marker) != 0) {
						m_lock_color_marker_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Marker) != 0)
							m_lock_color_marker_value = ~0;
						else
							m_lock_color_marker_value = 0;
					}
					else
						m_lock_color_marker_mask = m_lock_color_marker_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Vertex) != 0) {
						m_lock_color_vertex_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Vertex) != 0)
							m_lock_color_vertex_value = ~0;
						else
							m_lock_color_vertex_value = 0;
					}
					else
						m_lock_color_vertex_mask = m_lock_color_vertex_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Window) != 0) {
						m_lock_color_window_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Window) != 0)
							m_lock_color_window_value = ~0;
						else
							m_lock_color_window_value = 0;
					}
					else
						m_lock_color_window_mask = m_lock_color_window_value = 0;

					
					if ((m_lock_color_mask & TKO_Geo_Face_Contrast) != 0) {
						m_lock_color_face_contrast_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Face_Contrast) != 0)
							m_lock_color_face_contrast_value = ~0;
						else
							m_lock_color_face_contrast_value = 0;
					}
					else
						m_lock_color_face_contrast_mask = m_lock_color_face_contrast_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Edge_Contrast) != 0) {
						m_lock_color_edge_contrast_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Edge_Contrast) != 0)
							m_lock_color_edge_contrast_value = ~0;
						else
							m_lock_color_edge_contrast_value = 0;
					}
					else
						m_lock_color_edge_contrast_mask = m_lock_color_edge_contrast_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Line_Contrast) != 0) {
						m_lock_color_line_contrast_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Line_Contrast) != 0)
							m_lock_color_line_contrast_value = ~0;
						else
							m_lock_color_line_contrast_value = 0;
					}
					else
						m_lock_color_line_contrast_mask = m_lock_color_line_contrast_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Text_Contrast) != 0) {
						m_lock_color_text_contrast_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Text_Contrast) != 0)
							m_lock_color_text_contrast_value = ~0;
						else
							m_lock_color_text_contrast_value = 0;
					}
					else
						m_lock_color_text_contrast_mask = m_lock_color_text_contrast_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Marker_Contrast) != 0) {
						m_lock_color_marker_contrast_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Marker_Contrast) != 0)
							m_lock_color_marker_contrast_value = ~0;
						else
							m_lock_color_marker_contrast_value = 0;
					}
					else
						m_lock_color_marker_contrast_mask = m_lock_color_marker_contrast_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Vertex_Contrast) != 0) {
						m_lock_color_vertex_contrast_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Vertex_Contrast) != 0)
							m_lock_color_vertex_contrast_value = ~0;
						else
							m_lock_color_vertex_contrast_value = 0;
					}
					else
						m_lock_color_vertex_contrast_mask = m_lock_color_vertex_contrast_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Window_Contrast) != 0) {
						m_lock_color_window_contrast_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Window_Contrast) != 0)
							m_lock_color_window_contrast_value = ~0;
						else
							m_lock_color_window_contrast_value = 0;
					}
					else
						m_lock_color_window_contrast_mask = m_lock_color_window_contrast_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Simple_Reflection) != 0) {
						m_lock_color_simple_reflection_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Simple_Reflection) != 0)
							m_lock_color_simple_reflection_value = ~0;
						else
							m_lock_color_simple_reflection_value = 0;
					}
					else
						m_lock_color_simple_reflection_mask = m_lock_color_simple_reflection_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Cut_Face) != 0) {
						m_lock_color_cut_face_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Cut_Face) != 0)
							m_lock_color_cut_face_value = ~0;
						else
							m_lock_color_cut_face_value = 0;
					}
					else
						m_lock_color_cut_face_mask = m_lock_color_cut_face_value = 0;

					if ((m_lock_color_mask & TKO_Geo_Cut_Edge) != 0) {
						m_lock_color_cut_edge_mask = ~0;
						if ((m_lock_color_value & TKO_Geo_Cut_Edge) != 0)
							m_lock_color_cut_edge_value = ~0;
						else
							m_lock_color_cut_edge_value = 0;
					}
					else
						m_lock_color_cut_edge_mask = m_lock_color_cut_edge_value = 0;
				}
				else switch (m_progress) {
					case 0:
						if ((status = GetData (tk, m_lock_color_value)) != TK_Normal)
							return status;
						m_progress++;

					case 1:
						if ((m_lock_color_value & (TKO_Geo_Face|TKO_Geo_Front)) != 0)
							if ((status = GetData (tk, m_lock_color_face_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 2:
						if ((m_lock_color_value & (TKO_Geo_Face|TKO_Geo_Front)) != 0)
							if ((status = GetData (tk, m_lock_color_face_value)) != TK_Normal)
								return status;
						m_progress++;

					case 3:
						if ((m_lock_color_value & TKO_Geo_Back) != 0)
							if ((status = GetData (tk, m_lock_color_back_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 4:
						if ((m_lock_color_value & TKO_Geo_Back) != 0)
							if ((status = GetData (tk, m_lock_color_back_value)) != TK_Normal)
								return status;
						m_progress++;

					case 5:
						if ((m_lock_color_value & TKO_Geo_Edge) != 0)
							if ((status = GetData (tk, m_lock_color_edge_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 6:
						if ((m_lock_color_value & TKO_Geo_Edge) != 0)
							if ((status = GetData (tk, m_lock_color_edge_value)) != TK_Normal)
								return status;
						m_progress++;

					case 7:
						if ((m_lock_color_value & TKO_Geo_Line) != 0)
							if ((status = GetData (tk, m_lock_color_line_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 8:
						if ((m_lock_color_value & TKO_Geo_Line) != 0)
							if ((status = GetData (tk, m_lock_color_line_value)) != TK_Normal)
								return status;
						m_progress++;

					case 9:
						if ((m_lock_color_value & TKO_Geo_Text) != 0)
							if ((status = GetData (tk, m_lock_color_text_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 10:
						if ((m_lock_color_value & TKO_Geo_Text) != 0)
							if ((status = GetData (tk, m_lock_color_text_value)) != TK_Normal)
								return status;
						m_progress++;

					case 11:
						if ((m_lock_color_value & TKO_Geo_Marker) != 0)
							if ((status = GetData (tk, m_lock_color_marker_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 12:
						if ((m_lock_color_value & TKO_Geo_Marker) != 0)
							if ((status = GetData (tk, m_lock_color_marker_value)) != TK_Normal)
								return status;
						m_progress++;

					case 13:
						if ((m_lock_color_value & TKO_Geo_Vertex) != 0)
							if ((status = GetData (tk, m_lock_color_vertex_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 14:
						if ((m_lock_color_value & TKO_Geo_Vertex) != 0)
							if ((status = GetData (tk, m_lock_color_vertex_value)) != TK_Normal)
								return status;
						m_progress++;

					case 15:
						if ((m_lock_color_value & TKO_Geo_Window) != 0)
							if ((status = GetData (tk, m_lock_color_window_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 16:
						if ((m_lock_color_value & TKO_Geo_Window) != 0)
							if ((status = GetData (tk, m_lock_color_window_value)) != TK_Normal)
								return status;
						m_progress++;

					case 17:
						if ((m_lock_color_value & TKO_Geo_Face_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_face_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 18:
						if ((m_lock_color_value & TKO_Geo_Face_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_face_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 19:
						if ((m_lock_color_value & TKO_Geo_Edge_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_edge_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 20:
						if ((m_lock_color_value & TKO_Geo_Edge_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_edge_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 21:
						if ((m_lock_color_value & TKO_Geo_Line_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_line_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 22:
						if ((m_lock_color_value & TKO_Geo_Line_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_line_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 23:
						if ((m_lock_color_value & TKO_Geo_Text_Contrast) != 0 ||
							tk.GetVersion() < 1555 && (m_lock_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = GetData (tk, m_lock_color_text_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 24:
						if ((m_lock_color_value & TKO_Geo_Text_Contrast) != 0 ||
							tk.GetVersion() < 1555 && (m_lock_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = GetData (tk, m_lock_color_text_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 25:
						if ((m_lock_color_value & TKO_Geo_Marker_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_marker_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 26:
						if ((m_lock_color_value & TKO_Geo_Marker_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_marker_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 27:
						if ((m_lock_color_value & TKO_Geo_Vertex_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_vertex_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 28:
						if ((m_lock_color_value & TKO_Geo_Vertex_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_vertex_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 29:
						if ((m_lock_color_value & TKO_Geo_Window_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_window_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 30:
						if ((m_lock_color_value & TKO_Geo_Window_Contrast) != 0)
							if ((status = GetData (tk, m_lock_color_window_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 31:
						if ((m_lock_color_value & TKO_Geo_Simple_Reflection) != 0) {
							if (tk.GetVersion() >= 1550) {
								if ((status = GetData (tk, m_lock_color_simple_reflection_mask)) != TK_Normal)
									return status;
							}
							else
								m_lock_color_simple_reflection_mask = TKO_Lock_Channel_ALL;
						}
						m_progress++;
					case 32:
						if ((m_lock_color_value & TKO_Geo_Simple_Reflection) != 0) {
							if (tk.GetVersion() >= 1550) {
								if ((status = GetData (tk, m_lock_color_simple_reflection_value)) != TK_Normal)
									return status;
							}
							else
								m_lock_color_simple_reflection_value = TKO_Lock_Channel_ALL;
						}
						m_progress++;

					case 33:
						if ((m_lock_color_value & TKO_Geo_Cut_Face) != 0) {
							if (tk.GetVersion() >= 1220) {
								if ((status = GetData (tk, m_lock_color_cut_face_mask)) != TK_Normal)
									return status;
							}
							else
								m_lock_color_cut_face_mask = TKO_Lock_Channel_ALL;
						}
						m_progress++;
					case 34:
						if ((m_lock_color_value & TKO_Geo_Cut_Face) != 0) {
							if (tk.GetVersion() >= 1220) {
								if ((status = GetData (tk, m_lock_color_cut_face_value)) != TK_Normal)
									return status;
							}
							else
								m_lock_color_cut_face_value = TKO_Lock_Channel_ALL;
						}
						m_progress++;

					case 35:
						if ((m_lock_color_value & TKO_Geo_Cut_Edge) != 0) {
							if (tk.GetVersion() >= 1220) {
								if ((status = GetData (tk, m_lock_color_cut_edge_mask)) != TK_Normal)
									return status;
							}
							else
								m_lock_color_cut_edge_mask = TKO_Lock_Channel_ALL;
						}
						m_progress++;
					case 36:
						if ((m_lock_color_value & TKO_Geo_Cut_Edge) != 0) {
							if (tk.GetVersion() >= 1220) {
								if ((status = GetData (tk, m_lock_color_cut_edge_value)) != TK_Normal)
									return status;
							}
							else
								m_lock_color_cut_edge_value = TKO_Lock_Channel_ALL;
						}
						m_progress = 0;
						break;

					default:
						return tk.Error ("lost track processing color locks");
				}
			}
			m_stage++;
		}   nobreak;

		case 14: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Attribute_Lock) != 0 &&
				(m_lock_mask & m_lock_value & TKO_Lock_Visibility) != 0) {
				switch (m_progress) {
					case 0:
						if ((status = GetData (tk, m_lock_visibility_mask)) != TK_Normal)
							return status;
						m_progress++;
					case 1:
						if ((status = GetData (tk, m_lock_visibility_value)) != TK_Normal)
							return status;
						m_progress = 0;
						break;

					default:
						return tk.Error ("lost track processing visibility locks");
				}
			}
			m_stage++;
		}   nobreak;


		
		case 15: {
			if ((m_mask[2] & m_value[2] & TKO_Rendo_Forced_Lock) != 0) {
				if ((status = GetData (tk, m_forced_mask)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 16: {
			if ((m_mask[2] & m_value[2] & TKO_Rendo_Forced_Lock) != 0) {
				if ((status = GetData (tk, m_forced_value)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 17: {
			if ((m_mask[2] & m_value[2] & TKO_Rendo_Forced_Lock) != 0 &&
				(m_forced_mask & m_forced_value & TKO_Lock_Color) != 0) {
				if ((status = GetData (tk, m_forced_color_mask)) != TK_Normal)
					return status;
			}
			m_progress = 0; 
			m_stage++;
		}   nobreak;

		case 18: {
			if ((m_mask[2] & m_value[2] & TKO_Rendo_Forced_Lock) != 0 &&
				(m_forced_mask & m_forced_value & TKO_Lock_Color) != 0) {
				switch (m_progress) {
					case 0:
						if ((status = GetData (tk, m_forced_color_value)) != TK_Normal)
							return status;
						m_progress++;

					case 1:
						if ((m_forced_color_value & (TKO_Geo_Face|TKO_Geo_Front)) != 0)
							if ((status = GetData (tk, m_forced_color_face_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 2:
						if ((m_forced_color_value & (TKO_Geo_Face|TKO_Geo_Front)) != 0)
							if ((status = GetData (tk, m_forced_color_face_value)) != TK_Normal)
								return status;
						m_progress++;

					case 3:
						if ((m_forced_color_value & TKO_Geo_Back) != 0)
							if ((status = GetData (tk, m_forced_color_back_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 4:
						if ((m_forced_color_value & TKO_Geo_Back) != 0)
							if ((status = GetData (tk, m_forced_color_back_value)) != TK_Normal)
								return status;
						m_progress++;

					case 5:
						if ((m_forced_color_value & TKO_Geo_Edge) != 0)
							if ((status = GetData (tk, m_forced_color_edge_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 6:
						if ((m_forced_color_value & TKO_Geo_Edge) != 0)
							if ((status = GetData (tk, m_forced_color_edge_value)) != TK_Normal)
								return status;
						m_progress++;

					case 7:
						if ((m_forced_color_value & TKO_Geo_Line) != 0)
							if ((status = GetData (tk, m_forced_color_line_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 8:
						if ((m_forced_color_value & TKO_Geo_Line) != 0)
							if ((status = GetData (tk, m_forced_color_line_value)) != TK_Normal)
								return status;
						m_progress++;

					case 9:
						if ((m_forced_color_value & TKO_Geo_Text) != 0)
							if ((status = GetData (tk, m_forced_color_text_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 10:
						if ((m_forced_color_value & TKO_Geo_Text) != 0)
							if ((status = GetData (tk, m_forced_color_text_value)) != TK_Normal)
								return status;
						m_progress++;

					case 11:
						if ((m_forced_color_value & TKO_Geo_Marker) != 0)
							if ((status = GetData (tk, m_forced_color_marker_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 12:
						if ((m_forced_color_value & TKO_Geo_Marker) != 0)
							if ((status = GetData (tk, m_forced_color_marker_value)) != TK_Normal)
								return status;
						m_progress++;

					case 13:
						if ((m_forced_color_value & TKO_Geo_Vertex) != 0)
							if ((status = GetData (tk, m_forced_color_vertex_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 14:
						if ((m_forced_color_value & TKO_Geo_Vertex) != 0)
							if ((status = GetData (tk, m_forced_color_vertex_value)) != TK_Normal)
								return status;
						m_progress++;

					case 15:
						if ((m_forced_color_value & TKO_Geo_Window) != 0)
							if ((status = GetData (tk, m_forced_color_window_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 16:
						if ((m_forced_color_value & TKO_Geo_Window) != 0)
							if ((status = GetData (tk, m_forced_color_window_value)) != TK_Normal)
								return status;
						m_progress++;

					case 17:
						if ((m_forced_color_value & TKO_Geo_Face_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_face_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 18:
						if ((m_forced_color_value & TKO_Geo_Face_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_face_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 19:
						if ((m_forced_color_value & TKO_Geo_Edge_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_edge_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 20:
						if ((m_forced_color_value & TKO_Geo_Edge_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_edge_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 21:
						if ((m_forced_color_value & TKO_Geo_Line_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_line_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 22:
						if ((m_forced_color_value & TKO_Geo_Line_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_line_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 23:
						if ((m_forced_color_value & TKO_Geo_Text_Contrast) != 0 ||
							tk.GetVersion() < 1555 && (m_forced_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = GetData (tk, m_forced_color_text_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 24:
						if ((m_forced_color_value & TKO_Geo_Text_Contrast) != 0 ||
							tk.GetVersion() < 1555 && (m_forced_color_value & TKO_Geo_Cut_Edge) != 0)
							if ((status = GetData (tk, m_forced_color_text_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 25:
						if ((m_forced_color_value & TKO_Geo_Marker_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_marker_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 26:
						if ((m_forced_color_value & TKO_Geo_Marker_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_marker_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 27:
						if ((m_forced_color_value & TKO_Geo_Vertex_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_vertex_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 28:
						if ((m_forced_color_value & TKO_Geo_Vertex_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_vertex_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 29:
						if ((m_forced_color_value & TKO_Geo_Window_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_window_contrast_mask)) != TK_Normal)
								return status;
						m_progress++;
					case 30:
						if ((m_forced_color_value & TKO_Geo_Window_Contrast) != 0)
							if ((status = GetData (tk, m_forced_color_window_contrast_value)) != TK_Normal)
								return status;
						m_progress++;

					case 31:
						if ((m_forced_color_value & TKO_Geo_Simple_Reflection) != 0) {
							if (tk.GetVersion() >= 1550) {
								if ((status = GetData (tk, m_forced_color_simple_reflection_mask)) != TK_Normal)
									return status;
							}
							else
								m_forced_color_simple_reflection_mask = TKO_Lock_Channel_ALL;
						}
						m_progress++;
					case 32:
						if ((m_forced_color_value & TKO_Geo_Simple_Reflection) != 0) {
							if (tk.GetVersion() >= 1550) {
								if ((status = GetData (tk, m_forced_color_simple_reflection_value)) != TK_Normal)
									return status;
							}
							else
								m_forced_color_simple_reflection_value = TKO_Lock_Channel_ALL;
						}
						m_progress++;

					case 33:
						if ((m_forced_color_value & TKO_Geo_Cut_Face) != 0) {
							if (tk.GetVersion() >= 1220) {
								if ((status = GetData (tk, m_forced_color_cut_face_mask)) != TK_Normal)
									return status;
							}
							else
								m_forced_color_cut_face_mask = TKO_Lock_Channel_ALL;
						}
						m_progress++;
					case 34:
						if ((m_forced_color_value & TKO_Geo_Cut_Face) != 0) {
							if (tk.GetVersion() >= 1220) {
								if ((status = GetData (tk, m_forced_color_cut_face_value)) != TK_Normal)
									return status;
							}
							else
								m_forced_color_cut_face_value = TKO_Lock_Channel_ALL;
						}
						m_progress++;

					case 35:
						if ((m_forced_color_value & TKO_Geo_Cut_Edge) != 0) {
							if (tk.GetVersion() >= 1220) {
								if ((status = GetData (tk, m_forced_color_cut_edge_mask)) != TK_Normal)
									return status;
							}
							else
								m_forced_color_cut_edge_mask = TKO_Lock_Channel_ALL;
						}
						m_progress++;
					case 36:
						if ((m_forced_color_value & TKO_Geo_Cut_Edge) != 0) {
							if (tk.GetVersion() >= 1220) {
								if ((status = GetData (tk, m_forced_color_cut_edge_value)) != TK_Normal)
									return status;
							}
							else
								m_forced_color_cut_edge_value = TKO_Lock_Channel_ALL;
						}
						m_progress = 0;
						break;

					default:
						return tk.Error ("lost track processing color forced locks");
				}
			}
			m_stage++;
		}   nobreak;

		case 19: {
			if ((m_mask[2] & m_value[2] & TKO_Rendo_Forced_Lock) != 0 &&
				(m_forced_mask & m_forced_value & TKO_Lock_Visibility) != 0) {
				switch (m_progress) {
					case 0:
						if ((status = GetData (tk, m_forced_visibility_mask)) != TK_Normal)
							return status;
						m_progress++;
					case 1:
						if ((status = GetData (tk, m_forced_visibility_value)) != TK_Normal)
							return status;
						m_progress = 0;
						break;

					default:
						return tk.Error ("lost track processing visibility forced locks");
				}
			}
			m_stage++;
		}   nobreak;


		case 20: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Fog) != 0) {
				if ((status = GetData (tk, m_fog_limits, 2)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 21: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Debug) != 0) {
				if ((status = GetData (tk, m_debug)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 22: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Stereo_Separation) != 0) {
				if ((status = GetData (tk, m_stereo_separation)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 23: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Stereo_Distance) != 0) {
				if ((status = GetData (tk, m_stereo_distance)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;


		
		case 24: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Buffer_Options) != 0) {
				switch (m_progress) {
					case 0:
						if ((status = GetData (tk, m_buffer_options_mask)) != TK_Normal)
							return status;
						m_progress++;
					case 1:
						if ((status = GetData (tk, m_buffer_options_value)) != TK_Normal)
							return status;
						m_progress++;
					case 2:
						if ((m_buffer_options_mask & m_buffer_options_value & TKO_Buffer_Size_Limit) != 0) {
							if ((status = GetData (tk, m_buffer_size_limit)) != TK_Normal)
								return status;
						}
						m_progress = 0;
				}
			}
			m_stage++;
		}   nobreak;

		
		case 25: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Hidden_Line_Options) != 0) {
				switch (m_progress) {
					case 0: {
						unsigned char       byte;

						if ((status = GetData (tk, byte)) != TK_Normal)
							return status;
						m_hlr_options = byte;
						m_progress++;
					}

					case 1: {
						if ((m_hlr_options & TKO_Hidden_Line_Extended) != 0) {
							unsigned char       byte;

							if ((status = GetData (tk, byte)) != TK_Normal)
								return status;
							m_hlr_options |= byte << TKO_Hidden_Line_Extended_Shift;
						}
						m_progress++;
					}

					case 2: {
						if ((m_hlr_options & TKO_Hidden_Line_Extended2) != 0) {
							unsigned short      word;

							if ((status = GetData (tk, word)) != TK_Normal)
								return status;
							m_hlr_options |= word << TKO_Hidden_Line_Extended2_Shift;
						}
						m_progress++;
					}

					case 3: {
						if ((m_hlr_options & TKO_Hidden_Line_Pattern) != 0) {
							if ((status = GetData (tk, m_hlr_line_pattern)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 4: {
						if ((m_hlr_options & TKO_Hidden_Line_Dim_Factor) != 0) {
							if ((status = GetData (tk, m_hlr_dim_factor)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 5: {
						if ((m_hlr_options & TKO_Hidden_Line_Face_Displacement) != 0) {
							if ((status = GetData (tk, m_hlr_face_displacement)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 6: {
						if ((m_hlr_options & TKO_Hidden_Line_Color) != 0) {
							if ((status = GetData (tk, m_hlr_color, 3)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 7: {
						if ((m_hlr_options & TKO_Hidden_Line_Weight) != 0) {
							if ((status = GetData (tk, m_hlr_weight)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 8: {
						if ((m_hlr_options & TKO_Hidden_Line_Weight) != 0 &&
							m_hlr_weight != -1.0f) {
							if ((status = GetData (tk, m_hlr_weight_units)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 9: {
						if ((m_hlr_options & TKO_Hidden_Line_HSR_Algorithm) != 0) {
							if ((status = GetData (tk, m_hlr_hsr_algorithm)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;


		
		case 26: {
			if (m_mask[0] & TKO_Rendo_NURBS_Options) {
				switch (m_progress) {
					case 0: {
						unsigned char       byte;

						if ((status = GetData (tk, byte)) != TK_Normal)
							return status;
						m_nurbs_options_mask = byte;
						m_progress++;
					}

					case 1: {
						if ((m_nurbs_options_mask & TKO_NURBS_Extended)) {
							unsigned char       byte;

							if ((status = GetData (tk, byte)) != TK_Normal)
								return status;
							m_nurbs_options_mask |= (byte << TKO_NURBS_Extended_Shift);
						}
						m_progress++;
					}

					case 2: {
						unsigned char       byte;

						if ((status = GetData (tk, byte)) != TK_Normal)
							return status;
						m_nurbs_options_value = byte;
						m_progress++;
					}

					case 3: {
						if ((m_nurbs_options_mask & TKO_NURBS_Extended)) {
							unsigned char       byte;

							if ((status = GetData (tk, byte)) != TK_Normal)
								return status;
							m_nurbs_options_value |= (byte << TKO_NURBS_Extended_Shift);
						}
						m_progress++;
					}

					case 4: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Curve_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Curve_Budget) != 0) {
							if ((status = GetData (tk, m_curve_budget)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 5: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Curve_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Curve_Continued_Budget) != 0) {
							if ((status = GetData (tk, m_curve_continued_budget)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 6: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Surface_Budget) != 0) {
							if ((status = GetData (tk, m_surface_budget)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 7: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Surface_Trim_Budget) != 0) {
							if ((status = GetData (tk, m_surface_trim_budget)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 8: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Surface_Max_Trim_Curve_Deviation) != 0) {
							if ((status = GetData (tk, m_surface_max_trim_curve_deviation)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 9: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Surface_Max_Facet_Angle) != 0) {
							if ((status = GetData (tk, m_surface_max_facet_angle)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 10: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Surface_Max_Facet_Deviation) != 0) {
							if ((status = GetData (tk, m_surface_max_facet_deviation)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 11: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Surface_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Surface_Max_Facet_Width) != 0) {
							if ((status = GetData (tk, m_surface_max_facet_width)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 12: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Curve_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Curve_Max_Angle) != 0) {
							if ((status = GetData (tk, m_curve_max_angle)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 13: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Curve_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Curve_Max_Deviation) != 0) {
							if ((status = GetData (tk, m_curve_max_deviation)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 14: {
						if ((m_mask[0] & m_value[0] & TKO_Rendo_NURBS_Curve_Options) != 0 &&
							(m_nurbs_options_mask & m_nurbs_options_value & TKO_NURBS_Curve_Max_Length) != 0) {
							if ((status = GetData (tk, m_curve_max_length)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;


		
		case 27: {
			if ((m_mask[0] & m_value[0] & TKO_Rendo_LOD_Options) != 0) {
				switch (m_progress) {
					case 0: {
						if ((status = GetData (tk, m_lod_options_mask)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((status = GetData (tk, m_lod_options_value)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 2: {
						if ((m_lod_options_mask & TKO_LOD_Algorithm) != 0) {
							if ((status = GetData (tk, m_lod_algorithm)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Bounding_Explicit) != 0) {
							if ((status = GetData (tk, m_bounding, 6)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 4: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Calculation_Cutoff) != 0) {
							if ((status = GetData (tk, m_num_cutoffs)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 5: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Calculation_Cutoff) != 0) {
							if ((status = GetData (tk, m_cutoff, m_num_cutoffs)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 6: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Clamp) != 0) {
							if ((status = GetData (tk, m_clamp)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 7: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Fallback) != 0) {
							if ((status = GetData (tk, m_fallback)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 8: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Max_Degree) != 0) {
							if ((status = GetData (tk, m_max_degree)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 9: {
						if ((m_lod_options_mask & TKO_LOD_Min_Triangle_Count) != 0) {
							if ((status = GetData (tk, m_min_triangle_count)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 10: {
						if ((m_lod_options_mask & TKO_LOD_Num_Levels) != 0) {
							if ((status = GetData (tk, m_num_levels)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 11: {
						if ((m_lod_options_mask & TKO_LOD_Ratio) != 0) {
							if ((status = GetData (tk, m_num_ratios)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 12: {
						if ((m_lod_options_mask & TKO_LOD_Ratio) != 0) {
							if ((status = GetData (tk, m_ratio, m_num_ratios)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 13: {
						if ((m_lod_options_mask & TKO_LOD_Threshold) != 0) {
							if ((status = GetData (tk, m_num_thresholds)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 14: {
						if ((m_lod_options_mask & TKO_LOD_Threshold) != 0) {
							if ((status = GetData (tk, m_threshold, m_num_thresholds)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 15: {
						if ((m_lod_options_mask & TKO_LOD_Threshold) != 0 &&
							tk.GetVersion() >= 908) {
							if ((status = GetData (tk, m_threshold_type)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 16: {
						if ((m_lod_options_mask & m_lod_options_value & TKO_LOD_Usefulness_Heuristic) != 0) {
							if ((status = GetData (tk, m_heuristic)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;


		
		case 28: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Tessellation) != 0) {
				switch (m_progress) {
					case 0: {
						if ((status = GetData (tk, m_tessellations)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((m_tessellations & TKO_Tessellation_Cylinder) != 0) {
							if ((status = GetData (tk, m_num_cylinder)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 2: {
						if ((m_tessellations & TKO_Tessellation_Cylinder) != 0) {
							if ((status = GetData (tk, m_cylinder, m_num_cylinder)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_tessellations & TKO_Tessellation_Sphere) != 0) {
							if ((status = GetData (tk, m_num_sphere)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 4: {
						if ((m_tessellations & TKO_Tessellation_Sphere) != 0) {
							if ((status = GetData (tk, m_sphere, m_num_sphere)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;

		
		case 29: {
			if ((m_mask[1] & m_value[1] & (TKO_Rendo_Transparency_Style|TKO_Rendo_Transparency_Options)) != 0) {
				switch (m_progress) {
					case 0: {
						unsigned char       byte;

						if ((status = GetData (tk, byte)) != TK_Normal)
							return status;
						m_transparency_options = byte;
						m_progress++;
					}

					case 1: {
						if ((m_transparency_options & TKO_Simple_Shadow_Extended) != 0) {
							unsigned char       byte;

							if ((status = GetData (tk, byte)) != TK_Normal)
								return status;
							m_transparency_options |= byte << TKO_Simple_Shadow_Extended_Shift;
						}
						m_progress++;
					}

					case 2: {
						if ((m_transparency_options & TKO_Transparency_Peeling_Layers) != 0) {
							if ((status = GetData (tk, m_depth_peeling_layers)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_transparency_options & TKO_Transparency_Peeling_Min_Area) != 0) {
							if ((status = GetData (tk, m_depth_peeling_min_area)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;


		
		case 30: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Cut_Geometry) != 0) {
				switch (m_progress) {
					case 0: {
						if ((status = GetData (tk, m_cut_geometry)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((m_cut_geometry & TKO_Cut_Geometry_Level) != 0) {
							if ((status = GetData (tk, m_cut_geometry_level)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 2: {
						if ((m_cut_geometry & TKO_Cut_Geometry_Tolerance) != 0) {
							if ((status = GetData (tk, m_cut_geometry_tolerance)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_cut_geometry & TKO_Cut_Geometry_Match_Color) != 0) {
							if ((status = GetData (tk, m_cut_geometry_match)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;


		
		case 31: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Depth_Range) != 0) {
				if ((status = GetData (tk, m_depth_range, 2)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 32: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Mask_Transform) != 0) {
				if ((status = GetData (tk, m_mask_transform)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 33: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Image_Scale) != 0) {
				if ((status = GetData (tk, m_image_scale, 2)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		 
		case 34: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Simple_Shadow) != 0) {
				switch (m_progress) {
					case 0: {
						unsigned char       byte;

						if ((status = GetData (tk, byte)) != TK_Normal)
							return status;
						m_simple_shadow = byte;
						m_progress++;
					}

					case 1: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Extended) != 0) {
							unsigned char       byte;

							if ((status = GetData (tk, byte)) != TK_Normal)
								return status;
							m_simple_shadow |= byte << TKO_Simple_Shadow_Extended_Shift;
						}
						m_progress++;
					}

					case 2: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Plane) != 0) {
							if ((status = GetData (tk, m_simple_shadow_plane, 4)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Light_Direction) != 0) {
							if ((status = GetData (tk, m_simple_shadow_light, 3)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 4: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Color) != 0) {
							unsigned char       rgb[3];

							if ((status = GetData (tk, rgb, 3)) != TK_Normal)
								return status;

							bytes_to_floats (rgb, m_simple_shadow_color, 3);
						}
						m_progress++;
					}

					case 5: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Blur) != 0) {
							if ((status = GetData (tk, m_simple_shadow_blur)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 6: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Resolution) != 0) {
							if ((status = GetData (tk, m_simple_shadow_resolution)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 7: {
						if ((m_simple_shadow & TKO_Simple_Shadow_Opacity) != 0) {
							if ((status = GetData (tk, m_simple_shadow_opacity)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}
			m_stage++;
		}   nobreak;

		
		case 35: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Shadow_Map) != 0) {
				switch (m_progress) {
					case 0: {
						unsigned char byte;
						if ((status = GetData (tk, byte)) != TK_Normal)
							return status;
						m_shadow_map = byte;
						m_progress++;
					}

					case 1: {
						if ((m_shadow_map & TKO_Shadow_Map_Extended) != 0) {
							unsigned char byte;
							if ((status = GetData (tk, byte)) != TK_Normal)
								return status;
							m_shadow_map |= byte << 8;
						}
						m_progress++;
					}

					case 2: {
						if ((m_shadow_map & TKO_Shadow_Map_Resolution) != 0) {
							if ((status = GetData (tk, m_shadow_map_resolution)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_shadow_map & TKO_Shadow_Map_Samples) != 0) {
							if ((status = GetData (tk, m_shadow_map_samples)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
		   }
			m_stage++;
		}   nobreak;

		
		case 36: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Simple_Reflection) != 0) {
				switch (m_progress) {
					case 0: {
						unsigned char byte;
						if ((status = GetData (tk, byte)) != TK_Normal)
							return status;
						m_simple_reflection = byte;
						m_progress++;
					}

					case 1: {
						if ((m_simple_reflection & TKO_Simple_Reflection_Extended) != 0) {
							unsigned char byte;
							if ((status = GetData (tk, byte)) != TK_Normal)
								return status;
							m_simple_reflection |= byte << TKO_Simple_Reflection_Extended_Shift;
						}
						m_progress++;
					}

					case 2: {
						if ((m_simple_reflection & TKO_Simple_Reflection_Plane) != 0) {
							if ((status = GetData (tk, m_simple_reflection_plane, 4)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 3: {
						if ((m_simple_reflection & TKO_Simple_Reflection_Opacity) != 0) {
							if ((status = GetData (tk, m_simple_reflection_opacity)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 4: {
						if ((m_simple_reflection & TKO_Simple_Reflection_Blur) != 0) {
							if ((status = GetData (tk, m_simple_reflection_blur)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 5: {
						if ((m_simple_reflection & TKO_Simple_Reflection_Attenuation) != 0) {
							if ((status = GetData (tk, m_simple_reflection_hither)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 6: {
						if ((m_simple_reflection & TKO_Simple_Reflection_Attenuation) != 0) {
							if ((status = GetData (tk, m_simple_reflection_yon)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 7: {
						if ((m_simple_reflection & TKO_Simple_Reflection_Visibility) != 0) {
							if ((status = GetData (tk, m_simple_reflection_visibility_mask)) != TK_Normal)
								return status;
						}
						m_progress++;
					}

					case 8: {
						if ((m_simple_reflection & TKO_Simple_Reflection_Visibility) != 0) {
							if ((status = GetData (tk, m_simple_reflection_visibility_value)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
		   }
			m_stage++;
		}   nobreak;

		
		case 37: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Geometry_Options) != 0) {
				switch (m_progress) {
					case 0: {
						if ((status = GetData (tk, m_geometry_options)) != TK_Normal)
							return status;
						m_progress++;
					}

					case 1: {
						if ((m_geometry_options & TKO_Geometry_Options_Dihedral) != 0) {
							if ((status = GetData (tk, m_dihedral)) != TK_Normal)
								return status;
						}
						m_progress = 0;
					}
				}
			}

			m_stage++;
		}   nobreak;


		
		case 38: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Image_Tint) != 0) {
				unsigned char       rgb[3];

				if ((status = GetData (tk, rgb, 3)) != TK_Normal)
					return status;

				bytes_to_floats (rgb, m_image_tint_color, 3);
			}
			m_stage++;
		} nobreak;

		
		case 39: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_General_Displacement) != 0 && 
				tk.GetVersion() >= 1335) {

				if ((status = GetData (tk, m_general_displacement)) != TK_Normal)
					return status;

			}
			else
				m_mask[1] &= ~TKO_Rendo_General_Displacement;

			m_stage++;
		} nobreak;

		
		case 40: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Join_Cutoff_Angle) != 0 && 
				tk.GetVersion() >= 1405) {

				if ((status = GetData (tk, m_join_cutoff_angle)) != TK_Normal)
					return status;

			}
			else
				m_mask[1] &= ~TKO_Rendo_Join_Cutoff_Angle;

			m_stage++;

		} nobreak;

		
		case 41: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Screen_Range) != 0) {
				if ((status = GetData (tk, m_screen_range, 4)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		
		case 42: {
			m_display_list_level = TKO_Display_List_Level_Entity;
			if ((m_mask[0] & m_value[0] & TKO_Rendo_Display_Lists) != 0) {
				if (tk.GetVersion() >= 1515) {
					if ((status = GetData (tk, m_display_list_level)) != TK_Normal)
						return status;
				}
			}
			m_progress = 0;
			m_stage++;
		} nobreak;

		
		case 43: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Ambient_Up_Vector) != 0) {
				while (m_progress < 3) {
					if ((status = GetData (tk, m_ambient_up_vector[m_progress])) != TK_Normal)
						return status;
					m_progress++;
				}
			}
			m_progress = 0;
			m_stage++;
		} nobreak;

		
		case 44: {
			switch (m_progress)  {
				case 0: 
				case 1: {
					if ((m_mask[1] & m_value[1] & TKO_Rendo_Gooch_Color_Range) != 0) {
						while (m_progress < 2) {
							if ((status = GetData (tk, m_gooch_color_range[m_progress])) != TK_Normal)
								return status;
							m_progress++;
						}
					}
					else 
						m_progress += 2;
				} nobreak;
				case 2: {
					if ((m_mask[1] & TKO_Rendo_Gooch_Diffuse_Weight) != 0) {
						if ((status = GetData (tk, m_gooch_diffuse_weight)) != TK_Normal)
							return status;
					}
					m_progress++;
				} nobreak;
				case 3: {
					if ((m_mask[1] & m_value[1] & TKO_Rendo_Gooch_Color_Map) != 0) {
						if ((status = GetData (tk, m_gooch_color_map_segment_length)) != TK_Normal)
							return status;
						m_gooch_color_map_segment = new char [m_gooch_color_map_segment_length+1];
						m_gooch_color_map_segment[m_gooch_color_map_segment_length]='\0';
					}
					m_progress++;
				} nobreak;
				case 4: {
					if ((m_mask[1] & m_value[1] & TKO_Rendo_Gooch_Color_Map) != 0) {
						if ((status = GetData (tk, m_gooch_color_map_segment, m_gooch_color_map_segment_length)) != TK_Normal)
							return status;
					}
					m_progress++;
				}
			}
			m_stage++;
		} nobreak;

		
		case 45: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Antialias) != 0) {
				if ((status = GetData (tk, m_antialias)) != TK_Normal)
					return status;
			}
			m_stage++;
		} nobreak;

		
		case 46: {
			
			
			
			
			m_stage++;
		} nobreak;

		
		case 47: {
			if ((m_mask[1] & m_value[1] & TKO_Rendo_Vertex_Displacement) != 0) {
				if ((status = GetData (tk, m_vertex_displacement)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 48: {
			
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Heuristics::~TK_Heuristics () {
}

TK_Status TK_Heuristics::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;
	int				mask = m_mask;
	unsigned short	culling = m_culling;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 705)
		mask &= ~TKO_Heuristic_Extended;
	if (tk.GetTargetVersion() < 1201)
		mask &= ~TKO_Heuristic_Culling;
	if (tk.GetTargetVersion() < 1401)
		mask &= ~TKO_Heuristic_Ordered_Weights;
	if (tk.GetTargetVersion() < 1401)
		mask &= ~TKO_Heuristic_Internal_Polyline_Limit;
	if (tk.GetTargetVersion() < 1415)
		culling &= ~TKO_Heur_Culling_Extended;
	if (tk.GetTargetVersion() < 1510)
		culling &= ~TKO_Heur_Maximum_Extent_Mode;
	if (tk.GetTargetVersion() < 1515)
		mask &= ~TKO_Heuristic_Selection_Level;
	if (tk.GetTargetVersion() < 1550) {
		culling &= ~TKO_Heur_Vector_Culling;
		culling &= ~TKO_Heur_Vector_Tolerance;
	}

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned short      word = (unsigned short)(mask & 0x0000FFFF);

			if ((status = PutData (tk, word)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((mask & TKO_Heuristic_Extended)) {
				unsigned short      word = (unsigned short)(mask >> TKO_Heuristic_Extended_Shift);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			unsigned short      word = (unsigned short)(m_value & 0x0000FFFF);

			if ((status = PutData (tk, word)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((mask & TKO_Heuristic_Extended)) {
				unsigned short      word = (unsigned short)(m_value >> TKO_Heuristic_Extended_Shift);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_mask & m_value & TKO_Heuristic_Related_Select_Limit) != 0) {
				if ((status = PutData (tk, m_related)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_mask & m_value & TKO_Heuristic_Internal_Shell_Limit) != 0) {
				if ((status = PutData (tk, m_internal_shell)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if ((m_mask & m_value & TKO_Heuristic_Extras) != 0) {
				if ((status = PutData (tk, m_extras)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 8: {
			if ((mask & m_value & TKO_Heuristic_Culling)) {
				unsigned char		byte = (unsigned char)(culling & 0x00FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if ((mask & m_value & TKO_Heuristic_Culling) &&
				(culling & TKO_Heur_Culling_Extended) != 0) {
				unsigned char		byte = (unsigned char)(culling >> TKO_Heur_Culling_Extended_Shift);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 10: {
			if ((mask & TKO_Heuristic_Culling) &&
				(culling & TKO_Heur_Obscuration_Culling)) {
				if ((status = PutData (tk, m_pixel_threshold)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 11: {
			if ((mask & TKO_Heuristic_Culling) &&
				(culling & TKO_Heur_Extent_Culling)) {
				if ((status = PutData (tk, m_maximum_extent)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;
        
		case 12: {
			if ((mask & TKO_Heuristic_Culling) &&
				(culling & TKO_Heur_Maximum_Extent_Mode)) {
				if ((status = PutData (tk, m_maximum_extent_mode)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 13: {
			if ((mask & TKO_Heuristic_Ordered_Weights) ) {
				if ((status = PutData (tk, m_ordered_weights_mask)) != TK_Normal)
					return status;
				m_progress = 0;
			}
			m_stage++;
		}   nobreak;

		case 14: {
			if ((mask & TKO_Heuristic_Ordered_Weights) ) {
				while (m_progress < TKO_Heur_Order_Count) {
					if ((m_ordered_weights_mask & (1<<m_progress)) != 0 &&
						(status = PutData (tk, m_ordered_weights[m_progress])) != TK_Normal)
						return status;
					m_progress++;
				}
				m_progress = 0;
			}
			m_stage++;
		}   nobreak;

		case 15: {
			if ((mask & TKO_Heuristic_Internal_Polyline_Limit) ) {
				if ((mask & m_value & TKO_Heuristic_Internal_Polyline_Limit) != 0) {
					if ((status = PutData (tk, m_internal_polyline)) != TK_Normal)
						return status;
				}
			}
			m_stage++;
		}   nobreak;

		case 16: {
			if ((mask & TKO_Heuristic_Selection_Level) ) {
				if ((status = PutData (tk, m_selection_level)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;
		case 17: {
			if ((m_mask & TKO_Heuristic_Culling) && (m_culling & TKO_Heur_Vector_Culling))	{
				if ((status = PutData (tk, m_vector, 3)) != TK_Normal)
					return status;
			}
			m_stage++;
		}	nobreak;
		case 18: {
			if ((m_mask & TKO_Heuristic_Culling) && (m_culling & TKO_Heur_Vector_Tolerance))	{
				if ((status = PutData (tk, m_vector_tolerance)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}	break;

		

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Heuristics::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned short      word;

			if ((status = GetData (tk, word)) != TK_Normal)
				return status;
			m_mask = word;
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_mask & TKO_Heuristic_Extended) {
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_mask |= word << TKO_Heuristic_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			unsigned short      word;

			if ((status = GetData (tk, word)) != TK_Normal)
				return status;
			m_value = word;
			m_stage++;
		}   nobreak;

		case 3: {
			if (m_mask & TKO_Heuristic_Extended) {
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_value |= word << TKO_Heuristic_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & m_value & TKO_Heuristic_Related_Select_Limit) != 0) {
				if ((status = GetData (tk, m_related)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_mask & m_value & TKO_Heuristic_Internal_Shell_Limit) != 0) {
				if ((status = GetData (tk, m_internal_shell)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_mask & m_value & TKO_Heuristic_Extras) != 0) {
				if ((status = GetData (tk, m_extras)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if (tk.GetVersion() > 705 && (m_mask & TKO_Heuristic_Culling) &&
				(m_mask & m_value & TKO_Heuristic_Culling) != 0) {
				unsigned char			byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;

				m_culling = byte;
			}
			else
				m_culling = 0;
			m_stage++;
		}   nobreak;

		case 8: {
			if ((m_culling & TKO_Heur_Culling_Extended) != 0) {
				unsigned char			byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;

				m_culling |= byte << TKO_Heur_Culling_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if (tk.GetVersion() > 1200 && (m_mask & TKO_Heuristic_Culling) &&
				(m_culling & TKO_Heur_Obscuration_Culling)) {
				if ((status = GetData (tk, m_pixel_threshold)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 10: {
			if (tk.GetVersion() > 1200 && (m_mask & TKO_Heuristic_Culling) &&
				(m_culling & TKO_Heur_Extent_Culling)) {
				if ((status = GetData (tk, m_maximum_extent)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 11: {
			if (tk.GetVersion() > 1200 && (m_mask & TKO_Heuristic_Culling) &&
				(m_culling & TKO_Heur_Maximum_Extent_Mode)) {
				if ((status = GetData (tk, m_maximum_extent_mode)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;
        
		case 12: {
			if ((m_mask & TKO_Heuristic_Ordered_Weights) != 0) {
				if ((status = GetData (tk, m_ordered_weights_mask)) != TK_Normal)
					return status;
				m_progress = 0;
			}
			m_stage++;
		}   nobreak;

		case 13: {
			if ((m_mask & TKO_Heuristic_Ordered_Weights) != 0) {
				while (m_progress < TKO_Heur_Order_Count) {
					if ((m_ordered_weights_mask & (1<<m_progress)) != 0 &&
						(status = GetData (tk, m_ordered_weights[m_progress])) != TK_Normal)
						return status;
					m_progress++;
				}
				m_progress = 0;
			}
			m_stage++;
		}   nobreak;

		case 14: {
			if ((m_mask & m_value & TKO_Heuristic_Internal_Polyline_Limit) != 0) {
				if ((status = GetData (tk, m_internal_polyline)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 15: {
			if ((m_mask & TKO_Heuristic_Selection_Level) ) {
				if ((status = GetData (tk, m_selection_level)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 16: {
			if ((m_mask & TKO_Heuristic_Culling) && (m_culling & TKO_Heur_Vector_Culling))	{
				if ((status = GetData (tk, m_vector, 3)) != TK_Normal)
					return status;
			}
			m_stage++;
		}	nobreak;
		case 17: {
			if ((m_mask & TKO_Heuristic_Culling) && (m_culling & TKO_Heur_Vector_Tolerance))	{
					if ((status = GetData (tk, m_vector_tolerance)) != TK_Normal)
						return status;
			}
			m_stage = -1;
		}	break;

		default:
			return tk.Error();
	}

	return status;
}




TK_Geometry_Options::~TK_Geometry_Options () {
}

TK_Status TK_Geometry_Options::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1605)
		m_mask = 0;
	if (tk.GetTargetVersion() < 1660)
		m_mask &= ~TKO_Geometry_Options_Camera_Relative;

	if (m_mask == 0)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_mask)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (tk.GetTargetVersion() >= 1660) {
				if ((status = PutData (tk, m_value)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geometry_Options_Orientation)) {
				if ((status = PutData (tk, m_orientation_count)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & TKO_Geometry_Options_Orientation)) {
				if ((status = PutData (tk, m_orientation, m_orientation_count)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}	break;

		

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Geometry_Options::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_mask)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if (tk.GetVersion() >= 1660) {
				if ((status = GetData (tk, m_value)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_mask & TKO_Geometry_Options_Orientation) {
				if ((status = GetData (tk, m_orientation_count)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if (m_mask & TKO_Geometry_Options_Orientation) {
				if ((status = GetData (tk, m_orientation, m_orientation_count)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}	break;

		default:
			return tk.Error();
	}

	return status;
}




TK_Status TK_Visibility::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;
	unsigned int	mask;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	mask = m_mask;
	if (tk.GetTargetVersion() < 1305)
		mask &= ~TKO_Geo_Extended2;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte = (unsigned char) (mask & 0x000000FF);

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			unsigned char       byte = (unsigned char) (m_value & 0x000000FF);

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((mask & TKO_Geo_Extended) != 0) {
				unsigned short      word = (unsigned short) ((mask>>TKO_Geo_Extended_Shift) & 0x0000FFFF);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((mask & TKO_Geo_Extended) != 0) {
				unsigned short      word = (unsigned short) ((m_value>>TKO_Geo_Extended_Shift) & 0x0000FFFF);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((mask & TKO_Geo_Extended2) != 0) {
				unsigned char      byte = (unsigned short) ((mask>>TKO_Geo_Extended2_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if ((mask & TKO_Geo_Extended2) != 0) {
				unsigned char      byte = (unsigned short) ((m_value>>TKO_Geo_Extended2_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Visibility::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_mask = byte;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_value = byte;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_mask |= word << TKO_Geo_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_value |= word << TKO_Geo_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char      byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended2_Shift;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_mask & TKO_Geo_Extended2) != 0) {
				unsigned char      byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_value |= byte << TKO_Geo_Extended2_Shift;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Status TK_Selectability::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if (tk.GetTargetVersion() < 650)
				return TK_Normal;

			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word = (unsigned short)(m_mask & 0x0000FFFF);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			else {
				unsigned char       byte = (unsigned char)(m_mask & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word = (unsigned short)(m_down & 0x0000FFFF);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			else {
				unsigned char       byte = (unsigned char)(m_down & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word = (unsigned short)(m_up & 0x0000FFFF);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			else {
				unsigned char       byte = (unsigned char)(m_up & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word = (unsigned short)(m_move_down & 0x0000FFFF);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			else {
				unsigned char       byte = (unsigned char)(m_move_down & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word = (unsigned short)(m_move_up & 0x0000FFFF);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			else {
				unsigned char       byte = (unsigned char)(m_move_up & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word = (unsigned short)(m_invisible & 0x0000FFFF);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			else {
				unsigned char       byte = (unsigned char)(m_invisible & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Selectability::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_mask = byte;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Geo_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_down = word;
			}
			else {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_down = byte;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_up = word;
			}
			else {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_up = byte;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_move_down = word;
			}
			else {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_move_down = byte;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_move_up = word;
			}
			else {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_move_up = byte;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_mask & TKO_Geo_Extended) != 0) {
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_invisible = word;
			}
			else {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_invisible = byte;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Status TK_Matrix::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif


	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, &m_matrix[0], 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, &m_matrix[4], 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, &m_matrix[8], 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = PutData (tk, &m_matrix[12], 3)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Matrix::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			m_matrix[3] = m_matrix[7] = m_matrix[11] = 0.0f;
			m_matrix[15] = 1.0f;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, &m_matrix[0], 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, &m_matrix[4], 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = GetData (tk, &m_matrix[8], 3)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = GetData (tk, &m_matrix[12], 3)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Status TK_Size::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			float           value = m_value;

			if (m_units != TKO_Generic_Size_Unspecified)
				value = -value;

			if ((status = PutData (tk, value)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_units != TKO_Generic_Size_Unspecified) {
				if ((status = PutData (tk, m_units)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Size::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_value)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_value < 0.0f) {
				if ((status = GetData (tk, m_units)) != TK_Normal)
					return status;
				m_value = -m_value;
			}
			else
				m_units = TKO_Generic_Size_Unspecified;

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Status TK_Enumerated::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif


	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage = 1;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_index)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Enumerated::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	if (m_stage != 0)
		return tk.Error();

	if ((status = GetData (tk, m_index)) != TK_Normal)
		return status;
	m_stage = -1;

	return status;
}



TK_Status TK_Linear_Pattern::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage = 1;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_pattern)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Linear_Pattern::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	if (m_stage != 0)
		return tk.Error();

	if ((status = GetData (tk, m_pattern)) != TK_Normal)
		return status;
	m_stage = -1;

	return status;
}



TK_Named::~TK_Named() {
	delete [] m_name;
}

TK_Status TK_Named::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1160)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_name_length > 0) {
				unsigned char       byte = (unsigned char)0xFF;

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			else if (Opcode() != TKE_Marker_Symbol) {
				unsigned short      word = (unsigned char)m_index;

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			else {
				unsigned char       byte = (unsigned char)m_index;

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_name_length > 0) {
				if(m_name_length < 255){
					unsigned char       byte = (unsigned char)m_name_length;

					if ((status = PutData (tk, byte)) != TK_Normal)
						return status;
				}
				else {
					unsigned char       byte = 255;

					if ((status = PutData (tk, byte)) != TK_Normal)
						return status;
				}
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if(m_name_length >= 255){
				if ((status = PutData (tk, m_name_length)) != TK_Normal)
					return status;
			}
			++m_stage;
		}	nobreak;

		case 4: {
			if (m_name_length > 0) {
				if ((status = PutData (tk, m_name, m_name_length)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Named::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			SetIndex ((int)byte);
			m_stage++;
		}   nobreak;

		case 1: {
			if ((GetIndex() & 0x00FF) == 0x00FF) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				SetName ((int)byte);
			}
			else if (Opcode() != TKE_Marker_Symbol) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				SetIndex ((int)byte<<8 | GetIndex());
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if(m_name_length == 255){
				int real_len;
				if ((status = GetData (tk, real_len)) != TK_Normal)
					return status;
				SetName (real_len);
			}
			++m_stage;
		}	nobreak;

		case 3: {
			if (m_name_length > 0) {
				if ((status = GetData (tk, m_name, m_name_length)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}


void TK_Named::SetName (int length) alter {
	m_name_length = length;
	delete [] m_name;
	m_name = new char [m_name_length + 1];
	m_name[m_name_length] = '\0';
}
void TK_Named::SetName (char const * name) alter {
	SetName ((int) strlen (name));
	strcpy (m_name, name);
}


void TK_Named::Reset () {
	delete [] m_name;
	m_name = null;
	m_name_length = 0;
	m_index = 0;

	BBaseOpcodeHandler::Reset();
}



TK_Conditions::~TK_Conditions() {
	delete [] m_string;
}

TK_Status TK_Conditions::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1150)   
		return status;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char      byte;
            
			if (m_length > 65535)
				byte = 255;                 
			else if (m_length > 253)
				byte = 254;                 
			else
				byte = (unsigned char)m_length;

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_length > 65535) {
				if ((status = PutData (tk, m_length)) != TK_Normal)
					return status;
			}
			else if (m_length > 253) {
				unsigned short      word = (unsigned short)m_length;

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if (m_length > 0) {
				if ((status = PutData (tk, m_string, m_length)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Conditions::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char  byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_length = byte;
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_length == 255) {                  
				if ((status = GetData (tk, m_length)) != TK_Normal)
					return status;
			}
			else if (m_length == 254) {             
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_length = word;
			}
			SetConditions (m_length);
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_string, m_length)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Conditions::SetConditions (int length) alter {
	m_length = length;
	delete [] m_string;
	m_string = new char [m_length + 1];
	m_string[m_length] = '\0';
}

void TK_Conditions::SetConditions (char const * conditions) alter {
	SetConditions ((int) strlen (conditions));
	strcpy (m_string, conditions);
}

void TK_Conditions::Reset () alter {
	delete [] m_string;
	m_string = null;
	m_length = 0;

	BBaseOpcodeHandler::Reset();
}



TK_User_Options::~TK_User_Options() {
	delete [] m_string;
}

TK_Status TK_User_Options::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if (m_length > 0) {
				if ((status = PutOpcode (tk)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_length > 0) {
				unsigned short      word;
                
				if (m_length > 65534)
					word = 65535;       
				else
					word = (unsigned short)m_length;

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_length > 65534) {
				if ((status = PutData (tk, m_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if (m_length > 0) {
				if ((status = PutData (tk, m_string, m_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if (m_indices != null) {
				if ((status = m_indices->Write (tk)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if (m_unicode != null) {
				if ((status = m_unicode->Write (tk)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_User_Options::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned short  word;

			if ((status = GetData (tk, word)) != TK_Normal)
				return status;
			m_length = word;
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_length == 65535) {        
				if ((status = GetData (tk, m_length)) != TK_Normal)
					return status;
			}
			set_options (m_length);
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_string, m_length)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_User_Options::set_options (int length) alter {
	m_length = length;
	delete [] m_string;
	m_string = new char [m_length + 1];
	m_string[m_length] = '\0';
}

void TK_User_Options::set_options (char const * options) alter {
	set_options ((int) strlen (options));
	strcpy (m_string, options);
}

void TK_User_Options::Reset () alter {
	delete [] m_string;
	m_string = null;
	m_length = 0;

	
	
	if (m_indices != null)
		m_indices->Reset();
	m_indices = null;
	if (m_unicode != null)
		m_unicode->Reset();
	m_unicode = null;

	BBaseOpcodeHandler::Reset();
}



TK_Unicode_Options::~TK_Unicode_Options() {
	delete [] m_string;
}

TK_Status TK_Unicode_Options::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 905)
		return status;

	switch (m_stage) {
		case 0: {
			if (m_length > 0) {
				if ((status = PutOpcode (tk)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_length > 0) {
				unsigned short      word;
                
				if (m_length > 65534)
					word = 65535;       
				else
					word = (unsigned short)m_length;

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_length > 65534) {
				if ((status = PutData (tk, m_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if (m_length > 0) {
				if ((status = PutData (tk, m_string, m_length)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Unicode_Options::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned short  word;

			if ((status = GetData (tk, word)) != TK_Normal)
				return status;
			m_length = word;
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_length == 65535) {        
				if ((status = GetData (tk, m_length)) != TK_Normal)
					return status;
			}
			SetOptions (m_length);
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_string, m_length)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Unicode_Options::SetOptions (int length) alter {
	m_length = length;
	delete [] m_string;
	m_string = new unsigned short [m_length + 1];
	m_string[m_length] = '\0';
}

void TK_Unicode_Options::SetOptions (unsigned short const * options) alter {
	unsigned short const *      sp = options;
	unsigned int                length;

	while (*sp != 0) ++sp;
	length = sp - options;

	SetOptions (length);
	memcpy (m_string, options, length*sizeof(unsigned short));
}

void TK_Unicode_Options::Reset () alter {
	delete [] m_string;
	m_string = null;
	m_length = 0;

	BBaseOpcodeHandler::Reset();
}



TK_User_Index::~TK_User_Index() {
	delete [] m_indices;
	delete [] m_values;
}

TK_Status TK_User_Index::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_indices, m_count)) != TK_Normal)
				return status;
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 3: {
			while (m_progress < m_count) {
				int     value = (int)m_values[m_progress];

				if ((status = PutData (tk, value)) != TK_Normal)
					return status;
				++m_progress;
			}
			m_progress = 0;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_User_Index::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_count)) != TK_Normal)
				return status;
			if (!validate_count (m_count))
				return tk.Error("bad User Index count");
			m_indices = new int [m_count];
			m_values = new long [m_count];
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_indices, m_count)) != TK_Normal)
				return status;
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 2: {
			while (m_progress < m_count) {
				int     value;

				if ((status = GetData (tk, value)) != TK_Normal)
					return status;
				m_values[m_progress++] = value;
			}
			m_progress = 0;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_User_Index::set_indices (int count) alter {
	m_count = count;
	delete [] m_indices;
	delete [] m_values;
	m_indices = new int [count];
	m_values = new long [count];
}

void TK_User_Index::set_indices (int count, int const * indices, POINTER_SIZED_INT const * values) alter {
	set_indices (count);
	memcpy (m_indices, indices, count * sizeof (int));
    
	if (sizeof(POINTER_SIZED_INT) == sizeof(long))
		memcpy (m_values, values, count * sizeof (long));
	else
		while(count-->=0)
			m_values[count]=(long)values[count];

}

void TK_User_Index::Reset () alter {
	delete [] m_indices;
	delete [] m_values;
	m_indices = null;
	m_values = null;
	m_count = 0;
	m_current_value = 0;

	BBaseOpcodeHandler::Reset();
}



TK_Status TK_User_Value::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			int         value = (int)m_value;
			if ((status = PutData (tk, value)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_User_Value::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			int         value;
			if ((status = GetData (tk, value)) != TK_Normal)
				return status;
			m_value = value;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}




TK_Camera::~TK_Camera() {
	delete [] m_name;
}

TK_Status TK_Camera::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (Opcode() == TKE_View && tk.GetTargetVersion() < 610)
		return status;      

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_projection)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_settings, 11)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_projection & TKO_Camera_Oblique_Y) != 0) {
				if ((status = PutData (tk, m_settings[11])) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_projection & TKO_Camera_Oblique_X) != 0) {
				if ((status = PutData (tk, m_settings[12])) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_projection & TKO_Camera_Near_Limit) != 0 && tk.GetTargetVersion() > 1000) {
				if ((status = PutData (tk, m_settings[13])) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if (Opcode() == TKE_View) {
				unsigned char       length = (unsigned char)m_length;

				if ((status = PutData (tk, length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if (Opcode() == TKE_View && m_length > 0) {
				if ((status = PutData (tk, m_name, m_length)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Camera::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_projection)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_settings, 11)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_projection & TKO_Camera_Oblique_Y) != 0) {
				if ((status = GetData (tk, m_settings[11])) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_projection & TKO_Camera_Oblique_X) != 0) {
				if ((status = GetData (tk, m_settings[12])) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_projection & TKO_Camera_Near_Limit) != 0) {
				if ((status = GetData (tk, m_settings[13])) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if (Opcode() == TKE_View) {
				unsigned char       length;

				if ((status = GetData (tk, length)) != TK_Normal)
					return status;
				SetView ((int)length);
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if (Opcode() == TKE_View != 0 && m_length > 0) {
				if ((status = GetData (tk, m_name, m_length)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Camera::set_name (int length) alter {
	m_length = length;
	delete [] m_name;
	m_name = new char [m_length + 1];
	m_name[m_length] = '\0';
}

void TK_Camera::set_name (char const * name) alter {
	set_name ((int) strlen (name));
	strcpy (m_name, name);
}




TK_Status TK_Window::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_window, 4)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Window::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_window, 4)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Text_Font::~TK_Text_Font() {
	delete [] m_names;
}

TK_Status TK_Text_Font::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;
	int				mask = m_mask;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1210)
		mask &= 0x00007FFF;
	else if (tk.GetTargetVersion() < 1220)
		mask &= 0x0003FFFF;
	else if (tk.GetTargetVersion() < 1325)
		mask &= 0x001FFFFF;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte = (unsigned char)(mask & 0x000000FF);

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			unsigned char       byte = (unsigned char)(m_value & 0x000000FF);

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((mask & TKO_Font_Extended) != 0) {
				unsigned char       byte = (unsigned char)((mask>>TKO_Font_Extended_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((mask & TKO_Font_Extended) != 0) {
				unsigned char       byte = (unsigned char)((m_value>>TKO_Font_Extended_Shift) & 0x000000FF);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((mask & TKO_Font_Extended2) != 0) {
				unsigned short      word = (unsigned short)((mask>>TKO_Font_Extended2_Shift) & 0x0000FFFF);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if ((mask & TKO_Font_Extended2) != 0) {
				unsigned short      word = (unsigned short)((m_value>>TKO_Font_Extended2_Shift) & 0x0000FFFF);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if ((mask & m_value & TKO_Font_Names) != 0) {
				
				unsigned char       byte = (unsigned char)m_names_length;
				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 8: {
			if ((mask & m_value & TKO_Font_Names) != 0) {
				if ((status = PutData (tk, m_names, m_names_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if ((mask & m_value & TKO_Font_Size) != 0) {
				if ((status = PutData (tk, m_size)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 10: {
			if ((mask & m_value & TKO_Font_Size) != 0) {
				if ((status = PutData (tk, m_size_units)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 11: {
			if ((mask & m_value & TKO_Font_Size_Tolerance) != 0) {
				if ((status = PutData (tk, m_tolerance)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 12: {
			if ((mask & m_value & TKO_Font_Size_Tolerance) != 0) {
				if ((status = PutData (tk, m_tolerance_units)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 13: {
			if ((mask & m_value & TKO_Font_Transforms) != 0) {
				if ((status = PutData (tk, m_transforms)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 14: {
			if ((mask & m_value & TKO_Font_Rotation) != 0) {
				if ((status = PutData (tk, m_rotation)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 15: {
			if ((mask & m_value & TKO_Font_Slant) != 0) {
				if ((status = PutData (tk, m_slant)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 16: {
			if ((mask & m_value & TKO_Font_Width_Scale) != 0) {
				if ((status = PutData (tk, m_width_scale)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 17: {
			if ((mask & m_value & TKO_Font_Extra_Space) != 0) {
				if ((status = PutData (tk, m_extra_space)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 18: {
			if ((mask & m_value & TKO_Font_Extra_Space) != 0) {
				if ((status = PutData (tk, m_space_units)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 19: {
			if ((mask & m_value & TKO_Font_Line_Spacing) != 0) {
				if ((status = PutData (tk, m_line_spacing)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 20: {
			if ((mask & m_value & TKO_Font_Greeking_Limit) != 0) {
				if ((status = PutData (tk, m_greeking_limit)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 21: {
			if ((mask & m_value & TKO_Font_Greeking_Limit) != 0) {
				if ((status = PutData (tk, m_greeking_units)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 22: {
			if ((mask & m_value & TKO_Font_Greeking_Mode) != 0) {
				if ((status = PutData (tk, m_greeking_mode)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 23: {
			if ((mask & TKO_Font_Renderer) != 0) {
				char			byte;

				if (tk.GetTargetVersion() < 1325)
					byte = m_renderers[0] & 0x0F;
				else
					byte = (m_renderers[0] & 0x0F) | ((m_renderers[1] & 0x0F) << 4);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 24: {
			if ((mask & TKO_Font_Renderer) != 0 && tk.GetTargetVersion() >= 1325) {
				if ((status = PutData (tk, m_renderer_cutoff)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 25: {
			if ((mask & TKO_Font_Renderer) != 0 && tk.GetTargetVersion() >= 1325) {
				if ((status = PutData (tk, m_renderer_cutoff_units)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 26: {
			if ((mask & TKO_Font_Preference) != 0) {
				char			byte;

				byte = (m_preferences[0] & 0x0F) | ((m_preferences[1] & 0x0F) << 4);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 27: {
			if ((mask & TKO_Font_Preference) != 0) {
				if ((status = PutData (tk, m_preference_cutoff)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 28: {
			if ((mask & TKO_Font_Preference) != 0) {
				if ((status = PutData (tk, m_preference_cutoff_units)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Text_Font::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
	unsigned char   byte;
	unsigned short  word;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_mask = byte;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_value = byte;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((m_mask & TKO_Font_Extended) != 0) {
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_mask |= byte << TKO_Font_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((m_mask & TKO_Font_Extended) != 0) {
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_value |= byte << TKO_Font_Extended_Shift;
			}
			m_stage++;
		}   nobreak;

		case 4: {
			if ((m_mask & TKO_Font_Extended2) != 0) {
				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_mask |= word << TKO_Font_Extended2_Shift;
			}
			m_stage++;
		}   nobreak;

		case 5: {
			if ((m_mask & TKO_Font_Extended2) != 0) {
				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_value |= word << TKO_Font_Extended2_Shift;
			}
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_mask & m_value & TKO_Font_Names) != 0) {
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_names_length = byte;
				
				SetNames (m_names_length);
			}
			m_stage++;
		}   nobreak;

		case 7: {
			if ((m_mask & m_value & TKO_Font_Names) != 0) {
				if ((status = GetData (tk, m_names, m_names_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 8: {
			if ((m_mask & m_value & TKO_Font_Size) != 0) {
				if ((status = GetData (tk, m_size)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 9: {
			if ((m_mask & m_value & TKO_Font_Size) != 0) {
				if ((status = GetData (tk, m_size_units)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 10: {
			if ((m_mask & m_value & TKO_Font_Size_Tolerance) != 0) {
				if ((status = GetData (tk, m_tolerance)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 11: {
			if ((m_mask & m_value & TKO_Font_Size_Tolerance) != 0) {
				if ((status = GetData (tk, m_tolerance_units)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 12: {
			if ((m_mask & m_value & TKO_Font_Transforms) != 0) {
				if ((status = GetData (tk, m_transforms)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 13: {
			if ((m_mask & m_value & TKO_Font_Rotation) != 0) {
				if ((status = GetData (tk, m_rotation)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 14: {
			if ((m_mask & m_value & TKO_Font_Slant) != 0) {
				if ((status = GetData (tk, m_slant)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 15: {
			if ((m_mask & m_value & TKO_Font_Width_Scale) != 0) {
				if ((status = GetData (tk, m_width_scale)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 16: {
			if ((m_mask & m_value & TKO_Font_Extra_Space) != 0) {
				if ((status = GetData (tk, m_extra_space)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 17: {
			if ((m_mask & m_value & TKO_Font_Extra_Space) != 0) {
				if ((status = GetData (tk, m_space_units)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 18: {
			if ((m_mask & m_value & TKO_Font_Line_Spacing) != 0) {
				if ((status = GetData (tk, m_line_spacing)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 19: {
			if ((m_mask & m_value & TKO_Font_Greeking_Limit) != 0) {
				if ((status = GetData (tk, m_greeking_limit)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 20: {
			if ((m_mask & m_value & TKO_Font_Greeking_Limit) != 0) {
				if ((status = GetData (tk, m_greeking_units)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 21: {
			if ((m_mask & m_value & TKO_Font_Greeking_Mode) != 0) {
				if ((status = GetData (tk, m_greeking_mode)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 22: {
			if ((m_mask & TKO_Font_Renderer) != 0) {
				char			byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;

				if (tk.GetVersion() < 1325)
					m_renderers[0] = m_renderers[1] = (int)byte;
				else {
					if ((m_renderers[0] = (int)byte & 0x0F) == 0x0F)
						m_renderers[0] = TKO_Font_Renderer_Undefined;
					if ((m_renderers[1] = (int)(byte >> 4) & 0x0F) == 0x0F)
						m_renderers[1] = TKO_Font_Renderer_Undefined;
				}
			}
			m_stage++;
		}   nobreak;

		case 23: {
			if ((m_mask & TKO_Font_Renderer) != 0 && tk.GetVersion() >= 1325) {
				if ((status = GetData (tk, m_renderer_cutoff)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 24: {
			if ((m_mask & TKO_Font_Renderer) != 0 && tk.GetVersion() >= 1325) {
				if ((status = GetData (tk, m_renderer_cutoff_units)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 25: {
			if ((m_mask & TKO_Font_Preference) != 0) {
				char			byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;

				if ((m_preferences[0] = (int)byte & 0x0F) == 0x0F)
					m_preferences[0] = TKO_Font_Preference_Undefined;
				if ((m_preferences[1] = (int)(byte >> 4) & 0x0F) == 0x0F)
					m_preferences[1] = TKO_Font_Preference_Undefined;
			}
			m_stage++;
		}   nobreak;

		case 26: {
			if ((m_mask & TKO_Font_Preference) != 0) {
				if ((status = GetData (tk, m_preference_cutoff)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 27: {
			if ((m_mask & TKO_Font_Preference) != 0) {
				if ((status = GetData (tk, m_preference_cutoff_units)) != TK_Normal)
					return status;
			}
			m_stage = -1;
		}   break;
		default:
			return tk.Error();
	}

	return status;
}


void TK_Text_Font::Reset () alter {
	delete [] m_names;
	m_names = null;
	m_names_length = 0;
	m_renderers[0] = m_renderers[1] = -1;
	m_preferences[0] = m_preferences[1] = -1;
	m_renderer_cutoff = -1.0f;
	m_preference_cutoff = -1.0f;

	BBaseOpcodeHandler::Reset();
}

void TK_Text_Font::set_names (int length) alter {
	m_names_length = length;
	delete [] m_names;
	m_names = new char [m_names_length + 1];
	m_names[m_names_length] = '\0';
}
void TK_Text_Font::set_names (char const * names) alter {
	if (names != null) {
		set_names ((int) strlen (names));
		strcpy (m_names, names);
	}
	else {
		m_names_length = 0;
		delete [] m_names;
		m_names = null;
	}
}



TK_Texture::~TK_Texture() {
	delete [] m_name;
	delete [] m_image;
	delete [] m_transform;
}

TK_Status TK_Texture::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;
	int             flags = m_flags;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1165)
		flags &= 0x00007FFF;

	if (m_flags == 0 && streq (m_name, m_image)) {
		
		return status;
	}

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 1: {
			if (m_name_length < 255) {
				if ((status = PutData (tk, (unsigned char)m_name_length)) != TK_Normal)
					return status;
			}
			else {
				if (m_progress == 0) {
					if ((status = PutData (tk, (unsigned char)255)) != TK_Normal)
						return status;
					m_progress++;
				}
				if ((status = PutData (tk, m_name_length)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_name, m_name_length)) != TK_Normal)
				return status;
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 3: {
			if (m_image_length < 255) {
				if ((status = PutData (tk, (unsigned char)m_image_length)) != TK_Normal)
					return status;
			}
			else {
				if (m_progress == 0) {
					if ((status = PutData (tk, (unsigned char)255)) != TK_Normal)
						return status;
					m_progress++;
				}
				if ((status = PutData (tk, m_image_length)) != TK_Normal)
					return status;
			}

			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = PutData (tk, m_image, m_image_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			unsigned short      word = (unsigned short)flags;

			if ((status = PutData (tk, word)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 6: {
			if ((flags & TKO_Texture_Extended) != 0) {
				unsigned short      word = (unsigned short)(flags >> TKO_Texture_Extended_Shift);

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;


		case 7: {
			if ((flags & TKO_Texture_Param_Source) != 0)
				if ((status = PutData (tk, m_param_source)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 8: {
			if ((flags & TKO_Texture_Tiling) != 0)
				if ((status = PutData (tk, m_tiling)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 9: {
			if ((flags & TKO_Texture_Interpolation) != 0)
				if ((status = PutData (tk, m_interpolation)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 10: {
			if ((flags & TKO_Texture_Decimation) != 0) {
				char		decimation = m_decimation;

				if (tk.GetTargetVersion() < 1410 && decimation == TKO_Texture_Filter_Anisotropic)
					decimation = TKO_Texture_Filter_MipMap;

				if ((status = PutData (tk, decimation)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 11: {
			if ((flags & TKO_Texture_Red_Mapping) != 0)
				if ((status = PutData (tk, m_red_mapping)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 12: {
			if ((flags & TKO_Texture_Green_Mapping) != 0)
				if ((status = PutData (tk, m_green_mapping)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 13: {
			if ((flags & TKO_Texture_Blue_Mapping) != 0)
				if ((status = PutData (tk, m_blue_mapping)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 14: {
			if ((flags & TKO_Texture_Alpha_Mapping) != 0)
				if ((status = PutData (tk, m_alpha_mapping)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 15: {
			if ((flags & TKO_Texture_Param_Function) != 0)
				if ((status = PutData (tk, m_param_function)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 16: {
			if ((flags & TKO_Texture_Layout) != 0)
				if ((status = PutData (tk, m_layout)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 17: {
			if ((flags & TKO_Texture_Value_Scale) != 0)
				if ((status = PutData (tk, m_value_scale, 2)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 18: {
			if ((flags & TKO_Texture_Transform) != 0) {
				unsigned char       byte = (unsigned char) strlen (m_transform);

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 19: {
			if ((flags & TKO_Texture_Transform) != 0)
				if ((status = PutData (tk, m_transform, (int) strlen (m_transform))) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 20: {
			if ((flags & (TKO_Texture_Modulate|TKO_Texture_Decal)) != 0)
				if ((status = PutData (tk, m_apply_mode)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 21: {
			if ((flags & TKO_Texture_Param_Offset) != 0)
				if ((status = PutData (tk, m_param_offset)) != TK_Normal)
					return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Texture::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			m_progress = -1;	
			m_stage++;
		}	nobreak;

		case 1: {
			if (m_progress == -1) {
				unsigned char       byte;
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_progress = byte;
			}
			if (m_progress == 255) {
				if ((status = GetData (tk, m_name_length)) != TK_Normal)
					return status;
			}
			else
				m_name_length = m_progress;
			SetName (m_name_length);       
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_name, m_name_length)) != TK_Normal)
				return status;
			m_progress = -1;	
			m_stage++;
		}   nobreak;

		case 3: {
			if (m_progress == -1) {
				unsigned char       byte;
				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				m_progress = byte;
			}
			if (m_progress == 255) {
				if ((status = GetData (tk, m_image_length)) != TK_Normal)
					return status;
			}
			else
				m_image_length = m_progress;
			SetImage (m_image_length);      
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = GetData (tk, m_image, m_image_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			unsigned short      word;

			if ((status = GetData (tk, word)) != TK_Normal)
				return status;
			m_flags = word;
			m_stage++;
		}   nobreak;

		case 6: {
			if ((m_flags & TKO_Texture_Extended) != 0) {
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_flags |= (int)word << TKO_Texture_Extended_Shift;
			}
			m_stage++;
		}   nobreak;


		case 7: {
			if ((m_flags & TKO_Texture_Param_Source) != 0)
				if ((status = GetData (tk, m_param_source)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 8: {
			if ((m_flags & TKO_Texture_Tiling) != 0)
				if ((status = GetData (tk, m_tiling)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 9: {
			if ((m_flags & TKO_Texture_Interpolation) != 0)
				if ((status = GetData (tk, m_interpolation)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 10: {
			if ((m_flags & TKO_Texture_Decimation) != 0)
				if ((status = GetData (tk, m_decimation)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 11: {
			if ((m_flags & TKO_Texture_Red_Mapping) != 0)
				if ((status = GetData (tk, m_red_mapping)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 12: {
			if ((m_flags & TKO_Texture_Green_Mapping) != 0)
				if ((status = GetData (tk, m_green_mapping)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 13: {
			if ((m_flags & TKO_Texture_Blue_Mapping) != 0)
				if ((status = GetData (tk, m_blue_mapping)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 14: {
			if ((m_flags & TKO_Texture_Alpha_Mapping) != 0)
				if ((status = GetData (tk, m_alpha_mapping)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 15: {
			if ((m_flags & TKO_Texture_Param_Function) != 0)
				if ((status = GetData (tk, m_param_function)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 16: {
			if ((m_flags & TKO_Texture_Layout) != 0)
				if ((status = GetData (tk, m_layout)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 17: {
			if ((m_flags & TKO_Texture_Value_Scale) != 0)
				if ((status = GetData (tk, m_value_scale, 2)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 18: {
			if ((m_flags & TKO_Texture_Transform) != 0) {
				unsigned char       byte;

				if ((status = GetData (tk, byte)) != TK_Normal)
					return status;
				SetTransform (m_progress = (int)byte);
			}
			m_stage++;
		}   nobreak;

		case 19: {
			if ((m_flags & TKO_Texture_Transform) != 0) {
				if ((status = GetData (tk, m_transform, m_progress)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 20: {
			if ((m_flags & (TKO_Texture_Modulate|TKO_Texture_Decal)) != 0)
				if ((status = GetData (tk, m_apply_mode)) != TK_Normal)
					return status;
			m_stage++;
		}   nobreak;

		case 21: {
			if ((m_flags & TKO_Texture_Param_Offset) != 0)
				if ((status = GetData (tk, m_param_offset)) != TK_Normal)
					return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Texture::Reset () alter {
	m_name_length = 0;
	m_image_length = 0;
	delete [] m_transform;
	m_transform = null;
	m_flags = 0;          
	m_substage = 0;			
	m_param_source = 0;     
	m_interpolation = 0;    
	m_decimation = 0;       
	m_red_mapping = 0;      
	m_green_mapping = 0;    
	m_blue_mapping = 0;     
	m_alpha_mapping = 0;    
	m_param_function = 0;   
	m_layout = 0;           
	m_tiling = 0;           
	m_value_scale[0] = m_value_scale[1] = 0;   
	m_apply_mode = 0;    
	m_param_offset = 0;	    

	BBaseOpcodeHandler::Reset();
}

void TK_Texture::set_name (int length) alter {
	m_name_length = length;
	delete [] m_name;
	m_name = new char [m_name_length + 1];
	m_name[m_name_length] = '\0';
}
void TK_Texture::set_name (char const * name) alter {
	set_name ((int) strlen (name));
	strcpy (m_name, name);
}

void TK_Texture::set_image (int length) alter {
	m_image_length = length;
	delete [] m_image;
	m_image = new char [m_image_length + 1];
	m_image[m_image_length] = '\0';
}
void TK_Texture::set_image (char const * image) alter {
	set_image ((int) strlen (image));
	strcpy (m_image, image);
}


void TK_Texture::set_transform (int length) alter {
	delete [] m_transform;
	m_transform = new char [length + 1];
	m_transform[length] = '\0';
}
void TK_Texture::set_transform (char const * transform) alter {
	set_transform ((int) strlen (transform));
	strcpy (m_transform, transform);
}



TK_Glyph_Definition::~TK_Glyph_Definition() {
	delete [] m_name;
	delete [] m_data;
}

TK_Status TK_Glyph_Definition::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif


	if (tk.GetTargetVersion() < 1160)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			
			unsigned char       byte = (unsigned char)m_name_length;

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_name, m_name_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			
			unsigned short      word = (unsigned short)m_size;

			if ((status = PutData (tk, word)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = PutData (tk, m_data, m_size)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Glyph_Definition::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			SetName ((int)byte);       
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_name, m_name_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			unsigned short      word;

			if ((status = GetData (tk, word)) != TK_Normal)
				return status;
			SetDefinition ((int)word);      
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = GetData (tk, m_data, m_size)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Glyph_Definition::Reset () alter {
	m_name_length = 0;
	m_size = 0;
	delete [] m_data;
	m_data = null;

	BBaseOpcodeHandler::Reset();
}

void TK_Glyph_Definition::SetName (int length) alter {
	m_name_length = length;
	delete [] m_name;
	m_name = new char [m_name_length + 1];
	m_name[m_name_length] = '\0';
}
void TK_Glyph_Definition::SetName (char const * name) alter {
	SetName ((int) strlen (name));
	strcpy (m_name, name);
}

void TK_Glyph_Definition::SetDefinition (int length, char const * data) alter {
	m_size = length;
	delete [] m_data;
	m_data = new char [m_size+1];
	if (data != null)
		memcpy (m_data, data, length);
}



TK_Line_Style::~TK_Line_Style() {
	delete [] m_name;
	delete [] m_definition;
}

TK_Status TK_Line_Style::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 1160)
		return TK_Normal;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if(m_name_length < 255){
				unsigned char       byte = (unsigned char)m_name_length;

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			else {
				unsigned char       byte = 255;

				if ((status = PutData (tk, byte)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 2: {
			if(m_name_length >= 255){
				if ((status = PutData (tk, m_name_length)) != TK_Normal)
					return status;
			}
			++m_stage;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_name, m_name_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 4: {
			
			unsigned short      word = (unsigned short)m_definition_length;

			if ((status = PutData (tk, word)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 5: {
			if ((status = PutData (tk, m_definition, m_definition_length)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Line_Style::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			SetName ((int)byte);       
			m_stage++;
		}   nobreak;

		case 1: {
			if(m_name_length == 255){
				int real_len;
				if ((status = GetData (tk, real_len)) != TK_Normal)
					return status;
				SetName(real_len);
			}
			++m_stage;
		}	nobreak;

		case 2: {
			if ((status = GetData (tk, m_name, m_name_length)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			unsigned short      word;

			if ((status = GetData (tk, word)) != TK_Normal)
				return status;
			SetDefinition ((int)word);      
			m_stage++;
		}   nobreak;

		case 4: {
			if ((status = GetData (tk, m_definition, m_definition_length)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_Line_Style::Reset () alter {
	m_name_length = 0;
	m_definition_length = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_Line_Style::SetName (int length) alter {
	m_name_length = length;
	delete [] m_name;
	m_name = new char [m_name_length + 1];
	m_name[m_name_length] = '\0';
}
void TK_Line_Style::SetName (char const * name) alter {
	SetName ((int) strlen (name));
	strcpy (m_name, name);
}

void TK_Line_Style::SetDefinition (int length) alter {
	m_definition_length = length;
	delete [] m_definition;
	m_definition = new char [m_definition_length + 1];
	m_definition[m_definition_length] = '\0';
}
void TK_Line_Style::SetDefinition (char const * definition) alter {
	SetDefinition ((int) strlen (definition));
	strcpy (m_definition, definition);
}



TK_Status TK_Clip_Rectangle::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_options)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_rect, 4)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Clip_Rectangle::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_options)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_rect, 4)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}


void TK_Clip_Rectangle::Reset () alter {
	m_options = 0;

	BBaseOpcodeHandler::Reset();
}


TK_Clip_Region::~TK_Clip_Region() {
	delete [] m_points;
}

TK_Status TK_Clip_Region::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_options)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_count)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_points, 3*m_count)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_Clip_Region::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_options)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			int         count;

			if ((status = GetData (tk, count)) != TK_Normal)
				return status;
			if (!validate_count (count))
				return tk.Error("bad Clip Region count");
			SetPoints (count);   
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_points, 3*m_count)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}


void TK_Clip_Region::Reset () alter {
	delete [] m_points;
	m_points = null;
	m_count = 0;
	m_options = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_Clip_Region::SetPoints (int count, float const * points) alter {
	if (m_count != count) {
		delete [] m_points;
		m_points = null;
		m_count = count;
		if (m_count)
			m_points = new float [3*m_count];
	}
	if (points != null)
		memcpy (m_points, points, m_count*3*sizeof(float));
}



TK_User_Data::~TK_User_Data() {
	delete [] m_data;
}

TK_Status TK_User_Data::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif
 
	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk, 0)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_size)) != TK_Normal)
				return status;
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_data, m_size)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, (unsigned char)TKE_Stop_User_Data)) != TK_Normal)
				return status;
 
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_User_Data::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_size)) != TK_Normal)
				return status;
			set_data (m_size);      
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_data, m_size)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			unsigned char       stop_code;

			if ((status = GetData (tk, stop_code)) != TK_Normal)
				return status;

			if (stop_code != TKE_Stop_User_Data)    
				return tk.Error();

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

TK_Status TK_User_Data::Execute (BStreamFileToolkit & tk) alter {
	////LOGI("TK_User_Data::Execute");
	UNREFERENCED(tk);
	return TK_Normal;
}


void TK_User_Data::Reset () alter {
	delete [] m_data;
	m_data = null;
	m_size = 0;
	m_buffer_size = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_User_Data::set_data (int count, char const * bytes) alter {
	m_buffer_size = m_size = count;
	delete [] m_data;
	m_data = new char [m_size];
	if (bytes != null)
		memcpy (m_data, bytes, m_size*sizeof(char));
}

void TK_User_Data::Resize (int size)
{
	char * old_data = m_data;
	int old_size = m_size;

	m_data = new char[size];
	m_buffer_size = size; 

	if (old_size > 0) {
		
		if (old_size > size)
			old_size = size;

		memcpy(m_data,old_data,old_size);
	}

	delete[] old_data;
}

void TK_User_Data::SetSize (int size)
{

	if (size > m_buffer_size)
		Resize(size);

	m_size = size;
}



TK_XML::~TK_XML() {
	delete [] m_data;
}

TK_Status TK_XML::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	if (tk.GetTargetVersion() < 705)  
		return status;

	switch (m_stage) {
		case 0: {
			if ((status = PutOpcode (tk, 0)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, m_size)) != TK_Normal)
				return status;
			m_progress = 0;
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, m_data, m_size)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_XML::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			if ((status = GetData (tk, m_size)) != TK_Normal)
				return status;
			SetXML (m_size);      
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = GetData (tk, m_data, m_size)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

TK_Status TK_XML::Execute (BStreamFileToolkit & tk) alter {
	UNREFERENCED(tk);
	////LOGI("TK_XML::Execute");
	return TK_Normal;
}

void TK_XML::Reset () alter {
	delete [] m_data;
	m_data = null;
	m_size = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_XML::SetXML (int count, char const * bytes) alter {
	m_size = count;
	delete [] m_data;
	m_data = new char [m_size];
//    m_data = new char [869];
	if (bytes != null)
		memcpy (m_data, bytes, m_size*sizeof(char));
}

void TK_XML::AppendXML (int count, char const * bytes) alter {
	char *              old_data = m_data;
	int                 old_size = m_size;

	m_size += count;
	m_data = new char [m_size];
	if (old_size > 0)
		memcpy (m_data, old_data, old_size*sizeof(char));
	if (bytes != null)
		memcpy (m_data+old_size, bytes, count*sizeof(char));
	delete [] old_data;
}




TK_External_Reference::~TK_External_Reference() {
	delete [] m_string;
}

TK_Status TK_External_Reference::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if (tk.GetTargetVersion() < 805)    
				return TK_Normal;
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte;

			if (m_length > 65535)
				byte = 255;                     
			else if (m_length > 253)
				byte = 254;                     
			else
				byte = (unsigned char)m_length;

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_length > 65535) {
				if ((status = PutData (tk, m_length)) != TK_Normal)
					return status;
			}
			else if (m_length > 253) {
				unsigned short      word = (unsigned short)m_length;

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_string, m_length)) != TK_Normal)
				return status;

			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
				LogDebug (tk, "[");
				LogDebug (tk, m_string);
				LogDebug (tk, "]");
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_External_Reference::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_length = byte;

			m_stage++;
		}   nobreak;

		case 1: {
			if (m_length == 255) {                      
				if ((status = GetData (tk, m_length)) != TK_Normal)
					return status;
			}
			else if (m_length == 254) {                 
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_length = word;
			}
        
			SetString (m_length);      
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_string, m_length)) != TK_Normal)
				return status;

			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
				LogDebug (tk, "[");
				LogDebug (tk, m_string);
				LogDebug (tk, "]");
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Status TK_External_Reference::Execute (BStreamFileToolkit & tk) alter {
	
	////LOGI("TK_External_Reference::Execute");
	if (strchr (m_string, ':') || m_string[0] == '/')   
		tk.AddExternalReference (m_string, tk.CurrentSegment());
	else {
		wchar_t		reference[4096];
		char *		string = m_string;

		reference[0] = '\0';
		if (tk.GetCurrentFile() != null) {
			wcscpy (reference, tk.GetCurrentFile());

			wchar_t *	cp = wcsrchr2 (reference, '/', '\\');	

			if (cp != null) {
				wchar_t const * const	fs = wcschr2 (reference, '/', '\\');	

				cp[1] = '\0';   

				while (string[0] == '.') {
					if (string[1] == '/' || string[1] == '\\')   
						string += 2;
					else if (string[1] == '.' && (string[2] == '/' || string[2] == '\\')) {
						
						if (cp != fs) {		
							*cp = '\0';		
							cp = wcsrchr2 (reference, '/', '\\');	
							cp[1] = '\0';   

							string += 3;	
						}
						else {				
							while (string[0] == '.') {
								if (string[1] == '/' || string[1] == '\\')
									string += 2;			
								else if (string[1] == '.' && (string[2] == '/' || string[2] == '\\'))
									string += 3;			
								else
									break;					
							}
							break;
						}
					}
					else
						break;      
				}
			}
			else
				reference[0] = '\0';
		}
		wcsstrcpy (reference + wcslen(reference), string);	
		tk.AddExternalReference (reference, tk.CurrentSegment());
	}

	return TK_Normal;
}

void TK_External_Reference::Reset () alter {
	m_length = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_External_Reference::SetString (int length) alter {
	m_length = length;
	if (m_length >= m_allocated) {
		delete [] m_string;
		m_string = new char [m_allocated = m_length + 16];
	}
	m_string[m_length] = '\0';
}

void TK_External_Reference::SetString (char const * string) alter {
	SetString ((int) strlen (string));
	strcpy (m_string, string);
}




TK_External_Reference_Unicode::~TK_External_Reference_Unicode() {
	delete [] m_string;
}

TK_Status TK_External_Reference_Unicode::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

	H_UTF8 utf8(m_string);
	char const * str = (char const*)utf8.encodedText();
	int len = (int)strlen(str) +1;

	switch (m_stage) {
		case 0: {
			if (tk.GetTargetVersion() < 1650)    
				return TK_Normal;
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			if ((status = PutData (tk, len)) != TK_Normal)
				return status;

			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = PutData (tk, str, len)) != TK_Normal)
				return status;

			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
				LogDebug (tk, "[");
				LogDebug (tk, str);
				LogDebug (tk, "]");
			}

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_External_Reference_Unicode::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;

	switch (m_stage) {
		case 0: {
			int len = 0;
			if ((status = GetData (tk, len)) != TK_Normal)
				return status;
        
			SetString (len);      
			m_stage++;
		}   nobreak;

		case 1: {
			char * tmp = new char[m_length];
			if ((status = GetData (tk, tmp, m_length)) != TK_Normal) {
				delete [] tmp;
				return status;
			}

			H_UTF8 utf8;
			utf8.encodedText((utf8_char const *)tmp);
			H_WCS wcs(utf8);
			SetString((int)wcslen(wcs.encodedText()) +1);
			SetString((wchar_t const*)wcs.encodedText());

			if (tk.GetLogging() && tk.GetLoggingOptions (TK_Logging_Segment_Names)) {
				LogDebug (tk, "[");
				LogDebug (tk, tmp);
				LogDebug (tk, "]");
			}

			delete [] tmp;

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}



TK_Status TK_External_Reference_Unicode::Execute (BStreamFileToolkit & tk) alter {
	
	////LOGI("TK_External_Reference_Unicode::Execute");
	if (wcschr (m_string, L':') || m_string[0] == L'/')   
		tk.AddExternalReference (m_string, tk.CurrentSegment());
	else {
		wchar_t		reference[4096];
		wchar_t *	string = m_string;

		reference[0] = L'\0';
		if (tk.GetCurrentFile() != null) {
			wcscpy (reference, tk.GetCurrentFile());

			wchar_t *	cp = wcsrchr2 (reference, '/', '\\');	

			if (cp != null) {
				wchar_t const * const	fs = wcschr2 (reference, '/', '\\');	

				cp[1] = '\0';   

				while (string[0] == '.') {
					if (string[1] == '/' || string[1] == '\\')   
						string += 2;
					else if (string[1] == '.' && (string[2] == '/' || string[2] == '\\')) {
						
						if (cp != fs) {		
							*cp = '\0';		
							cp = wcsrchr2 (reference, '/', '\\');	
							cp[1] = '\0';   

							string += 3;	
						}
						else {				
							while (string[0] == '.') {
								if (string[1] == '/' || string[1] == '\\')
									string += 2;			
								else if (string[1] == '.' && (string[2] == '/' || string[2] == '\\'))
									string += 3;			
								else
									break;					
							}
							break;
						}
					}
					else
						break;      
				}
			}
			else
				reference[0] = '\0';
		}
		wcscpy (reference + wcslen(reference), string);	
		tk.AddExternalReference (reference, tk.CurrentSegment());
	}

	return TK_Normal;
}

void TK_External_Reference_Unicode::Reset () alter {
	m_length = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_External_Reference_Unicode::SetString (int length) alter {
	m_length = length;
	if (m_length >= m_allocated) {
		delete [] m_string;
		m_string = new wchar_t [m_allocated = m_length + 16];
	}
	m_string[m_length] = '\0';
}

#ifdef _MSC_VER
void TK_External_Reference_Unicode::SetString (unsigned short const * string) alter {
	SetString((wchar_t const *) string);
}
#endif

void TK_External_Reference_Unicode::SetString (wchar_t const * string) alter {
	SetString ((int) wcslen (string));
	wcscpy (m_string, string);
}




TK_URL::~TK_URL() {
	delete [] m_string;
}

TK_Status TK_URL::Write (BStreamFileToolkit & tk) alter {
#ifndef BSTREAM_READONLY
	TK_Status       status = TK_Normal;

#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return WriteAscii (tk);
#endif

	switch (m_stage) {
		case 0: {
			if (tk.GetTargetVersion() < 805)    
				return TK_Normal;
			if ((status = PutOpcode (tk)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 1: {
			unsigned char       byte;

			if (m_length > 65535)
				byte = 255;                     
			else if (m_length > 253)
				byte = 254;                     
			else
				byte = (unsigned char)m_length;

			if ((status = PutData (tk, byte)) != TK_Normal)
				return status;
			m_stage++;
		}   nobreak;

		case 2: {
			if (m_length > 65535) {
				if ((status = PutData (tk, m_length)) != TK_Normal)
					return status;
			}
			else if (m_length > 253) {
				unsigned short      word = (unsigned short)m_length;

				if ((status = PutData (tk, word)) != TK_Normal)
					return status;
			}
			m_stage++;
		}   nobreak;

		case 3: {
			if ((status = PutData (tk, m_string, m_length)) != TK_Normal)
				return status;

			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
#else
	return tk.Error (stream_readonly);
#endif
}

TK_Status TK_URL::Read (BStreamFileToolkit & tk) alter {
	TK_Status       status = TK_Normal;
#ifndef BSTREAM_DISABLE_ASCII
	if (tk.GetAsciiMode()) 
		return ReadAscii (tk);
#endif
	switch (m_stage) {
		case 0: {
			unsigned char       byte;

			if ((status = GetData (tk, byte)) != TK_Normal)
				return status;
			m_length = byte;

			m_stage++;
		}   nobreak;

		case 1: {
			if (m_length == 255) {                      
				if ((status = GetData (tk, m_length)) != TK_Normal)
					return status;
			}
			else if (m_length == 254) {                 
				unsigned short      word;

				if ((status = GetData (tk, word)) != TK_Normal)
					return status;
				m_length = word;
			}
        
			SetString (m_length);      
			m_stage++;
		}   nobreak;

		case 2: {
			if ((status = GetData (tk, m_string, m_length)) != TK_Normal)
				return status;
			m_stage = -1;
		}   break;

		default:
			return tk.Error();
	}

	return status;
}

void TK_URL::Reset () alter {
	m_length = 0;

	BBaseOpcodeHandler::Reset();
}

void TK_URL::SetString (int length) alter {
	m_length = length;
	if (m_length >= m_allocated) {
		delete [] m_string;
		m_string = new char [m_allocated = m_length + 16];
	}
	m_string[m_length] = '\0';
}

void TK_URL::SetString (char const * string) alter {
	SetString ((int) strlen (string));
	strcpy (m_string, string);
}



IMPLEMENT_CLONE (TK_Open_Segment);
IMPLEMENT_CLONE (TK_Close_Segment);
IMPLEMENT_CLONE (TK_Reopen_Segment);
IMPLEMENT_CLONE (TK_LOD);
IMPLEMENT_CLONE_OPCODE (TK_Renumber);
IMPLEMENT_CLONE_OPCODE (TK_Referenced_Segment);
IMPLEMENT_CLONE (TK_Reference);
IMPLEMENT_CLONE (TK_Instance);
IMPLEMENT_CLONE (TK_Delete_Object);
IMPLEMENT_CLONE_OPCODE (TK_Bounding);
IMPLEMENT_CLONE (TK_Streaming);
IMPLEMENT_CLONE_OPCODE (TK_Point);
IMPLEMENT_CLONE_OPCODE (TK_Line);
IMPLEMENT_CLONE_OPCODE (TK_Polypoint);
IMPLEMENT_CLONE (TK_NURBS_Curve);
IMPLEMENT_CLONE (TK_NURBS_Surface);
IMPLEMENT_CLONE (TK_Area_Light);
IMPLEMENT_CLONE (TK_Spot_Light);
IMPLEMENT_CLONE (TK_Cutting_Plane);
IMPLEMENT_CLONE_OPCODE (TK_Circle);
IMPLEMENT_CLONE_OPCODE (TK_Ellipse);
IMPLEMENT_CLONE (TK_Sphere);
IMPLEMENT_CLONE (TK_Cylinder);
IMPLEMENT_CLONE (TK_PolyCylinder);
IMPLEMENT_CLONE (TK_Grid);
IMPLEMENT_CLONE_OPCODE (TK_Text);
//IMPLEMENT_CLONE (TK_Image);
//IMPLEMENT_CLONE (TK_Thumbnail);
IMPLEMENT_CLONE (TK_Color);
IMPLEMENT_CLONE (TK_Color_RGB);
IMPLEMENT_CLONE (TK_Color_By_Value);
IMPLEMENT_CLONE_OPCODE (TK_Color_By_Index);
IMPLEMENT_CLONE (TK_Color_By_FIndex);
IMPLEMENT_CLONE (TK_Color_Map);
IMPLEMENT_CLONE (TK_Callback);
IMPLEMENT_CLONE (TK_Rendering_Options);
IMPLEMENT_CLONE (TK_Geometry_Options);
IMPLEMENT_CLONE (TK_Heuristics);
IMPLEMENT_CLONE (TK_Visibility);
IMPLEMENT_CLONE (TK_Selectability);
IMPLEMENT_CLONE_OPCODE (TK_Matrix);
IMPLEMENT_CLONE_OPCODE (TK_Size);
IMPLEMENT_CLONE (TK_Conditions);
IMPLEMENT_CLONE (TK_User_Options);
IMPLEMENT_CLONE (TK_Unicode_Options);
IMPLEMENT_CLONE (TK_User_Index);
IMPLEMENT_CLONE (TK_User_Value);
IMPLEMENT_CLONE (TK_Camera);
IMPLEMENT_CLONE (TK_Window);
IMPLEMENT_CLONE (TK_Clip_Region);
IMPLEMENT_CLONE (TK_Clip_Rectangle);
IMPLEMENT_CLONE (TK_Text_Font);
IMPLEMENT_CLONE (TK_Font);
IMPLEMENT_CLONE (TK_Texture);
IMPLEMENT_CLONE (TK_Glyph_Definition);
IMPLEMENT_CLONE (TK_Line_Style);

IMPLEMENT_CLONE_OPCODE (TK_Enumerated);
IMPLEMENT_CLONE_OPCODE (TK_Linear_Pattern);
IMPLEMENT_CLONE_OPCODE (TK_Named);

IMPLEMENT_CLONE (TK_External_Reference);
IMPLEMENT_CLONE (TK_External_Reference_Unicode);
IMPLEMENT_CLONE (TK_User_Data);
IMPLEMENT_CLONE (TK_URL);
IMPLEMENT_CLONE (TK_XML);