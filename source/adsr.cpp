// Implementacija generatora anvelope

#include "adsr.h"


C_adsr::C_adsr():stage(OFF),envelope(0.f){};

void C_adsr::setSampleRate(float p_srate)
{
	sampleRate=p_srate;	
}


	
void C_adsr::update(float p_att, float p_dec, float p_sus, float p_rel)
{
	this->attackCoeff=1 - powf(0.01f, 1.0f/( p_att * sampleRate ));
	this->decayCoeff=1- powf(0.01f, 1.0f/(p_dec*sampleRate));
	this->sustainLevel=p_sus;
	this->releaseCoeff=1 - powf(0.01f, 1.0f/(p_rel*sampleRate));
}

void C_adsr::noteOn()
{
	stage=ATTACK;
}


void C_adsr::noteOff()
{
	stage=RELEASE;	
} 