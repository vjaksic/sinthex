Sinthex VSTi

1. OSNOVNE INFORMACIJE
2. INSTALACIJA I UPORABA
3. IZVORNI KOD


1. OSNOVNE INFORMACIJE

Sinthex je sintetizator digitalnog glazbenog signala sa sljedećim mogućnostima:

* oscilator sa mogućnosću odabira 3 različita valna oblika: pilasti, pravokutni, sinus
* pojasno ograničeni oscilator sa pilastim valnim oblikom
* eksponencijalni generator anvelope sa podesivim ADSR parametrima
* filtar sa mogućnošću podešavanja granične frekvencije, rezonancije te odabira tipa
  (niski propust, visoki propust, pojasni propust, notch)
* 16 glasovnom polifonijom


2. INSTALACIJA I UPORABA

Za instalaciju programa potrebno je iskopirati datoteku win\Release\sinthex.dll u direktorij
predviđen za VST dodatke namješten u domaćinskoj aplikaciji (hostu). Najčešće je to
"C:\Program Files\Steinberg\Vstplugins". Nakon toga potrebno je učitati plugin koristeći
sučelje instalirane domaćinske aplikacije.
Sintetizator je testiran na Cubase SX3, FL Studio 7 i Reaper 2.51 domaćinima unutar kojih 
su se problemi pojavili jedino kod ucitavanja u FL Studio prilikom automatizacije parametara. Unutar ostalih aplikacija nije primjeceno neobicno ponasanje.
Nakon učitavanja plugina u host, pojavit će se sučelje sa 12 podesivih parametara:
Wave 1 - odabir valnog oblika za prvi oscilator (sinus, pravokutni, pilasti) 
Detune - odmak frekvencije prvog oscilatora u polutonovima
Levl1 - razina prvog oscilatora
Levl2 - razina drugog oscilatora
Volume - glavna razina signala
Attack - vrijeme porasta signala
Decay - vrijeme pada signala
Sustain - razina signala prilikom drzanja tipke pritisnute
Release - vrijeme pada signala nakon otpustanja tipke
Cutoff - granicna frekvencija filtra
Res - rezonancija (Q parametar)
Type - tip filtra

3. IZVORNI KOD
Sintetizator je građen koristeći Microsoftov Visual Studio IDE te je napisan u C++
programskom jeziku. Datoteke izvornog koda nalaze se u direktoriju source\, a Visual
Studio Solution se nalazi u win\sinthex.sln datoteci.


5.10.2008
Vladimir Jakšić