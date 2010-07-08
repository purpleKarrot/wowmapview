#include "util.h"
#ifdef _WINDOWS
#include <windows.h>
#include <wx/msw/winundef.h>
#endif
#include <wx/choicdlg.h>
#include <wx/dir.h>
#include <wx/dirdlg.h>

int gameVersion = 0;

bool useLocalFiles = false;
bool useRandomLooks = true;
bool bHideHelmet = false;
bool bKnightEyeGlow = true;
bool bShowParticle = true;
bool bZeroParticle = true;

long langID = 0;
long langOffset = 0;
int ssCounter = 100; // ScreenShot Counter
int imgFormat = 0;

wxString CSConv(wxString str)
{
	if (langID <= 0)
		return str;

	return wxConvLocal.cWC2WX(wxConvUTF8.cMB2WC(str.mb_str()));
}

float frand()
{
    return rand()/(float)RAND_MAX;
}

float randfloat(float lower, float upper)
{
	return lower + (upper-lower)*(rand()/(float)RAND_MAX);
}

int randint(int lower, int upper)
{
    return lower + (int)((upper+1-lower)*frand());
}

void fixname(std::string &name)
{
	for (uint32 i=0; i<name.length(); i++) {
		if (i>0 && name[i]>='A' && name[i]<='Z' && isalpha(name[i-1])) {
			name[i] |= 0x20;
		} else if ((i==0 || !isalpha(name[i-1])) && name[i]>='a' && name[i]<='z') {
			name[i] &= ~0x20;
		}
	}
}
void fixnamen(char *name, uint32 len)
{
	for (uint32 i=0; i<len; i++) {
		if (i>0 && name[i]>='A' && name[i]<='Z' && isalpha(name[i-1])) {
			name[i] |= 0x20;
		} else if ((i==0 || !isalpha(name[i-1])) && name[i]>='a' && name[i]<='z') {
			name[i] &= ~0x20;
		}
	}
}

int wxStringToInt(const wxString& str)
{
	long number = 0;
	str.ToLong(&number);
	return number;
}

// Byteswap for 2 Bytes
unsigned short _SwapTwoBytes (unsigned short w)
{
	unsigned short tmp;
	tmp =  (w & 0x00ff);
	tmp = ((w & 0xff00) >> 0x08) | (tmp << 0x08);
	return tmp;
}

// Round a float, down to the specified decimal
float round(float input, int limit = 2){
	if (limit > 0){
		input *= (10^limit);
	}
	input = int(input+0.5);
	if (limit > 0){
		input /= (10^limit);
	}
	return input;
}
