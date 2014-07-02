// Oscilator s pojasno ogranicenim valnim tablicama.
// Algoritam je preuzet iz primjera: 
// http://www.musicdsp.org/archive.php?classid=1#17
// Napravljena je prilagodba u c++ klasu koja se moze
// integrirati u VST standard.

#include <cmath>
#include "public.sdk/source/vst2.x/audioeffectx.h"


//konstante
#define WAVETABLE_SIZE		4096
#define WAVETABLE_SIZEF		WAVETABLE_SIZE.0f
#define WAVETABLE_MASK		(WAVETABLE_SIZE - 1)

const float TWOPI = 6.283185307179586f;
const float HALFPI = 1.570796326794896f;



//deklaracija klase za oscilator
class C_osc2
{
public:
	C_osc2();

	// funkcije koje se pozivaju u suspendiranom stanju i pri inicijalizaciji
	static void generate();  // generiranje valnih tablica za pilasti signal
	// frekvencija uzorkovanja je potrebna za odredivanje najveceg broja 
	// harmonika
	static void setSampleRate(float p_srate); 

	// funkcije koje se pozivaju unutar izvrsavanja processReplacing() funkcije
	void update(float p_vol); //postavljanje glasnoce
	void processGen(float* buffer,int length); //generiranje izlaznog bafera

	// funkcije koje se pozivaju unutar izvrsavanja processEvents() funkcije
	void noteOn(int midinote );  // tipka pritisnuta

private:	
	// valne tablice
	static float data[524288];
	static float sinetable[4096];	
	static int reverse[32768];

	static float SPEED; //frekvencija uzorkovanja
	static float TUNING; //stimanje A4 note, postavljeno na 440 u konstruktoru

	// parametri odredeni pritisnutom tipkom
	float index;
	float rate;
	int wavetableindex;
	float ratetofloatfactor;
	float* wavetable;

	// promjenjivi parametri oscilatora
	float volume;
};
