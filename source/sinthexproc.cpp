#include "sinthex.h"


// Postavljanje frekvencije uzorkovanja
// poziva se samo u suspendiranom stanju.
void Sinthex::setSampleRate (float sampleRate)
{
	AudioEffectX::setSampleRate (sampleRate);
	// Postavljanje frekvencije za glasove
	for (int i=0;i<NUM_VOICES;i++) voices[i].setSampleRate(sampleRate);	
	// Postavljanje frekvencije za osc2 pobuduje generiranje
	// pojasno ogranicenih valnih tablica.
	C_osc2::setSampleRate(sampleRate);
}

// Promjena velicine bloka
void Sinthex::setBlockSize (VstInt32 blockSize)
{
	AudioEffectX::setBlockSize (blockSize);	
	for (int i=0;i<NUM_VOICES;i++) voices[i].setBlockSize(blockSize);
}

//-----------------------------------------------------------------------------------------
void Sinthex::initProcess ()
{

}

//-----------------------------------------------------------------------------------------
// Glavna processReplacing metoda
// **inputs bafer se ne koristi
void Sinthex::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
	// Izlazni bafer se postavlja na nulu
    memset( outputs[0],0,sampleFrames*sizeof(float));
	memset( outputs[1],0,sampleFrames*sizeof(float));

	float* o1=outputs[0];
	float* o2=outputs[1];

	VstInt32 i=NUM_VOICES;

	
	while (--i >= 0)
	{		
		if (voices[i].env.stage)
		{
			// Za svaki aktivni glas vrse se kalkulacije parametara unutar
			// update() metoda.

			
				voices[i].osc1.update(fDetune,fWaveform1,fVolume1);
				voices[i].osc2.update(fVolume2);
				voices[i].env.update(fAttack,fDecay,fSustain,fRelease);				
			

			// Glasovna process metoda akumulira izlazne podatke na izlazni bafer
			voices[i].processAcc(o1, sampleFrames);					
		}
	}	
	// Kalkulacije za filtar	
	filtar.update(fCut,fRes,fType);

	filtar.process(o1,sampleFrames);

	// Desni kanal se izjednacuje s lijevim
	while (--sampleFrames >= 0) (*o2++)= ((*o1++)*= (this->fVolume));
}


// Dogadaji se salju pomocu *ev parametra
VstInt32 Sinthex::processEvents (VstEvents* ev)
{
	for (VstInt32 i = 0; i < ev->numEvents; i++)
	{
		// Prepoznavanje da li je dogadaj tipa MIDI
		if ((ev->events[i])->type != kVstMidiType)
			continue;

		VstMidiEvent* event = (VstMidiEvent*)ev->events[i];
		char* midiData = event->midiData;

		// Ignorira se kanal
		VstInt32 status = midiData[0] & 0xf0;	

		// Gledaju se samo note
		if (status == 0x90 || status == 0x80)	
		{
			// Zapis note
			VstInt32 note = midiData[1] & 0x7f;  
			// Zapis glasnoce
			VstInt32 velocity = midiData[2] & 0x7f; 
			// Nota ugasena pomocu glasnoce 0
			if (status == 0x80)
				velocity = 0;	
			if (!velocity)
				// Poziv glavne noteOff metode
				noteOff (note); 
			else
				// Poziv glavne noteOn metode
				noteOn (note, velocity, event->deltaFrames); 
		}
		else if (status == 0xb0)
		{
			// Sve note iskljucene
			if (midiData[1] == 0x7e || midiData[1] == 0x7b)	
			{
				// Ako su sve note iskljucene
				// svi aktivni glasovi prelaze u release fazu.
				int i=NUM_VOICES;
				while (--i >= 0)
					if (voices[i].env.stage) 
						voices[i].noteOff();		
			}
		}
		event++;
	}
	return 1;
}

// Glavna noteOn metoda
void Sinthex::noteOn (VstInt32 note, VstInt32 velocity, VstInt32 delta)
{
	int i=NUM_VOICES;
	while (--i >= 0)
		// Trazi aktivni glas
		if (!voices[i].env.stage)
		{			
			// Pokreni njegovu noteOn metodu
			voices[i].noteOn(note, velocity, delta);
			break;
		}}

// Glavna noteOff metoda
void Sinthex::noteOff (VstInt32 note)
{
	int i=NUM_VOICES;
	// Trazi otpusteni glas
	while (--i >= 0)
		if (voices[i].note == note)
		{
			// Pokreni njegovu noteOff metodu
			voices[i].noteOff();		
		}
}
