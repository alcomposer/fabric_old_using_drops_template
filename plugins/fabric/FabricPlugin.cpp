/* 
    Drops - Drops Really Only Plays Samples
    Copyright (C) 2021  Rob van den Berg

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "DistrhoPlugin.hpp"
#include "FabricPlugin.hpp"

#include <mutex>
#include <random>

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------

FabricPlugin::FabricPlugin():
    Plugin(kParameterCount, 0, 2)
    ,waveForm(2000)
    ,grainPlayer()
    ,bufferPos(0)
{

    sampleRate = getSampleRate();
    sig_sampleLoaded = false;
    loadedSample = false;
    bpm = 120.;
    fSampleIn = 0.0f;
    fSampleOut = 1.0f;
    fSampleLoopStart = 0.0f;
    fSampleLoopEnd = 1.0f;
    fSamplePitchKeyCenter = 60.0f;
    fSamplePlayMode = 0.0f;
    fSamplePlayDirection = 0.0f;
    fSamplePitch = 100.0f;
    fSampleOversampling = 1.0f;
    // amp
    //fGrainDensity = 0.0f;
    fPlayheadSpeed = 0.0f;  //was fAmpEgDecay
    fGrainDensity = 0.5f;
    fGrainLength = 0.0f;
    fAmpLFOType = 0.0f;
    fAmpLFOSync = 0.0f;
    fAmpLFOSyncFreq = 0.0f;
    fSpray = 0.0f; //was fAmpLFOFade
    fAmpLFOFreq = 0.0f;
    fAmpLFOSync = 0.0f;

    // filter
    fFilterCutOff = 1.0f;
    fFilterResonance = 0.0f;
    fFilterEGDepth = 1.0f;
    fFilterEGAttack = 0.0f;
    fFilterEgDecay = 0.0f;
    fFilterEgSustain = 0.0f;
    fFilterEgRelease = 0.0f;
    fFilterType = 0.0f;
    fFilterLFOType = 0.0f;
    fFilterLFOSync = 0.0f;
    fFilterLFOSyncFreq = 0.0f;
    fFilterLFOFreq = 0.0f;
    fFilterLFODepth = 0.0f;
    fFilterLFOSync = 0.0f;
    fFilterLFOFade = 0.0f;

    // pitch
    fPitchEGDepth = 1.0f;
    fPitchEGAttack = 0.0f;
    fPitchEgDecay = 0.0f;
    fPitchEgSustain = 0.0f;
    fPitchEgRelease = 0.0f;
    fPitchLFOType = 0.0f;
    fPitchLFOSync = 0.0f;
    fPitchLFOFreq = 0.0f;
    fPitchLFOSyncFreq = 0.0f;
    fPitchLFODepth = 0.0f;
    fPitchLFOFade = 0.0f;

    fFilterMaxFreq = sampleRate * .5;



    st_audioBuffer.resize(10*sampleRate);

    bufferPos = 0;

    recTrue = false;


}

// --  PARAMETERS  -------------------------------------------------------------

// Init
void FabricPlugin::initParameter(uint32_t index, Parameter &parameter)
{
    // parameters that don't have a sfz "onccN" opcode cause the sfz file to be
    // reload, thus can't be automated
    // these parameters have kParameterIsAutomable disabled
    switch (index)
    {
    case kSampleIn:
        parameter.name = "Sample In";
        parameter.symbol = "sample_in";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kSampleOut:
        parameter.name = "Sample Out";
        parameter.symbol = "sample_out";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 1.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kSampleLoopStart:
        parameter.name = "Loop Start";
        parameter.symbol = "sample_loop_start";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kSampleLoopEnd:
        parameter.name = "Loop End";
        parameter.symbol = "sample_loop_end";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 1.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kSamplePitchKeyCenter:
        parameter.name = "Key Center";
        parameter.symbol = "pitch_center";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 127.0f;
        parameter.ranges.def = 60.0f;
        parameter.hints = kParameterIsInteger;
        break;
    case kSamplePitch:
        parameter.name = "Pitch";
        parameter.symbol = "pitch";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 200.0f;
        parameter.ranges.def = 100.0f;
        parameter.hints = kParameterIsAutomable | kParameterIsInteger;
        break;
    case kSamplePlayMode:
        parameter.name = "Playmode";
        parameter.symbol = "playmode";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 3.0f;
        parameter.ranges.def = 0.0f;
        parameter.enumValues.count = 4;
        parameter.enumValues.restrictedMode = true;
        parameter.enumValues.values = new ParameterEnumerationValue[4]{
            ParameterEnumerationValue(0.0f, "no_loop"),
            ParameterEnumerationValue(1.0f, "one_shot"),
            ParameterEnumerationValue(2.0f, "loop_continuous"),
            ParameterEnumerationValue(3.0f, "loop_sustain")};

        parameter.hints = kParameterIsInteger;
        break;
    case kSamplePlayDirection:
        parameter.name = "Play Direction";
        parameter.symbol = "play_direction";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.enumValues.count = 2;
        parameter.enumValues.restrictedMode = true;
        parameter.enumValues.values = new ParameterEnumerationValue[2]{
            ParameterEnumerationValue(0.0f, "forward"),
            ParameterEnumerationValue(1.0f, "reverse"),
        };
        parameter.hints = kParameterIsInteger;
        break;
    case kSampleOversampling:
        parameter.name = "Oversampling";
        parameter.symbol = "oversampling";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 3.0f;
        parameter.ranges.def = 0.0f;
        parameter.enumValues.count = 4;
        parameter.enumValues.restrictedMode = true;
        parameter.enumValues.values = new ParameterEnumerationValue[4]{
            ParameterEnumerationValue(0.0f, "1x"),
            ParameterEnumerationValue(1.0f, "2x"),
            ParameterEnumerationValue(2.0f, "3x"),
            ParameterEnumerationValue(3.0f, "4x"),
        };
        parameter.hints = kParameterIsInteger;
        break;
    //case kAmpEgAttack:
    //    parameter.name = "Amp Attack";
    //    parameter.symbol = "amp_attack";
    //    parameter.ranges.min = 0.0f;
    //    parameter.ranges.max = 1.0f;
    //    parameter.ranges.def = 0.0f;
    //    parameter.hints = kParameterIsAutomable;
    //    break;
    case kPlayheadSpeed:
        parameter.name = "Playhead Speed";
        parameter.symbol = "playhead_speed";
        parameter.ranges.min = -1.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kGrainDensity: //was kampegsustain
        parameter.name = "Grain Density";
        parameter.symbol = "grain_density";
        parameter.ranges.min = 0.f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.5f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kGrainLength:
        parameter.name = "Grain Length";
        parameter.symbol = "grain_length";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kAmpLFOType:
        parameter.name = "Amp LFO Type";
        parameter.symbol = "amp_lfo_type";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 5.0f;
        parameter.ranges.def = 0.0f;
        parameter.enumValues.count = 6;
        parameter.enumValues.restrictedMode = true;
        parameter.enumValues.values = new ParameterEnumerationValue[6]{
            ParameterEnumerationValue(0.0f, "triangle"),
            ParameterEnumerationValue(1.0f, "sine"),
            ParameterEnumerationValue(2.0f, "square"),
            ParameterEnumerationValue(3.0f, "saw up"),
            ParameterEnumerationValue(4.0f, "saw down"),
            ParameterEnumerationValue(5.0f, "s/h")};
        //parameter.hints = kParameterIsAutomable;
        break;
    case kAmpLFOSync:
        parameter.name = "Amp LFO Sync";
        parameter.symbol = "amp_lfo_sync";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsBoolean;
        break;
    case kAmpLFOFreq:
        parameter.name = "Amp LFO Freq";
        parameter.symbol = "amp_lfo_freq";
        parameter.ranges.min = 1.0f;
        parameter.ranges.max = 1000.0f;
        parameter.ranges.def = 500.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kAmpLFOSyncFreq:
        parameter.name = "Amp LFO Sync Freq";
        parameter.symbol = "amp_lfo_sync_freq";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 17.0f;
        parameter.ranges.def = 2.0f;
        parameter.enumValues.count = 18;
        parameter.enumValues.restrictedMode = true;
        parameter.enumValues.values = new ParameterEnumerationValue[18]{
            ParameterEnumerationValue(0.0f, "1/16"),
            ParameterEnumerationValue(1.0f, "1/8"),
            ParameterEnumerationValue(2.0f, "1/4"),
            ParameterEnumerationValue(3.0f, "1/2"),
            ParameterEnumerationValue(4.0f, "1/1"),
            ParameterEnumerationValue(5.0f, "2/1"),
            ParameterEnumerationValue(6.0f, "1/16T"),
            ParameterEnumerationValue(7.0f, "1/8T"),
            ParameterEnumerationValue(8.0f, "1/4T"),
            ParameterEnumerationValue(9.0f, "1/2T"),
            ParameterEnumerationValue(10.0f, "1/1T"),
            ParameterEnumerationValue(11.0f, "2/1T"),
            ParameterEnumerationValue(12.0f, "1/16."),
            ParameterEnumerationValue(13.0f, "1/8."),
            ParameterEnumerationValue(14.0f, "1/4."),
            ParameterEnumerationValue(15.0f, "1/2."),
            ParameterEnumerationValue(16.0f, "1/1."),
            ParameterEnumerationValue(17.0f, "2/1."),
        };
        break;
    case kSpray:
        parameter.name = "SPRAY";
        parameter.symbol = "spray";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;

    // filter
    case kFilterType:
        parameter.name = "Filter Type";
        parameter.symbol = "filter_type";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 2.0f;
        parameter.ranges.def = 0.0f;
        parameter.enumValues.count = 3;
        parameter.enumValues.restrictedMode = true;
        parameter.enumValues.values = new ParameterEnumerationValue[3]{
            ParameterEnumerationValue(0.0f, "lpf_2p"),
            ParameterEnumerationValue(1.0f, "bpf_2p"),
            ParameterEnumerationValue(2.0f, "hpf_2p")};
        parameter.hints = kParameterIsInteger;
        break;
    case kFilterCutOff:
        parameter.name = "Cutoff";
        parameter.symbol = "cutoff";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kFilterResonance:
        parameter.name = "Resonance";
        parameter.symbol = "resonance";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kFilterEgDepth:
        parameter.name = "Filter EG Depth";
        parameter.symbol = "filter_eg_depth";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kFilterEgAttack:
        parameter.name = "Filter Attack";
        parameter.symbol = "filter_attack";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kFilterEgDecay:
        parameter.name = "Filter Decay";
        parameter.symbol = "filter_decay";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kFilterEgSustain:
        parameter.name = "Filter Sustain";
        parameter.symbol = "filter_sustain";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kFilterEgRelease:
        parameter.name = "Filter Release";
        parameter.symbol = "filter_release";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kFilterLFOType:
        parameter.name = "Filter LFO Type";
        parameter.symbol = "filter_lfo_type";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 5.0f;
        parameter.ranges.def = 0.0f;
        parameter.enumValues.count = 6;
        parameter.enumValues.restrictedMode = true;
        parameter.enumValues.values = new ParameterEnumerationValue[6]{
            ParameterEnumerationValue(0.0f, "triangle"),
            ParameterEnumerationValue(1.0f, "sine"),
            ParameterEnumerationValue(2.0f, "square"),
            ParameterEnumerationValue(3.0f, "saw up"),
            ParameterEnumerationValue(4.0f, "saw down"),
            ParameterEnumerationValue(5.0f, "s/h")};
        parameter.hints = kParameterIsInteger;
        break;
    case kFilterLFOSync:
        parameter.name = "Filter LFO Sync";
        parameter.symbol = "filter_lfo_sync";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsBoolean;
        break;
    case kFilterLFOFreq:
        parameter.name = "Filter LFO Freq";
        parameter.symbol = "filter_lfo_freq";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kFilterLFOSyncFreq:
        parameter.name = "Filter LFO Sync Freq";
        parameter.symbol = "filter_lfo_sync_freq";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 17.0f;
        parameter.ranges.def = 2.0f;
        parameter.enumValues.count = 18;
        parameter.enumValues.restrictedMode = true;
        parameter.enumValues.values = new ParameterEnumerationValue[18]{
            ParameterEnumerationValue(0.0f, "1/16"),
            ParameterEnumerationValue(1.0f, "1/8"),
            ParameterEnumerationValue(2.0f, "1/4"),
            ParameterEnumerationValue(3.0f, "1/2"),
            ParameterEnumerationValue(4.0f, "1/1"),
            ParameterEnumerationValue(5.0f, "2/1"),
            ParameterEnumerationValue(6.0f, "1/16T"),
            ParameterEnumerationValue(7.0f, "1/8T"),
            ParameterEnumerationValue(8.0f, "1/4T"),
            ParameterEnumerationValue(9.0f, "1/2T"),
            ParameterEnumerationValue(10.0f, "1/1T"),
            ParameterEnumerationValue(11.0f, "2/1T"),
            ParameterEnumerationValue(12.0f, "1/16."),
            ParameterEnumerationValue(13.0f, "1/8."),
            ParameterEnumerationValue(14.0f, "1/4."),
            ParameterEnumerationValue(15.0f, "1/2."),
            ParameterEnumerationValue(16.0f, "1/1."),
            ParameterEnumerationValue(17.0f, "2/1."),
        };
        parameter.hints = kParameterIsInteger;
        break;
    case kFilterLFODepth:
        parameter.name = "Filter LFO Depth";
        parameter.symbol = "filter_lfo_depth";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kFilterLFOFade:
        parameter.name = "Filter LFO Fade";
        parameter.symbol = "filter_lfo_fade";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;
    // pitch
    case kPitchEgDepth:
        parameter.name = "Pitch EG Depth";
        parameter.symbol = "pitch_eg_depth";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 1.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kPitchEgAttack:
        parameter.name = "Pitch Attack";
        parameter.symbol = "pitch_attack";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kPitchEgDecay:
        parameter.name = "Pitch Decay";
        parameter.symbol = "pitch_decay";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kPitchEgSustain:
        parameter.name = "Pitch Sustain";
        parameter.symbol = "pitch_sustain";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kPitchEgRelease:
        parameter.name = "Pitch Release";
        parameter.symbol = "pitch_release";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kPitchLFOType:
        parameter.name = "Pitch LFO Type";
        parameter.symbol = "pitch_lfo_type";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 5.0f;
        parameter.ranges.def = 0.0f;
        parameter.enumValues.count = 6;
        parameter.enumValues.restrictedMode = true;
        parameter.enumValues.values = new ParameterEnumerationValue[6]{
            ParameterEnumerationValue(0.0f, "triangle"),
            ParameterEnumerationValue(1.0f, "sine"),
            ParameterEnumerationValue(2.0f, "square"),
            ParameterEnumerationValue(3.0f, "saw up"),
            ParameterEnumerationValue(4.0f, "saw down"),
            ParameterEnumerationValue(5.0f, "s/h")};
        parameter.hints = kParameterIsInteger;
        break;
    case kPitchLFOFreq:
        parameter.name = "Pitch LFO Freq";
        parameter.symbol = "pitch_lfo_freq";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kPitchLFOSyncFreq:
        parameter.name = "Pitch LFO Sync Freq";
        parameter.symbol = "pitch_lfo_sync_freq";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 17.0f;
        parameter.ranges.def = 2.0f;
        parameter.enumValues.count = 18;
        parameter.enumValues.restrictedMode = true;
        parameter.enumValues.values = new ParameterEnumerationValue[18]{
            ParameterEnumerationValue(0.0f, "1/16"),
            ParameterEnumerationValue(1.0f, "1/8"),
            ParameterEnumerationValue(2.0f, "1/4"),
            ParameterEnumerationValue(3.0f, "1/2"),
            ParameterEnumerationValue(4.0f, "1/1"),
            ParameterEnumerationValue(5.0f, "2/1"),
            ParameterEnumerationValue(6.0f, "1/16T"),
            ParameterEnumerationValue(7.0f, "1/8T"),
            ParameterEnumerationValue(8.0f, "1/4T"),
            ParameterEnumerationValue(9.0f, "1/2T"),
            ParameterEnumerationValue(10.0f, "1/1T"),
            ParameterEnumerationValue(11.0f, "2/1T"),
            ParameterEnumerationValue(12.0f, "1/16."),
            ParameterEnumerationValue(13.0f, "1/8."),
            ParameterEnumerationValue(14.0f, "1/4."),
            ParameterEnumerationValue(15.0f, "1/2."),
            ParameterEnumerationValue(16.0f, "1/1."),
            ParameterEnumerationValue(17.0f, "2/1."),
        };
        parameter.hints = kParameterIsInteger;
        break;
    case kPitchLFOFade:
        parameter.name = "Pitch LFO Fade";
        parameter.symbol = "pitch_lfo_fade";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.5f;
        parameter.hints = kParameterIsAutomable;
        break;
    case kPitchLFODepth:
        parameter.name = "Pitch LFO Depth";
        parameter.symbol = "pitch_lfo_depth";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsAutomable;
        break;

    case kSampleLoaded:
        parameter.name = "Sample Loaded";
        parameter.symbol = "sampleloaded";
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.ranges.def = 0.0f;
        parameter.hints = kParameterIsBoolean | kParameterIsOutput;
    default:
        break;
    }
}

float FabricPlugin::getParameterValue(uint32_t index) const
{
    float val = 0.0f;
    switch (index)
    {
    case kSampleIn:
        val = fSampleIn;
        break;
    case kSampleOut:
        val = fSampleOut;
        break;
    case kSampleLoopStart:
        val = fSampleLoopStart;
        break;
    case kSampleLoopEnd:
        val = fSampleLoopEnd;
        break;
    case kSamplePitchKeyCenter:
        val = fSamplePitchKeyCenter;
        break;
    case kSamplePitch:
        val = fSamplePitch;
        break;
    case kSamplePlayMode:
        val = fSamplePlayMode;
        break;
    case kSamplePlayDirection:
        val = fSamplePlayDirection;
        break;
    case kSampleOversampling:
        val = fSampleOversampling;
        break;
    case kPlayheadSpeed:
        val = fPlayheadSpeed;
        break;
    case kGrainDensity:
        val = fGrainDensity;
        break;
    case kGrainLength:
        val = fGrainLength;
        break;
    case kAmpLFOType:
        val = fAmpLFOType;
        break;
    case kAmpLFOSync:
        val = fAmpLFOSync;
        break;
    case kAmpLFOFreq:
        val = fAmpLFOFreq;
        break;
    case kAmpLFOSyncFreq:
        val = fAmpLFOSyncFreq;
        break;
    case kSpray:
        val = fSpray;
        break;
        // filter
    case kFilterType:
        val = fFilterType;
        break;
    case kFilterCutOff:
        val = fFilterCutOff;
        break;
    case kFilterResonance:
        val = fFilterResonance;
        break;
    case kFilterEgDepth:
        val = fFilterEGDepth;
        break;
    case kFilterEgAttack:
        val = fFilterEGAttack;
        break;
    case kFilterEgDecay:
        val = fFilterEgDecay;
        break;
    case kFilterEgSustain:
        val = fFilterEgSustain;
        break;
    case kFilterEgRelease:
        val = fFilterEgRelease;
        break;
    case kFilterLFOType:
        val = fFilterLFOType;
        break;
    case kFilterLFOSync:
        val = fFilterLFOSync;
        break;
    case kFilterLFOFreq:
        val = fFilterLFOFreq;
        break;
    case kFilterLFOSyncFreq:
        val = fFilterLFOSyncFreq;
        break;
    case kFilterLFODepth:
        val = fFilterLFODepth;
        break;
    case kFilterLFOFade:
        val = fFilterLFOFade;
        break;
    // pitch
    case kPitchEgDepth:
        val = fPitchEGDepth;
        break;
    case kPitchEgAttack:
        val = fPitchEGAttack;
        break;
    case kPitchEgDecay:
        val = fPitchEgDecay;
        break;
    case kPitchEgSustain:
        val = fPitchEgSustain;
        break;
    case kPitchEgRelease:
        val = fPitchEgRelease;
        break;
    case kPitchLFOType:
        val = fPitchLFOType;
        break;
    case kPitchLFOSync:
        val = fPitchLFOSync;
        break;
    case kPitchLFOFreq:
        val = fPitchLFOFreq;
        break;
    case kPitchLFOSyncFreq:
        val = fPitchLFOSyncFreq;
        break;
    case kPitchLFODepth:
        val = fPitchLFODepth;
        break;
    case kPitchLFOFade:
        val = fPitchLFOFade;
        break;

    case kSampleLoaded:
        val = sig_sampleLoaded;
        break;
    default:
#ifdef DEBUG
        printf("Unknown parameter: %i\n", index);
#endif
        break;
    }
    return val;
}

void FabricPlugin::setParameterValue(uint32_t index, float value)
{
    switch (index)
    {
    case kSampleIn:
        fSampleIn = value;
        break;
    case kSampleOut:
        fSampleOut = value;
        break;
    case kSampleLoopStart:
        fSampleLoopStart = value;
        break;
    case kSampleLoopEnd:
        fSampleLoopEnd = value;
        break;


    //}
    // amp
    //case kAmpEgAttack:
    //    fAmpEGAttack = value;
    //    break;
    case kPlayheadSpeed:
        fPlayheadSpeed = value;
        break;
    case kGrainDensity:
        fGrainDensity = value;
        break;
    case kGrainLength:
        fGrainLength = value;
        break;
        // lfo
    case kAmpLFOFreq:
        fAmpLFOFreq = value;
        break;
    case kSpray: // was kAmpLFOFade
        fSpray = value;
        break;
        // filter
    case kFilterCutOff:
        fFilterCutOff = value;
        break;
    case kFilterResonance:
        fFilterResonance = value;
        break;
    case kFilterEgDepth:
        fFilterEGDepth = value;
        break;
    case kFilterEgAttack:
        fFilterEGAttack = value;
        break;
    case kFilterEgDecay:
        fFilterEgDecay = value;
        break;
    case kFilterEgSustain:
        fFilterEgSustain = value;
        break;
    case kFilterEgRelease:
        fFilterEgRelease = value;
        break;
    case kFilterLFOFreq:
        fFilterLFOFreq = value;
        break;
    case kFilterLFODepth:
        fFilterLFODepth = value;
        break;
    case kFilterLFOFade:
        fFilterLFOFade = value;
        break;
    // pitch
    case kPitchEgDepth:
        fPitchEGDepth = value;
        break;
    case kPitchEgAttack:
        fPitchEGAttack = value;
        break;
    case kPitchEgDecay:
        fPitchEgDecay = value;
        break;
    case kPitchEgSustain:
        fPitchEgSustain = value;
        break;
    case kPitchEgRelease:
        fPitchEgRelease = value;
        break;
    case kPitchLFOFreq:
        fPitchLFOFreq = value;
        break;
    case kPitchLFODepth:
        fPitchLFODepth = value;
        break;
    case kPitchLFOFade:
        fPitchLFOFade = value;
        break;

    case kSampleLoaded:
        break;
    default:
        printf("setParameterValue : unknown parameter %i\n", index);
        break;
    }
}

void FabricPlugin::setState(const char *key, const char *value)
{
    if (strcmp(key, "ui_sample_loaded") == 0)
    {
        sig_sampleLoaded = false;
    }
    if (strcmp(key, "filepath") == 0)
    {
        path = std::string(value);
        //loadSample(value);
    }
}

String FabricPlugin::getState(const char *key) const
{
    String retString = String("undefined state");
#ifdef DEBUG
    printf("getState(%s)\n", key);
#endif
    if (strcmp(key, "filepath") == 0)
    {
        retString = path.c_str();
    }
    if (strcmp(key, "ui_sample_loaded") == 0)
    {
        retString = "ui_sample_loaded yes/no";
    }
    return retString;
};

void FabricPlugin::initState(unsigned int index, String &stateKey, String &defaultStateValue)
{
    switch (index)
    {
    case 0:
        stateKey = "filepath";
        defaultStateValue = "empty";
        break;
    case 1:
        stateKey = "ui_sample_loaded";
        defaultStateValue = "false";
        break;
    default:
#ifdef DEBUG
        printf("initState %i\n", index);
#endif
        break;
    }
}

int FabricPlugin::loadSample(const char *fp)
{
    // init waveform and miniMap
    waveForm.resize(0);
    miniMap.resize(0);

    SndfileHandle fileHandle(fp);

    // get the number of frames in the sample
    // sample range in sfizz is 0 ... frames - 1
    sampleLength = fileHandle.frames() - 1;
    if (sampleLength == -1)
    {
//TODO: show this in UI
//file doesn't exist or is of incompatible type, main handles the -1
#ifdef DEBUG
        printf("Can't load sample %s \n", fp);
#endif
        sig_sampleLoaded = false;
        return -1;
    }
    // get some more info of the sample
    int sampleChannels = fileHandle.channels();
    // get max value
    double max_val;
    fileHandle.command(SFC_CALC_NORM_SIGNAL_MAX, &max_val, sizeof(max_val));
    // normalize factor
    float ratio = max_val > 1.0f ? 1.0f : 1.0f / max_val;

    // resize vector
    std::vector<float> sample;
    sample.resize(sampleLength * sampleChannels);
    // load sample memory in samplevector
    fileHandle.read(&sample.at(0), sampleLength * sampleChannels);
    // sum to mono if needed
    sf_count_t size = sampleLength;
    if (sampleChannels == 2)
    { // sum to mono

        for (int i = 0, j = 0; i < size; i++)
        {
            float sum_mono = (sample[j] + sample[j + 1]) * 0.5f;
            waveForm.push_back((sum_mono * ratio) * float(display_height / 2));
            j += 2;
        }
    }
    else
    {
        waveForm.resize(size);
        for (int i = 0; i < size; i++)
        {
            waveForm[i] = (sample[i] * ratio) * float(display_height / 2);
        }
    }

    // make minimap
    miniMap.resize(display_width);
    float samples_per_pixel = static_cast<float>(sampleLength) / (float)display_width;
    float fIndex;
    uint iIndex;
    for (uint16_t i = 0; i < display_width; i++)
    {
        fIndex = float(i) * samples_per_pixel;
        iIndex = fIndex;
        auto minmax = std::minmax_element(waveForm.begin() + iIndex, waveForm.begin() + iIndex + int(samples_per_pixel));
        signed char min = std::abs(*minmax.first);
        signed char max = std::abs(*minmax.second);
        signed char maxValue = std::max(min, max);
        miniMap[i] = (float)maxValue / (float)(display_height / 2) * (float)minimap_height;
    }
    sig_sampleLoaded = true;
    loadedSample = true;
    return 0;
}


// --  MAIN PLUGIN FUNCTIONS  --------------------------------------------------

/* Main Audio and MIDI processing function */
void FabricPlugin::run(
    const float **inputs,        // incoming audio
    float **outputs,             // outgoing audio
    uint32_t frames,             // size of block to process
    const MidiEvent *midiEvents, // MIDI pointer
    uint32_t midiEventCount      // Number of MIDI events in block
)
{
    // get timing info

    std::unique_lock<std::mutex> lock(synthMutex, std::try_to_lock);
    if (!lock.owns_lock())
    { // synth is locked?
        std::fill_n(outputs[0], frames, 0.0f);
        std::fill_n(outputs[1], frames, 0.0f);
        return;
    }
    const TimePosition &timePos(getTimePosition());
    if (timePos.bbt.valid)
    {
        //synth.tempo(0, 60.0f / timePos.bbt.beatsPerMinute);
        //synth.timeSignature(0, timePos.bbt.beatsPerBar, timePos.bbt.beatType);
        const double beat = timePos.bbt.beat - 1;
        const double fracBeat = timePos.bbt.tick / timePos.bbt.ticksPerBeat;
        const double barBeat = beat + fracBeat;
        //synth.timePosition(0, timePos.bbt.bar, barBeat);
        //printf("barBeat %f\n", barBeat);
        //synth.playbackState(0, static_cast<int>(timePos.playing));
    }

    uint32_t framesDone = 0;
    uint32_t curEventIndex = 0; // index for midi event to process

    while (framesDone < frames)
    {
        while (curEventIndex < midiEventCount && framesDone == midiEvents[curEventIndex].frame)
        {

            int status = midiEvents[curEventIndex].data[0]; // midi status
                                                            //  int channel = status & 0x0F ; // get midi channel
            int midi_message = status & 0xF0;
            int midi_data1 = midiEvents[curEventIndex].data[1];
            int midi_data2 = midiEvents[curEventIndex].data[2];
            switch (midi_message)
            {
            case 0x80: // note_off
                //synth.noteOff(framesDone, midi_data1, midi_data2);
                break;
            case 0x90: // note_on
                //synth.noteOn(framesDone, midi_data1, midi_data2);
                break;
            default:
                break;
            }
            curEventIndex++;

        } // midi events
        ++framesDone;
    } // frames loop
   // synth.renderBlock(outputs, frames, 2);

        //write audio input into buffer
        if (recTrue){
            for (int pos = 0; pos < frames; pos++)
            {
                st_audioBuffer[bufferPos].first = inputs[0][pos];
                st_audioBuffer[bufferPos].second = inputs[1][pos];
                bufferPos++;
                if (bufferPos > st_audioBuffer.size()) bufferPos = 0;
            }
        }
        int densityHz = (int)(sampleRate / (fGrainDensity*1000));

        for (int pos = 0; pos < frames; pos++){
            playheadPos += fPlayheadSpeed*4.0-2.0;
            if (playheadPos > st_audioBuffer.size()) playheadPos = 0;
            if (playheadPos < 0) playheadPos = st_audioBuffer.size();

            if (grainStart == 0){
                // 0-1 float random number. Buffer is 10 seconds.
                float normalizedRand = float(std::rand() % 1000000) / 1000000;
                // first normalize the headpos, then add a random number of 1 second duration and center it on playhead
                float spray = fSpray;
                float startPos = playheadPos / st_audioBuffer.size() + normalizedRand * spray - spray * 0.5;
                if (startPos < 0.0) startPos += 1.0;
                if (startPos > 1.0) startPos -= 1.0;


                grainPlayer.add(startPos, fGrainLength*sampleRate*10, GRAIN_DIR::forward, &st_audioBuffer, 128);
            }
            if (grainStart > densityHz){
                grainStart = 0;
            }else{
                grainStart++;
            }

            std::pair<float,float> output = grainPlayer.process();
            outputs[0][pos] = output.first;
            outputs[1][pos] = output.second;
        };

        int increment = st_audioBuffer.size()/display_width;

        waveForm.clear();
        //convert audio buffer into low quality buffer
        for (int pos = 0; pos < (sampleRate * 10) ; pos+=increment)
        {
            waveForm.push_back(((st_audioBuffer[pos].first + st_audioBuffer[pos].second) * 0.5) * float(display_height * 0.5));
        }
        
        if(grainArrayDisplayMutex.try_lock()){
            memcpy(&grainArrayDisplay, grainPlayer.grain_array, sizeof(grainArrayDisplay));
            grainArrayDisplayMutex.unlock();
        };
} // run

/* Plugin entry point, called by DPF to create a new plugin instance. */
Plugin *createPlugin()
{
    return new FabricPlugin();
}

// -----------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
