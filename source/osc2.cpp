#include "osc2.h"

float C_osc2::SPEED=44100.f;
float C_osc2::TUNING=440.f;
float C_osc2::data[524288];
int C_osc2::reverse[32768];
float C_osc2::sinetable[4096];

C_osc2::C_osc2():volume(0.f)
{				
		int i;
		// Generiranje sinusne valne tablice
		// za potrebe generiranja harmonika
		for(i=0;i<4096;i++)			
		sinetable[i]=sin(TWOPI*(float)i/4096.0f);		
		
		// ustimavanje oscilatora
		TUNING = 440.f;
}

void C_osc2::generate()
{
		int a,b,n;
		float* datap;
		int partials;
	    int partial;
	    int partialindex,reverseindex,lastnumpartials;
	    float max,m;		

		reverseindex=0;
		partialindex=0;
		lastnumpartials=-1;

		for(n=0;n<128;n++)
		{
			// broj clanova u fourierovom redu
			partials=(int)((SPEED*0.5f)/float(TUNING*(float)pow(2,(float) (n-69)/12.0f)));
			if(partials!=lastnumpartials)
			{
				datap=&data[partialindex*4096];
				for(b=0;b<4096;b++)
					datap[b]=0.0f;  //prazna valna tablica
				for(a=0;a<partials;a++)
				{
					partial=a+1;
					//neutralizacija Gibbsovog efekta
					m=cos((float)a*HALFPI/(float)partials);
					m*=m;
					m/=(float)partial;
					//punjenje valne tablice
					for(b=0;b<4096;b++)
						datap[b]+=m*sinetable[(b*partial)%4096];
				}
				lastnumpartials=partials;
				a=int(2.0f*TUNING*(float)pow(2,(float) (n-69)/12.0f)); //2f
				for(b=reverseindex;b<=a;b++)
					reverse[b]=partialindex;
				reverseindex=a+1;
				partialindex++;
			}
		}

		for(b=reverseindex;b<32768;b++)
			reverse[b]=partialindex-1;		

		max=0.0;
		for(b=0;b<4096;b++)
		{
			if(fabs(*(data+b))>max)
				max=(float)fabs(*(data+b));
		}

		for(b=0;b<4096*partialindex;b++)
		{
			*(data+b)/=max;
		}		
}	

void C_osc2::setSampleRate(float p_srate){
	SPEED = p_srate;
	// Valne tablice se generiraju nakon sto oscilator
	// dobije frekvenciju uzorkovanja.
	generate(); 
}

void C_osc2::update(float p_vol)
{
	volume = 0.5f * p_vol;
}

void C_osc2::processGen(float* buffer,int length)
{
	int currentsample;
	int nextsample;
	float m;		
	while(--length >= 0)
	{
		currentsample=(int) index;
		nextsample=(currentsample+1) & WAVETABLE_MASK;
		m=index-(float) currentsample; 
		// linearna interpolacija
		(*buffer++)=volume * ((1.0f-m)*wavetable[currentsample]+m*wavetable[nextsample]);
		index += rate;
		if(index>WAVETABLE_SIZEF)
		{			
			wavetableindex=reverse[(int)(ratetofloatfactor*rate)];
			wavetable=&data[wavetableindex*WAVETABLE_SIZE];
			index-=WAVETABLE_SIZEF;
		}
	}
}

void C_osc2::noteOn(int midinote)
{
	float f=TUNING*(float)pow(2,(float) (midinote-69)/12.0f);
	rate=f*WAVETABLE_SIZEF/SPEED;
	ratetofloatfactor=2.0f*SPEED/WAVETABLE_SIZEF;
	index=0.0f;
	wavetableindex=reverse[(int)(2.0f*f)];
	wavetable=&data[wavetableindex*WAVETABLE_SIZE];
}