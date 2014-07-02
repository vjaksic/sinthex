#include "osc1.h"

//static varijable moraju se jednom inicijalizirati izvan svih funkcija
float C_osc1::sawtooth[kWaveSize+1];
float C_osc1::pulse[kWaveSize+1];
float C_osc1::sine[kWaveSize+1];
float C_osc1::freqtab[kNumFrequencies+1];

//konstruktor
C_osc1::C_osc1():phase(0.f),note(0)
{
	VstInt32 i;

	//scaler se inicijalizira za svaki slucaj u konstruktoru
	scaler = (float)((double)kWaveSize / 44100.);

	// generiranje valnih tablica
	VstInt32 wh = kWaveSize / 4;	// 1:3 impuls
	for (i = 0; i < kWaveSize; i++)
	{
		sawtooth[i] = (float)(-1. + (2. * ((double)i / (double)kWaveSize)));
		pulse[i] = (i < wh) ? -1.f : 1.f;
		sine[i] = xSin((double)i);
	}
	sawtooth[kWaveSize] = sawtooth[0];
	pulse[kWaveSize]=pulse[0];
	sine[kWaveSize]=sine[0];

	// generiranje frekvencijske tablice
	double k = 1.059463094359;	// 12ti korijen od 2
	double a = 6.875;	// a
	a *= k;	// b
	a *= k;	// h
	a *= k;	// c, frekvencija midi note 0
	for (i = 0; i < kNumFrequencies; i++) // 128 midi nota
	{
		freqtab[i] = (float)a;
		a *= k;
	}
}

C_osc1::~C_osc1() {}

//funkcija za brzo izracunavanje sinusa
//u valnoj tablici sa 4096 uzoraka
//preuzeta sa 
//http://www.musicdsp.org/archive.php?classid=1#241

float C_osc1::xSin(double x)
{
    //x is scaled 0<=x<4096
    const double A=-0.015959964859;
    const double B=217.68468676;
    const double C=0.000028716332164;
    const double D=-0.0030591066066;
    const double E=-7.3316892871734489e-005;
    double y;

    bool negate=false;
    if (x>2048)
    {
        negate=true;
        x-=2048;
    }
    if (x>1024)
        x=2048-x;
    if (negate)
        y=-((A+x)/(B+C*x*x)+D*x-E);
    else
        y=(A+x)/(B+C*x*x)+D*x-E;
    return (float)y;
}

//funkcija postavlja frekvenciju uzorkovanja za objekt
//te podesava scaler
void C_osc1::setSampleRate(float p_srate)
{
	sampleRate=p_srate;
	scaler=(float)((double)kWaveSize / (double)sampleRate);
}


//postavljanje parametara oscilatora na pocetku svakog bloka
void C_osc1::update(float p_detune, float p_waveform, float p_vol)
{
		volume= 0.5f * p_vol; //-6dB maksimalno zbog drugog oscilatora
		detune=(VstInt32)(p_detune*12);
		freq = freqtab[( note + detune ) & 0x7f] * scaler;	
		if (p_waveform < .3) wave1=sawtooth;
		if ((p_waveform >=.3)&&(p_waveform<.6)) wave1=pulse;
		if (p_waveform >= .6) wave1=sine;		

}

//generiranje signala i akumulacija na ulazni bafer
void C_osc1::processAcc(float *output, VstInt32 sampleFrames)
{
	while (--sampleFrames >= 0)
	{
		//sljedeca linija koda se koristi umjesto modulo 
		//operatora jer se % operator generalno smatra skupim
		if (phase > (float)kWaveSize )
			phase -= (float)kWaveSize;
		float phase_tmp=phase;
		phase += freq;

		//(*output++) += volume*wave1[(VstInt32)phase_tmp]; 
		//ovo je tocka u kojoj bi neinterpolirajuci oscilator 
		//zavrsio
		
		//linearna interpolacija		
		VstInt32 x1=(VstInt32)phase_tmp;
		float y1=wave1[x1];
		float y2=wave1[x1+1];		
		(*output++) += volume*( y1 + (phase_tmp - (float)x1)*(y2-y1) );		
	}
}

//tipka pritisnuta
void C_osc1::noteOn(VstInt32 p_note)
{
	phase=0.f;
	note=p_note;
	freq=freqtab[(p_note + detune) & 0x7f] * scaler;
}


