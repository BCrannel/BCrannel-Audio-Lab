#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

class BCrannelAudioProcessor final : public juce::AudioProcessor {
public:
    BCrannelAudioProcessor()
        : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                           .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          apvts(*this, nullptr, "PARAMETERS", makeParameters()) {}

    const juce::String getName() const override { return JucePlugin_Name; }
    void prepareToPlay(double sampleRate, int samplesPerBlock) override {
        currentSampleRate = sampleRate;
        juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock,
                                     (juce::uint32) getTotalNumOutputChannels() };
        for (auto& f : highPass) f.prepare(spec);
        for (auto& f : lowPass) f.prepare(spec);
        for (auto& f : deEss) f.prepare(spec);
        for (auto& f : tone) f.prepare(spec);
        juce::dsp::Reverb::Parameters rp;
        rp.roomSize = 0.35f; rp.damping = 0.55f; rp.wetLevel = 0.18f;
        rp.dryLevel = 0.82f; rp.width = 0.8f; rp.freezeMode = 0.0f;
        reverb.setParameters(rp);
        reverb.prepare(spec);
        envelope = 0.0f;
        limiterGain = 1.0f;
        levelGain.reset(sampleRate, 0.08);
        levelGain.setCurrentAndTargetValue(1.0f);
        updateVoiceFilters();
    }
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& l) const override {
        return l.getMainInputChannelSet() == l.getMainOutputChannelSet()
            && !l.getMainOutputChannelSet().isDisabled();
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
        juce::ScopedNoDenormals noDenormals;
        const int samples = buffer.getNumSamples();
        const int channels = juce::jmin(buffer.getNumChannels(), 2);
        for (int c = getTotalNumInputChannels(); c < buffer.getNumChannels(); ++c)
            buffer.clear(c, 0, samples);

        const auto plugin = juce::String(JucePlugin_Name);
        if (plugin.contains("Voice")) processVoice(buffer, samples, channels);
        else if (plugin.contains("Space")) processSpace(buffer, samples, channels);
        else if (plugin.contains("Master")) processMaster(buffer, samples, channels);
        else if (plugin.contains("Effector")) processEffector(buffer, samples, channels);
        // Harmony and Tune remain transparent until their pitch engines are implemented.
    }

    juce::AudioProcessorEditor* createEditor() override { return new juce::GenericAudioProcessorEditor(*this); }
    bool hasEditor() const override { return true; }
    double getTailLengthSeconds() const override { return 2.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock& dest) override {
        auto state = apvts.copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, dest);
    }
    void setStateInformation(const void* data, int size) override {
        std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
        if (xml && xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

private:
    using Range = juce::NormalisableRange<float>;
    static juce::AudioProcessorValueTreeState::ParameterLayout makeParameters() {
        juce::AudioProcessorValueTreeState::ParameterLayout p;
        auto add = [&p](const char* id, const char* name, float lo, float hi, float def, float step = 0.01f) {
            p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id, 1}, name,
                Range(lo, hi, step), def));
        };
        // Shared voice / dynamics controls
        add("amount", "Amount", 0.0f, 1.0f, 0.65f);
        add("input", "Input", -18.0f, 18.0f, 0.0f, 0.1f);
        add("output", "Output", -18.0f, 12.0f, 0.0f, 0.1f);
        add("hpf", "Clean Low / High-pass (Hz)", 20.0f, 300.0f, 75.0f, 1.0f);
        add("low", "Low Cleanup", 0.0f, 12.0f, 2.0f, 0.1f);
        add("mid", "Mid Cleanup", 0.0f, 12.0f, 1.0f, 0.1f);
        add("high", "High Cleanup", 0.0f, 12.0f, 1.5f, 0.1f);
        add("deess", "De-ess", 0.0f, 1.0f, 0.35f);
        add("leveler", "Leveler", 0.0f, 1.0f, 0.3f);
        add("color", "Color", 0.0f, 1.0f, 0.15f);
        add("colorMix", "Color Mix", 0.0f, 1.0f, 0.25f);
        add("reverbMix", "Reverb Mix", 0.0f, 1.0f, 0.08f);
        add("limiter", "Limiter", 0.0f, 1.0f, 0.8f);
        add("width", "Width", 0.0f, 2.0f, 1.0f);
        add("room", "Room", 0.0f, 1.0f, 0.3f);
        add("decay", "Decay", 0.1f, 0.98f, 0.55f);
        add("tone", "Tone", 0.0f, 1.0f, 0.5f);
        add("mod", "Modulation", 0.0f, 1.0f, 0.2f);
        add("attack", "Attack", 0.0f, 1.0f, 0.25f);
        add("smooth", "Smooth", 0.0f, 1.0f, 0.35f);
        add("radio", "Radio Character", 0.0f, 1.0f, 0.0f);
        add("telephone", "Telephone Character", 0.0f, 1.0f, 0.0f);
        add("wireless", "Wireless Character", 0.0f, 1.0f, 0.0f);
        add("retune", "Retune Speed (ms)", 0.0f, 400.0f, 80.0f, 1.0f);
        add("humanize", "Humanize", 0.0f, 1.0f, 0.2f);
        add("flex", "Flex Tune", 0.0f, 1.0f, 0.2f);
        add("tracking", "Tracking", 0.0f, 1.0f, 0.5f);
        add("harmony", "Harmony Amount", 0.0f, 1.0f, 0.5f);
        add("voice2", "Second Voice", 0.0f, 1.0f, 0.0f);
        return p;
    }
    float value(const char* id) const { return apvts.getRawParameterValue(id)->load(); }

    void updateVoiceFilters() {
        const auto sampleRate = currentSampleRate;
        if (sampleRate <= 0.0) return;
        *highPass[0].coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, value("hpf"), 0.7071f);
        *highPass[1].coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, value("hpf"), 0.7071f);
        *lowPass[0].coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 16000.0f, 0.7071f);
        *lowPass[1].coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 16000.0f, 0.7071f);
        const float lowDb = -value("low");
        const float midDb = -value("mid");
        const float highDb = -value("high");
        for (int c = 0; c < 2; ++c) {
            *tone[c].coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 180.0f, 0.85f, juce::Decibels::decibelsToGain(lowDb));
            *cleanupMid[c].coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 520.0f, 1.0f, juce::Decibels::decibelsToGain(midDb));
            *cleanupHigh[c].coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 4200.0f, 0.8f, juce::Decibels::decibelsToGain(highDb));
        }
    }

    void processVoice(juce::AudioBuffer<float>& b, int n, int ch) {
        updateVoiceFilters();
        const float amt = value("amount"), in = juce::Decibels::decibelsToGain(value("input"));
        const float out = juce::Decibels::decibelsToGain(value("output"));
        const float levelAmount = value("leveler") * amt;
        const float color = value("color") * value("colorMix") * amt;
        const float de = value("deess") * amt;
        for (int c = 0; c < ch; ++c) {
            auto* x = b.getWritePointer(c);
            for (int i = 0; i < n; ++i) {
                float s = x[i] * in;
                const float a = std::abs(s);
                const float detector = a * a;
                const float detectorCoeff = detector > envelope ? 0.996f : 0.9997f;
                envelope = detectorCoeff * envelope + (1.0f - detectorCoeff) * detector;
                const float levelDb = juce::Decibels::gainToDecibels(std::sqrt(juce::jmax(envelope, 1.0e-8f)));
                const float targetDb = juce::jlimit(-24.0f, -15.0f, -20.0f - 4.0f * (1.0f - amt));
                const float desiredDb = juce::jlimit(-6.0f, 6.0f, (targetDb - levelDb) * levelAmount * 0.45f);
                levelGain.setTargetValue(juce::Decibels::decibelsToGain(desiredDb));
                s *= levelGain.getNextValue();
                // Gentle high-frequency transient softening; intentionally not a hard gate.
                const float softened = s - de * 0.12f * (s - previous[c]);
                previous[c] = s;
                s = softened * (1.0f - color) + std::tanh(s * (1.0f + 2.0f * color)) * color;
                x[i] = s;
            }
        }
        const float wet = value("reverbMix");
        if (wet > 0.0f) {
            juce::AudioBuffer<float> dry(b.getNumChannels(), n);
            for (int c = 0; c < ch; ++c) dry.copyFrom(c, 0, b, c, 0, n);
            juce::dsp::AudioBlock<float> block(b);
            auto sub = block.getSubBlock(0, (size_t)n).getSubsetChannelBlock(0, (size_t)ch);
            juce::dsp::ProcessContextReplacing<float> ctx(sub);
            reverb.process(ctx);
            for (int c = 0; c < ch; ++c)
                for (int i = 0; i < n; ++i)
                    b.setSample(c, i, dry.getSample(c, i) * (1.0f - wet) + b.getSample(c, i) * wet);
        }
        for (int c = 0; c < ch; ++c) {
            auto* channel = b.getWritePointer(c);
            for (int i = 0; i < n; ++i) {
                float sample = channel[i];
                sample = highPass[c].processSample(sample);
                sample = tone[c].processSample(sample);
                sample = cleanupMid[c].processSample(sample);
                channel[i] = cleanupHigh[c].processSample(sample);
            }
        }
        const float ceiling = juce::Decibels::decibelsToGain(-0.5f - 5.5f * value("limiter"));
        const float release = std::exp(-1.0f / (0.08f * static_cast<float>(currentSampleRate)));
        for (int i = 0; i < n; ++i) {
            float peak = 0.0f;
            for (int c = 0; c < ch; ++c)
                peak = juce::jmax(peak, std::abs(b.getSample(c, i) * out));
            const float targetGain = peak > ceiling ? ceiling / peak : 1.0f;
            if (targetGain < limiterGain)
                limiterGain = targetGain;
            else
                limiterGain = release * limiterGain + (1.0f - release) * targetGain;
            for (int c = 0; c < ch; ++c) {
                const float gained = b.getSample(c, i) * out * limiterGain;
                b.setSample(c, i, juce::jlimit(-ceiling, ceiling, gained));
            }
        }
    }

    void processSpace(juce::AudioBuffer<float>& b, int n, int ch) {
        juce::dsp::Reverb::Parameters rp;
        rp.roomSize = value("room"); rp.damping = 1.0f - value("tone");
        rp.wetLevel = 0.35f; rp.dryLevel = 0.85f; rp.width = value("width");
        rp.freezeMode = 0.0f; reverb.setParameters(rp);
        juce::dsp::AudioBlock<float> block(b);
        auto sub = block.getSubBlock(0, (size_t)n).getSubsetChannelBlock(0, (size_t)ch);
        juce::dsp::ProcessContextReplacing<float> ctx(sub);
        reverb.process(ctx);
    }
    void processMaster(juce::AudioBuffer<float>& b, int n, int ch) {
        const float width = value("width");
        for (int i=0;i<n;++i) {
            if (ch == 2) {
                const float l=b.getSample(0,i), r=b.getSample(1,i);
                const float mid=(l+r)*0.5f, side=(l-r)*0.5f*width;
                b.setSample(0,i,mid+side); b.setSample(1,i,mid-side);
            }
            for (int c=0;c<ch;++c) {
                float s=b.getSample(c,i);
                s = s * (1.0f - value("smooth")*0.12f) + std::tanh(s*1.05f)*value("smooth")*0.12f;
                b.setSample(c,i,s*juce::Decibels::decibelsToGain(value("output")));
            }
        }
    }
    void processEffector(juce::AudioBuffer<float>& b, int n, int ch) {
        const float radio=value("radio"), phone=value("telephone"), wireless=value("wireless");
        const float blend=juce::jlimit(0.0f,1.0f,juce::jmax(radio,juce::jmax(phone,wireless)));
        const float drive=1.0f+2.5f*blend;
        for(int c=0;c<ch;++c) {
            auto* x=b.getWritePointer(c);
            for(int i=0;i<n;++i) {
                float s=x[i];
                const float crushed=std::tanh(s*drive);
                s=s*(1.0f-blend)+crushed*blend;
                if(blend>0.001f) s *= (1.0f-0.18f*blend);
                x[i]=s;
            }
        }
    }

    juce::AudioProcessorValueTreeState apvts;
    juce::dsp::IIR::Filter<float> highPass[2], lowPass[2], deEss[2], tone[2];
    juce::dsp::IIR::Filter<float> cleanupMid[2], cleanupHigh[2];
    juce::dsp::Reverb reverb;
    juce::SmoothedValue<float> levelGain;
    double currentSampleRate = 0.0;
    float limiterGain = 1.0f;
    float envelope=0.0f, previous[2]{0.0f,0.0f};
};

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new BCrannelAudioProcessor(); }

