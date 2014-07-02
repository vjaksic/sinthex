// State variable filter

#ifndef __filtar__
#define __filtar__
#include "math.h"
#include "public.sdk/source/vst2.x/audioeffectx.h"

// Konstante
const float PI =  3.141592653589793f;
const float ANTIDENORMAL = 1e-18f;

// Deklaracije klase za filtar
class C_svf
{
public:
	C_svf();

	// Funkcije koje se pozivaju iz processReplacing() funkcije
	inline float MIN(float x, float y) { return (x<=y)?x : y; }
	void update(float fc,float res, float p_type);	
	void process(float *output, VstInt32 sampleFrames);	
	
private:
	// parametri filtra
	float freq,damp,prev,notch,band,high,low;

	// tip filtra
	int type;
};

#endif