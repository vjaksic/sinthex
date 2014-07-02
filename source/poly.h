// Modul za realizaciju polifonije

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "adsr.h"
#include "osc1.h"
#include "osc2.h"

// Konstante
const double midiScaler = (1. / 127.);


// Deklaracije klase voice
class voice
{
public:
	voice();
	~voice();

	// Funkcije koje se pozivaju u suspendiranom stanju i pri inicijalizaciji
	void setSampleRate(float sampleRate); // Promjena frekvencije uzorkovanja
	void setBlockSize(VstInt32 blockSize); // Promjena velicine bloka

	// Funkcije koje se pozivaju unutar processReplacing funkcije
	void processAcc(float *output, VstInt32 sampleFrames);		

	// Funkcije koje se pozivaju unutar izvrsavanja processEvents() funkcije
	void noteOn(VstInt32 p_note, VstInt32 p_velocity, VstInt32 p_delta);
	void noteOff();

	// Svaki glas sastoji se od dva oscilatora i generatora anvelope
	C_adsr env;
	C_osc1 osc1;
	C_osc2 osc2;

	// Parametri note
	VstInt32 note;
	VstInt32 velocity;
	VstInt32 delta;			

	// Pomocni izlazni bafer
	float* out;	
};
