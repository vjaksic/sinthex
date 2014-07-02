// Implementacija filtra
#include "svf.h"

C_svf::C_svf()
{
type=0;
prev=band=low=0;
this->update(440.f,0.5f,0.f);
}

void C_svf::update(float fc, float res, float p_type)
{
	
	// Velika prednost ovog filtra je sto nema potrebe za frekvencijom uzorkovanja
	// prilikom kalkulacije koeficijenata.
	freq   = 2.0f*sin(PI*MIN(0.24f, fc/8.f));
	damp   = res*2;
	type   = (int)(3.999f*p_type);
}

// Osnovni algoritam filtra preuzet sa 
// http://www.musicdsp.org/archive.php?classid=3#92

void C_svf::process(float *output, VstInt32 sampleFrames)
{
	for(int i=0;i<sampleFrames;i++)
	{
		//naduzorkovanje sa faktorom 2 koristeci
		//linearnu interpolaciju
		float i1=(prev + *output) /2;
		prev = *output;
		
		//osnovni algoritam filtra
		//konstanta ANTIDENORMAL se dodaje 
		//radi izbjegavanja operacija u denormalnom
		//modu
		notch = i1 - damp * band + ANTIDENORMAL;
		low   = low + freq * band;                                                              
        high  = notch - low;                                                                    
        band  = freq * high + band;   
		
		//algoritam se ponovno primjenjuje na originalnom
		//uzorku radi pomicanja granice stabilnosti
        notch = *output - damp * band;
        low   = low + freq * band;                                                              
        high  = notch - low;                                                                    
        band  = freq * high + band;

		//u ovisnosti o odabranom tipu filtra postavlja se
		//izlaz
		switch(type)
		{
		case 0:		(*output++) = low/2;		break;
		case 1:		(*output++) = band/2;		break;
		case 2:		(*output++) = high/2;		break;
		case 3:		(*output++) = notch/2;		break;
		}
	}
}
