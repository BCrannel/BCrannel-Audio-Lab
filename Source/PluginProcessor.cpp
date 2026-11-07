#include <juce_audio_utils/juce_audio_utils.h>
#include <cmath>
#include <juce_dsp/juce_dsp.h>
class BCrannelAudioProcessor final : public juce::AudioProcessor {
public:
 BCrannelAudioProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)){}
 const juce::String getName() const override { return JucePlugin_Name; }
 void prepareToPlay(double sr,int) override { juce::dsp::ProcessSpec s{sr,(juce::uint32) getBlockSize(),(juce::uint32)getTotalNumOutputChannels()}; for(auto& f:filters) f.prepare(s); }
 void releaseResources() override {}
 bool isBusesLayoutSupported(const BusesLayout& l) const override { return l.getMainInputChannelSet()==l.getMainOutputChannelSet() && !l.getMainOutputChannelSet().isDisabled(); }
 void processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&) override {
  juce::ScopedNoDenormals no; auto n=b.getNumSamples(); auto chans=b.getNumChannels();
  for(int c=getTotalNumInputChannels();c<chans;++c)b.clear(c,0,n);
  const auto name=juce::String(JucePlugin_Name);
  if(name.contains("Master")) { for(int c=0;c<chans;++c) b.applyGain(c,0,n,0.90f); }
  else if(name.contains("Space")) { for(int c=0;c<chans;++c){auto* x=b.getWritePointer(c);float z=0;for(int i=0;i<n;++i){z=0.18f*z+0.08f*x[i];x[i]+=z;}} }
  else if(name.contains("Effector")) { for(int c=0;c<chans;++c){auto* x=b.getWritePointer(c);for(int i=0;i<n;++i)x[i]=std::tanh(1.25f*x[i]);} }
  else if(name.contains("Harmony")) { if(chans>0 && chans>1){auto* l=b.getWritePointer(0);auto* r=b.getWritePointer(1);for(int i=1;i<n;++i)r[i]=0.88f*r[i]+0.12f*l[i-1];} }
  else if(name.contains("Tune")) { /* Safe pass-through until pitch tracking is implemented. */ }
  else { /* VoiceProcessor: conservative gentle level conditioning; no hard gate. */ for(int c=0;c<chans;++c){auto* x=b.getWritePointer(c);for(int i=0;i<n;++i){float a=std::abs(x[i]);if(a>0.72f)x[i]=std::copysign(0.72f+0.20f*(a-0.72f),x[i]);}} }
 }
 juce::AudioProcessorEditor* createEditor() override { return new juce::GenericAudioProcessorEditor(*this); }
 bool hasEditor() const override{return true;}
 double getTailLengthSeconds() const override{return 0.25;}
 int getNumPrograms() override{return 1;} int getCurrentProgram() override{return 0;} void setCurrentProgram(int) override{} const juce::String getProgramName(int) override{return {};} void changeProgramName(int,const juce::String&) override{}
 void getStateInformation(juce::MemoryBlock& d) override{juce::ignoreUnused(d);} void setStateInformation(const void*,int) override{}
 bool acceptsMidi() const override{return false;} bool producesMidi() const override{return false;} bool isMidiEffect() const override{return false;}
private: juce::dsp::IIR::Filter<float> filters[2];
};
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new BCrannelAudioProcessor();}
