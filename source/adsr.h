// Generator anvelope

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include <cmath>

// Enumerator za fazu 
enum
{
	OFF=0,
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE
};


// Deklaracija klase za GA (EG)
class C_adsr
{
public:
	C_adsr();

	// Funkcije koje se pozivaju u suspendiranom stanju i pri inicijalizaciji
	void setSampleRate(float p_srate);

	// Funkcije koje se pozivaju unutar izvrsavanja processReplacing() funkcije
	// Racunanje koeficijenata
	void update(float p_att, float p_dec, float p_sus, float p_rel);
	// Funckija mnozi buffer sa vrijednoscu anvelope
	void processMul(float *output, VstInt32 sampleFrames);

	// Funkcije koje se pozivaju unutar izvrsavanja processEvents() funkcije
	void noteOn();
	void noteOff();

	// Stage varijabla oznacava trenutnu fazu GA.
	// Ona mora biti public clanica funkcije jer
	// se pomocu nje odreduje da li je glas aktivan.
	int stage;

private:
	// Frekvencija uzorkovanja
	float sampleRate;

	// Vrijednost anvelope [0,1]
	float envelope;

    // Koeficijenti za odziv filtra
    float attackCoeff;
    float decayCoeff;
    float sustainLevel;
    float releaseCoeff;	
};


// Inline funkcija mora biti definirana u headeru
inline void C_adsr::processMul(float *output, VstInt32 sampleFrames)
{
	while (--sampleFrames >= 0){
	// U ovisnosti o fazi, izracunava se odziv filtra prema
	// koeficijentima ili se prelazi na sljedecu fazu.
		switch ( stage ) {
		    case ATTACK:
			    envelope += attackCoeff * (1.0f - envelope);
                if ( envelope > 0.99f ) {
				    stage=DECAY;
                }
                break;                
            case DECAY:
                envelope -= decayCoeff * envelope;
                if ( envelope <= sustainLevel + 0.001f ) {
                    stage = SUSTAIN;
                }
                break;
            case SUSTAIN:
     			if ( sustainLevel == 0.0f )
				{
					envelope = 0.f;
					stage = OFF;
				}
                break;
				// Release faza se poziva iz noteOff metode,
				// nakon koje generator prelazi
				// u OFF fazu sto javlja glavnom programu
				// da je glass postao neaktivan.
			case RELEASE:
				envelope -= releaseCoeff * envelope;
				// Prelazak u stanje iskljuceno vrsi se kod
				// pada signala na -60 dB
                if ( envelope < 0.001f ) {
					envelope = 0.f;
                    stage = OFF;
                }
                break;
			case OFF:
                break;
		}
    (*output++) *= envelope; // Ulaz se mnozi sa vrijednoscu anvelope
	}
}
