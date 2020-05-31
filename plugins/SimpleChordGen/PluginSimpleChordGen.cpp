/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2018 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*
THERE IS A BUG, if you change the chordType while still playing one some notes will not receive noteOFF
*/
#include "DistrhoPlugin.hpp"
START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

/**
  Plugin that demonstrates the latency API in DPF.
 */
class SimpleChordGen : public Plugin
{
public:
    SimpleChordGen()
        : Plugin(2, 0, 0) {} //one parameter, one program
        //std::memset(vel, 0.0, sizeof(float));
         
protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

   /**
      Get the plugin label.
      This label is a short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    */
    const char* getLabel() const override
    {
        return "SimpleChordGen";
    }

   /**
      Get an extensive comment/description about the plugin.
    */
    const char* getDescription() const override
    {
        return "A midi chord generator controlled by a ChordType parameter, Major, Minor, Dim and Aug triads";
    }

   /**
      Get the plugin author/maker.
    */
    const char* getMaker() const override
    {
        return "Haradai";
    }

   /**
      Get the plugin homepage.
    */
    const char* getHomePage() const override
    {
        return "";
    }

   /**
      Get the plugin license name (a single line of text).
      For commercial plugins this should return some short copyright information.
    */
    const char* getLicense() const override
    {
        return "ISC";
    }

   /**
      Get the plugin version, in hexadecimal.
    */
    uint32_t getVersion() const override
    {
        return d_version(1, 0, 0);
    }

   /**
      Get the plugin unique Id.
      This value is used by LADSPA, DSSI and VST plugin formats.
    */
    int64_t getUniqueId() const override
    {
        return d_cconst('d', 'M', 'T', 'r');
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init and Internal data, unused in this plugin */

    void  initParameter(uint32_t index, Parameter& parameter) override {
        switch(index){
            case 0:
                parameter.hints = kParameterIsAutomable;
                parameter.ranges.min = 0.0;
                parameter.ranges.max = 1.0;
                parameter.ranges.def = 0.5;
                parameter.name = "Velocity";
                parameter.symbol = "vel";
                break;
            case 1:
                parameter.hints = kParameterIsAutomable;
                parameter.ranges.min = 0.0;
                parameter.ranges.max = 1.0;
                parameter.ranges.def = 0.0;
                parameter.name = "ChordType";
                parameter.symbol = "cho_typ";
                break;
        }
    }
    float getParameterValue(uint32_t index) const   override { 
        switch(index){
            case 0:
                return vel;
                break;
            case 1:
                return chordType;
                break;
        }
        return 0;
    }
    void  setParameterValue(uint32_t index, float value)  override {
        switch(index){
            case 0:
                vel = value;
                break;
            case 1:
                if(value < 0.125){
                    chordType = 0.0;
                }
                else if(value < 0.375){
                    chordType = 0.25;
                }
                else if(value < 0.625){
                    chordType = 0.5;
                }
                else if(value < 0.875){
                    chordType = 0.75;
                }
                else{
                    chordType = 1;
                }
                break;
        }
    }

   /* --------------------------------------------------------------------------------------------------------
    * Audio/MIDI Processing */ 

   /**
      Run/process function for plugins with MIDI input.
      In this case we just pass-through all MIDI events.
    */
    void run(const float**, float**, uint32_t,
             const MidiEvent* midiEvents, uint32_t midiEventCount) override
    {
        switch((int)(100*chordType)){
            case 0:
                noChord(chord,midiEvents,midiEventCount);
                break;
            case 25:
                majorTriad(chord,midiEvents,midiEventCount); 
                break;
            case 50:
                minorTriad(chord,midiEvents,midiEventCount); 
                break;
            case 75:
                augTriad(chord,midiEvents,midiEventCount); 
                break;
            case 100:
                dimTriad(chord,midiEvents,midiEventCount); 
                break;
        }

    }
     // --------------------------------------CHORD GENERATION-----------------------------------------------
    void setupMidiData(MidiEvent *chord, uint8_t notes,const MidiEvent* midiEvents ,uint32_t a){
        for(uint8_t i=0; i<notes; ++i){
            chord[i].frame = 0; 
            chord[i].size = 3;
            chord[i].data[0] = midiEvents[a].data[0]; //get note on or off
            chord[i].data[2] =  (int)127*vel; //velocity
            writeMidiEvent(chord[i]);
        }
    }

    void majorTriad(MidiEvent *chord,const MidiEvent* midiEvents,uint32_t midiEventCount){
        for (uint32_t a=0; a<midiEventCount; ++a){
            chord[0].data[1] = midiEvents[a].data[1]; //get note 
            chord[1].data[1] = midiEvents[a].data[1] + 4; //sum to major third
            chord[2].data[1] = midiEvents[a].data[1] + 4 + 3 ; //sum to minor third over the third, so a fifth
            setupMidiData(chord,3,midiEvents,a);
        }
    }

    void minorTriad(MidiEvent *chord,const MidiEvent* midiEvents,uint32_t midiEventCount){
        for (uint32_t a=0; a<midiEventCount; ++a){
            chord[0].data[1] = midiEvents[a].data[1]; //get note 
            chord[1].data[1] = midiEvents[a].data[1] + 3; //sum to minor third
            chord[2].data[1] = midiEvents[a].data[1] + 3 + 4 ; //fifht
            setupMidiData(chord,3,midiEvents,a);
        }
    }

    void augTriad(MidiEvent *chord,const MidiEvent* midiEvents,uint32_t midiEventCount){
        for (uint32_t a=0; a<midiEventCount; ++a){
            chord[0].data[1] = midiEvents[a].data[1]; //get note 
            chord[1].data[1] = midiEvents[a].data[1] + 4; //sum to major third
            chord[2].data[1] = midiEvents[a].data[1] + 4 + 4 ; //aug fifht
            setupMidiData(chord,3,midiEvents,a);
        }
    }

    void dimTriad(MidiEvent *chord,const MidiEvent* midiEvents,uint32_t midiEventCount){
        for (uint32_t a=0; a<midiEventCount; ++a){
            chord[0].data[1] = midiEvents[a].data[1]; //get note 
            chord[1].data[1] = midiEvents[a].data[1] + 3; //sum to minor third
            chord[2].data[1] = midiEvents[a].data[1] + 3 + 3 ; //dim fifht
            setupMidiData(chord,3,midiEvents,a);
        }
    }

    void noChord(MidiEvent *chord,const MidiEvent* midiEvents,uint32_t midiEventCount){
        for (uint32_t a=0; a<midiEventCount; ++a){
            chord[0].data[1] = midiEvents[a].data[1]; //get note 
            setupMidiData(chord,1,midiEvents,a);
        }
    }

    

    // -------------------------------------------------------------------------------------------------------

private:
    //
    MidiEvent chorda[7]; // I don't think any chord should use more than 7 notes
    MidiEvent *chord = chorda;
    float vel = 0.0;
    float chordType = 0.0;
   /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleChordGen)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    return new SimpleChordGen();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO