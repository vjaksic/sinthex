// Implementacija polifonije
#include "poly.h"

voice::voice():delta(0),note(0),out(NULL)
{
	this->setSampleRate(44100.f);
}

voice::~voice()
{
	// Dealokacija memorije.
	if (out) delete [] out;
}

void voice::setSampleRate(float sampleRate)
{
	// Polifonija se brine o frekvenciji uzorkovanja 
	// oscilatora 1 i generatora anvelope.
	osc1.setSampleRate(sampleRate);
	env.setSampleRate(sampleRate);
}

void voice::setBlockSize(VstInt32 blockSize)
{
	// Realociranje memorije za pomocni bafer
	// se vrsi u suspendiranom stanju.
	if (out) delete [] out;
	out = new float[blockSize];	
}

void voice::processAcc(float *output, VstInt32 sampleFrames)
{
		float vol = (float)((double)velocity * midiScaler);		
		float *point=out;
	
		if (delta > 0)
			{
				if (delta >= sampleFrames)	// buducnost
				{
					delta -= sampleFrames;
					return;
				}
				memset (out, 0, delta * sizeof (float));
				sampleFrames -= delta;
				while (--delta >=0) (*output++) += (*point++);
				delta = 0;
			}		
		osc2.processGen(point,sampleFrames); // Generiranje signala
		osc1.processAcc(point,sampleFrames); // Nadodavanje
		env.processMul(point,sampleFrames);  // Mnozenje s anvelopom

		while (--sampleFrames >= 0)
			(*output++) += (*point++) * vol;  // Nadodavanje na izlaz
}

void voice::noteOn(VstInt32 p_note, VstInt32 p_velocity, VstInt32 p_delta)
{
	env.noteOn();
	osc1.noteOn(p_note);
	osc2.noteOn(p_note);
	note=p_note;
	velocity=p_velocity;
	delta=p_delta;	
}

void voice::noteOff()
{
	env.noteOff();	
}
