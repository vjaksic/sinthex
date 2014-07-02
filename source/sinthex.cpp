#include "sinthex.h"
#include "gmnames.h"


AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new Sinthex (audioMaster);
}


SinthexProgram::SinthexProgram ()
{
	// Defaultne vrijednosti
	fWaveform1 = 0.9f;	// pilasti
	fDetune     =0.f;
	fVolume1   = .5f;

	fVolume2	= .5f;	

	fAttack = .5f;
	fDecay = .5f;
	fSustain = 0.5f;
	fRelease = 0.5f;

	fCut=0.4f,
	fRes=0.1f,
	fType=0.f,
	
	fVolume    = .9f;
	vst_strncpy (name, "Basic", kVstMaxProgNameLen);
}


Sinthex::Sinthex (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, kNumPrograms, kNumParams)
{	
	// Inicijalizacija programa
	programs = new SinthexProgram[kNumPrograms];
	for (VstInt32 i = 0; i < 16; i++)
		channelPrograms[i] = i;

	if (programs)
		setProgram (0);
	
	if (audioMaster)
	{
		setNumInputs (0);				// Nema ulaza
		setNumOutputs (kNumOutputs);	// 2 izlaza
		canProcessReplacing ();
		isSynth ();						// Sintetizator
		setUniqueID ('STHX');			// Jednistveni ID
	}

	initProcess ();
	suspend ();
}

//-----------------------------------------------------------------------------------------
Sinthex::~Sinthex ()
{
	if (programs)
		delete[] programs;
}

//-----------------------------------------------------------------------------------------
void Sinthex::setProgram (VstInt32 program)
{
	if (program < 0 || program >= kNumPrograms)
		return;
	
	SinthexProgram *ap = &programs[program];
	curProgram = program;
	
	fWaveform1 = ap->fWaveform1;
	fVolume1   = ap->fVolume1;
	fDetune	   = ap->fDetune;

	fVolume2	= ap->fVolume2;	

	fAttack = ap->fAttack;
	fDecay = ap->fDecay;
	fSustain = ap->fSustain;
	fRelease = ap->fRelease;

	fCut = ap->fCut;
	fRes = ap->fRes;
	fType = ap->fType;

	fVolume    = ap->fVolume;
}

//-----------------------------------------------------------------------------------------
void Sinthex::setProgramName (char* name)
{
	vst_strncpy (programs[curProgram].name, name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void Sinthex::getProgramName (char* name)
{
	vst_strncpy (name, programs[curProgram].name, kVstMaxProgNameLen);
}

// Funkcija koja trazi oznaku parametra od plugina
void Sinthex::getParameterLabel (VstInt32 index, char* label)
{
	switch (index)
	{
		case kWaveform1:		
			vst_strncpy (label, "Shape", kVstMaxParamStrLen);
			break;
		case kDetune:		
			vst_strncpy (label, "Semi", kVstMaxParamStrLen);
			break;
		case kVolume1:
		case kVolume2:
		case kVolume:
			vst_strncpy (label, "dB", kVstMaxParamStrLen);
			break;

		case kAttack:			
		case kDecay:		
		case kRelease:
			vst_strncpy (label, "s", kVstMaxParamStrLen);
			break;
		case kSustain: 
			vst_strncpy (label, "level", kVstMaxParamStrLen); break;
		case kCut:
			vst_strncpy (label, "Hz", kVstMaxParamStrLen); break;
		case kRes:
			vst_strncpy (label, "", kVstMaxParamStrLen); break;
		case kType:
			vst_strncpy (label,"Type",kVstMaxParamStrLen); break;
	}
}

// Postavljanje vrijednosti na ekran
void Sinthex::getParameterDisplay (VstInt32 index, char* text)
{
	text[0] = 0;
	switch (index)
	{
		case kWaveform1:
			if (fWaveform1 < .3)
				vst_strncpy (text, "Sawtooth", kVstMaxParamStrLen);
			else if ((fWaveform1 >=.3)&&(fWaveform1<.6))
				vst_strncpy (text, "Pulse", kVstMaxParamStrLen);
			else vst_strncpy (text, "Sine", kVstMaxParamStrLen);
			break;

		case kDetune:		int2string((VstInt32)(12*fDetune),text,kVstMaxParamStrLen); break;
		case kVolume1:		dB2string (fVolume1, text, kVstMaxParamStrLen);	break;
		case kVolume2:		dB2string (fVolume2, text, kVstMaxParamStrLen); break;
		case kVolume:		dB2string (fVolume, text, kVstMaxParamStrLen);	break;
		case kAttack:		float2string (fAttack,text, kVstMaxParamStrLen); break;
		case kDecay:		float2string (fDecay, text, kVstMaxParamStrLen); break;
		case kSustain:		float2string (fSustain, text, kVstMaxParamStrLen); break;
		case kRelease:		float2string (fRelease, text, kVstMaxParamStrLen); break;
		case kCut:			float2string (11025*fCut,text,kVstMaxParamStrLen); break;
		case kRes:			float2string (fRes,text,kVstMaxParamStrLen);break;
		case kType:			
			if (fType < 0.25) vst_strncpy(text,"LP",kVstMaxParamStrLen);
			else if (fType < .5f ) vst_strncpy(text,"BP",kVstMaxParamStrLen);
			else if (fType < .75f) vst_strncpy(text,"HP",kVstMaxParamStrLen);
			else vst_strncpy(text,"NF",kVstMaxParamStrLen);
	}
}

// Dobivanje imena parametra od plugina
void Sinthex::getParameterName (VstInt32 index, char* label)
{
	switch (index)
	{
		case kWaveform1:	vst_strncpy (label, "Wave 1", kVstMaxParamStrLen);	break;
		case kDetune:		vst_strncpy (label, "Detune", kVstMaxParamStrLen);	break;
		case kVolume1:		vst_strncpy (label, "Levl 1", kVstMaxParamStrLen);	break;		
		
		case kVolume2:		vst_strncpy (label, "Levl 2", kVstMaxParamStrLen);	break;
		case kVolume:		vst_strncpy (label, "Volume", kVstMaxParamStrLen);	break;
		case kAttack:		vst_strncpy (label, "Attack", kVstMaxParamStrLen); break;
		case kDecay:		vst_strncpy (label, "Decay", kVstMaxParamStrLen); break;
		case kSustain:		vst_strncpy (label, "Sustain", kVstMaxParamStrLen); break;
		case kRelease:		vst_strncpy (label, "Release", kVstMaxParamStrLen);	break;
		case kCut:			vst_strncpy (label, "Cutoff", kVstMaxParamStrLen);	break;
		case kRes:			vst_strncpy (label, "Res", kVstMaxParamStrLen);	break;
		case kType:			vst_strncpy(label,"Filter",kVstMaxParamStrLen);break;
	}
}

// Funkcija za postavljanje parametara u plugin
void Sinthex::setParameter (VstInt32 index, float value)
{
	SinthexProgram *ap = &programs[curProgram];
	VstInt32 i=NUM_VOICES;
	switch (index)
	{
		case kWaveform1:
			fWaveform1	= ap->fWaveform1	= value;			
			break;
		case kDetune:
			fDetune		= ap->fDetune		= value;				
			break;
		case kVolume1:
			fVolume1	= ap->fVolume1		= value;			
			break;		
		case kVolume2:		fVolume2	= ap->fVolume2		= value;	break;
		case kVolume:		fVolume		= ap->fVolume		= value;	break;
		case kAttack:
			fAttack		= ap->fAttack		= value;			
			break;
		case kDecay:
			fDecay		= ap->fDecay		= value;			
			break;
		case kSustain:
			fSustain	= ap->fSustain		= value;			
			break;		
		case kRelease:
			fRelease	= ap->fRelease		= value;			
			break;
		case kCut:
			fCut = ap->fCut	= value;			
			break;
		case kRes:
			fRes = ap->fRes = value;
			break;
		case kType:
			fType = ap->fType = value;
			break;
	}
}

// Vracanje vrijednosti za odredeni parametar
float Sinthex::getParameter (VstInt32 index)
{
	float value = 0;
	switch (index)
	{
		case kWaveform1:	value = fWaveform1;	break;
		case kDetune:		value = fDetune;	break;
		case kVolume1:		value = fVolume1;	break;

		case kVolume2:		value = fVolume2;	break;

		case kVolume:		value = fVolume;	break;

		case kAttack:		value = fAttack;	break;
		case kDecay:		value = fDecay;     break;
		case kSustain:		value = fSustain;	break;
		case kRelease:		value = fRelease;	break;

		case kCut:			value = fCut;		break;
		case kRes:			value = fRes;		break;
		case kType:			value = fType;		break;
	}
	return value;
}


bool Sinthex::getOutputProperties (VstInt32 index, VstPinProperties* properties)
{
	if (index < kNumOutputs)
	{
		// Naziv u izlaznom mikseru
		vst_strncpy (properties->label, "Sthx ", 63);
		char temp[11] = {0};
		int2string (index + 1, temp, 10);
		vst_strncat (properties->label, temp, 63);

		properties->flags = kVstPinIsActive;
		if (index < 2)
			properties->flags |= kVstPinIsStereo;	// make channel 1+2 stereo
		return true;
	}
	return false;
}


bool Sinthex::getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text)
{
	if (index < kNumPrograms)
	{
		vst_strncpy (text, programs[index].name, kVstMaxProgNameLen);
		return true;
	}
	return false;
}

//vraca ime efekta
bool Sinthex::getEffectName (char* name)
{
	vst_strncpy (name, "Sinthex", kVstMaxEffectNameLen);
	return true;
}

//ime proizvodaca
bool Sinthex::getVendorString (char* text)
{
	vst_strncpy (text, "Vladimir Jaksic", kVstMaxVendorStrLen);
	return true;
}

//ime proizvoda
bool Sinthex::getProductString (char* text)
{
	vst_strncpy (text, "SINTHesizer EXample", kVstMaxProductStrLen);
	return true;
}

//verzija
VstInt32 Sinthex::getVendorVersion ()
{ 
	return 1000; 
}

//mogucnosti dodatka
VstInt32 Sinthex::canDo (char* text)
{
	if (!strcmp (text, "receiveVstEvents"))
		return 1;
	if (!strcmp (text, "receiveVstMidiEvent"))
		return 1;
	if (!strcmp (text, "midiProgramNames"))
		return 1;
	return -1;	// explicitly can't do; 0 => don't know
}


VstInt32 Sinthex::getNumMidiInputChannels ()
{
	return 1; //jedan midi ulaz
}


VstInt32 Sinthex::getNumMidiOutputChannels ()
{
	return 0; // nema midi izlaza
}


//standardne funkcije za baratanje sa MIDI programima
//iako u ovom slucaju nema potrebe za njihovom implementacijom
//jer je instrument monotimbralan, ovo je stavljeno samo
//za demonstraciju
VstInt32 Sinthex::getMidiProgramName (VstInt32 channel, MidiProgramName* mpn)
{
	VstInt32 prg = mpn->thisProgramIndex;
	if (prg < 0 || prg >= 128)
		return 0;
	fillProgram (channel, prg, mpn);
	if (channel == 9)
		return 1;
	return 128L;
}


VstInt32 Sinthex::getCurrentMidiProgram (VstInt32 channel, MidiProgramName* mpn)
{
	if (channel < 0 || channel >= 16 || !mpn)
		return -1;
	VstInt32 prg = channelPrograms[channel];
	mpn->thisProgramIndex = prg;
	fillProgram (channel, prg, mpn);
	return prg;
}


void Sinthex::fillProgram (VstInt32 channel, VstInt32 prg, MidiProgramName* mpn)
{
	mpn->midiBankMsb =
	mpn->midiBankLsb = -1;
	mpn->reserved = 0;
	mpn->flags = 0;

	if (channel == 9)	// drums
	{
		vst_strncpy (mpn->name, "Standard", 63);
		mpn->midiProgram = 0;
		mpn->parentCategoryIndex = 0;
	}
	else
	{
		vst_strncpy (mpn->name, GmNames[prg], 63);
		mpn->midiProgram = (char)prg;
		mpn->parentCategoryIndex = -1;	// for now

		for (VstInt32 i = 0; i < kNumGmCategories; i++)
		{
			if (prg >= GmCategoriesFirstIndices[i] && prg < GmCategoriesFirstIndices[i + 1])
			{
				mpn->parentCategoryIndex = i;
				break;
			}
		}
	}
}


VstInt32 Sinthex::getMidiProgramCategory (VstInt32 channel, MidiProgramCategory* cat)
{
	cat->parentCategoryIndex = -1;	// -1:no parent category
	cat->flags = 0;					// reserved, none defined yet, zero.
	VstInt32 category = cat->thisCategoryIndex;
	if (channel == 9)
	{
		vst_strncpy (cat->name, "Drums", 63);
		return 1;
	}
	if (category >= 0 && category < kNumGmCategories)
		vst_strncpy (cat->name, GmCategories[category], 63);
	else
		cat->name[0] = 0;
	return kNumGmCategories;
}


bool Sinthex::hasMidiProgramsChanged (VstInt32 channel)
{
	return false;	// updateDisplay ()
}


bool Sinthex::getMidiKeyName (VstInt32 channel, MidiKeyName* key)
								// struct will be filled with information for 'thisProgramIndex' and 'thisKeyNumber'
								// if keyName is "" the standard name of the key will be displayed.
								// if false is returned, no MidiKeyNames defined for 'thisProgramIndex'.
{
	// key->thisProgramIndex;		// >= 0. fill struct for this program index.
	// key->thisKeyNumber;			// 0 - 127. fill struct for this key number.
	key->keyName[0] = 0;
	key->reserved = 0;				// zero
	key->flags = 0;					// reserved, none defined yet, zero.
	return false;
}
