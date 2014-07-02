//-------------------------------------------------------------------------------------------------------
// Programska potpora Sinthex izradena je 2008 na Fakultetu elektrotehnike i racunarstva u 
// Zagrebu u okviru diplomskog rada Vladimira Jaksica pod nazivom "Sinteza virtualnih glazbenih
// instrumenata upotrebom racunala" i vodstvom doc.dr.sc. Kristiana Jambrosica.
//-------------------------------------------------------------------------------------------------------

#ifndef __sinthex__
#define __sinthex__


#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "poly.h"
#include "svf.h"

//maksimalni broj glasova
#define NUM_VOICES 16



//enumerator parametara
enum
{	
	kNumPrograms = 128,
	kNumOutputs = 2,

	//oscilator 1
	kWaveform1 = 0,
	kDetune,
	kVolume1,

	//oscilator 2
	kVolume2,

	//glavna glasnoca
	kVolume,

	//anvelopa
	kAttack,
	kDecay,
	kSustain,
	kRelease,

	//filtar
	kCut,
	kRes,
	kType,
	
	kNumParams
};


class SinthexProgram
{
friend class Sinthex;
public:
	SinthexProgram ();
	~SinthexProgram () {}

private:
	//osc1
	float fWaveform1;
	float fDetune;
	float fVolume1;

	//osc2
	float fVolume2;

	//anvelopa
	float fAttack;
	float fDecay;
	float fSustain;
	float fRelease;
	
	//filtar
	float fCut;
	float fRes;
	float fType;
	
	float fVolume;
	char name[kVstMaxProgNameLen+1];	
};


//glavna klasa
class Sinthex : public AudioEffectX
{
public:
	Sinthex (audioMasterCallback audioMaster);
	~Sinthex ();

	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual VstInt32 processEvents (VstEvents* events);

	virtual void setProgram (VstInt32 program);
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);
	virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);

	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);
	
	virtual void setSampleRate (float sampleRate);
	virtual void setBlockSize (VstInt32 blockSize);
	
	virtual bool getOutputProperties (VstInt32 index, VstPinProperties* properties);
		
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
	virtual VstInt32 canDo (char* text);

	virtual VstInt32 getNumMidiInputChannels ();
	virtual VstInt32 getNumMidiOutputChannels ();

	virtual VstInt32 getMidiProgramName (VstInt32 channel, MidiProgramName* midiProgramName);
	virtual VstInt32 getCurrentMidiProgram (VstInt32 channel, MidiProgramName* currentProgram);
	virtual VstInt32 getMidiProgramCategory (VstInt32 channel, MidiProgramCategory* category);
	virtual bool hasMidiProgramsChanged (VstInt32 channel);
	virtual bool getMidiKeyName (VstInt32 channel, MidiKeyName* keyName);

private:
	//glavna glasnoca
	float fVolume;	

	//osc1
	float fWaveform1;		
	float fVolume1;	
	float fDetune;

	//osc2
	float fVolume2;

	//anvelopa
	float fAttack;
	float fDecay;
	float fSustain;
	float fRelease;

	//filtar
	float fCut;
	float fRes;
	float fType;

	SinthexProgram* programs;
	VstInt32 channelPrograms[16];

	void initProcess ();
	void noteOn (VstInt32 note, VstInt32 velocity, VstInt32 delta);
	void noteOff (VstInt32 note);
	void fillProgram (VstInt32 channel, VstInt32 prg, MidiProgramName* mpn);

	C_svf filtar;  //jedna instanca filtra 
	voice voices[NUM_VOICES]; //niz glasova

	
};


#endif
