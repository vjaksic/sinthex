//oscilator sa potrazivanjem u valnoj tablici

#include "public.sdk/source/vst2.x/audioeffectx.h"

//konstante
#define Pi  3.141592653589793

//enumerator
enum
{
	kNumFrequencies = 128,	// 128 frekvencija, svaka za jednu MIDI notu
	kWaveSize = 4096		// broj uzoraka u valnoj tablici
};

//deklaracija klase za oscilator
class C_osc1
{
public:
	C_osc1();
	~C_osc1();
	
	//funkcije koje se pozivaju u suspendiranom stanju i pri inicijalizaciji
	float xSin(double x);	//brza sinus funkcija
	void setSampleRate(float p_srate);	
	
	//funkcije koje se pozivaju unutar izvrsavanja processReplacing() funkcije
	void update(float p_detune, float p_waveform, float p_vol); //poziva se jednom po baferu
	void processAcc(float *output, VstInt32 sampleFrames); //akumulira izlazne podatke

	//funkcije koje se pozivaju unutar izvrsavanja processEvents() funkcije
	void noteOn(VstInt32 p_note); //poziva se pri pritisku note

private:
	//valne tablice unutar kojih se spremaju periode signala
	//+1 služi za olaksavanje linearne interpolacije
	static float sawtooth[kWaveSize+1]; //pilasti
	static float pulse[kWaveSize+1]; //pravokutni
	static float sine[kWaveSize+1]; //sinusoida
	static float freqtab[kNumFrequencies+1]; //popis frekvencija za odredenu MIDI notu

	//podaci odredeni frekvencijom uzorkovanja
	float scaler;
	float sampleRate;

	//parametri odredeni pritisnutom tipkom
	VstInt32 note;
	float phase;
	float freq;		
	
	//promjenjivi parametri oscilatora
	float* wave1;	
	float volume;
	VstInt32 detune;		
};

