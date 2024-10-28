/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
LModelAudioProcessor::LModelAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
	//firInit(&firtest);
	for (int i = 0; i < FFTFilterSize; ++i)
	{
		window[i] = 0.5 - 0.5 * cosf((float)i / FFTFilterSize * 2.0 * M_PI);//加窗
	}
	FFTFilterInit(&ffttestl, window);
	FFTFilterInit(&ffttestr, window);
}


juce::AudioProcessorValueTreeState::ParameterLayout LModelAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	layout.add(std::make_unique<juce::AudioParameterFloat>("freql", "FreqL", 0.0, 1.0, 0.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("freqr", "FreqR", 0.0, 1.0, 0.2));

	//layout.add(std::make_unique<juce::AudioParameterFloat>("minKey", "minKey", 0, 128, 24));

	return layout;
}

LModelAudioProcessor::~LModelAudioProcessor()
{
}

//==============================================================================
const juce::String LModelAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool LModelAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool LModelAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool LModelAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double LModelAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int LModelAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int LModelAudioProcessor::getCurrentProgram()
{
	return 0;
}

void LModelAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String LModelAudioProcessor::getProgramName(int index)
{
	return "Monoband Delay";
}

void LModelAudioProcessor::changeProgramName(int index, const juce::String& newName)
{

}

//==============================================================================
void LModelAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
}

void LModelAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LModelAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif


void LModelAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	const int numSamples = buffer.getNumSamples();
	float* wavbufl = buffer.getWritePointer(0);
	float* wavbufr = buffer.getWritePointer(1);
	const float* recbufl = buffer.getReadPointer(0);
	const float* recbufr = buffer.getReadPointer(1);

	float SampleRate = getSampleRate();

	float freql = *Params.getRawParameterValue("freql");
	float freqr = *Params.getRawParameterValue("freqr");
	freql = expf((freql - 1.0) * 8);
	freqr = expf((freqr - 1.0) * 8);
	//firApplyBPF(&firtest, freql, freqr, 512);
	//firProcStereo(&firtest, recbufl, recbufr, wavbufl, wavbufr, numSamples);//垃圾fir又慢又大
	for (int i = 0; i < FFTFilterSize / 2; ++i)
	{
		float x = (float)i / FFTFilterSize / 2;
		/*
		complex_f32_t disp = { cosf(x * x * freql * 1000000.0),sinf(x * x * freql * 1000000.0) };
		ffttestl.core[i] = disp;
		ffttestr.core[i] = disp;
		*/
		if (x >= freql && x <= freqr)
		{
			ffttestl.core[i].re = 1.0;
			ffttestr.core[i].re = 1.0;
			ffttestl.core[i].im = 0.0;
			ffttestr.core[i].im = 0.0;
		}
		else
		{
			ffttestl.core[i].re = 0.0;
			ffttestr.core[i].re = 0.0;
			ffttestl.core[i].im = 0.0;
			ffttestr.core[i].im = 0.0;
		}
	}
	FFTFilterProc(&ffttestl, recbufl, wavbufl, numSamples);
	FFTFilterProc(&ffttestr, recbufr, wavbufr, numSamples);

}

//==============================================================================
bool LModelAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LModelAudioProcessor::createEditor()
{
	return new LModelAudioProcessorEditor(*this);

	//return new juce::GenericAudioProcessorEditor(*this);//×Ô¶¯»æÖÆ(µ÷ÊÔ)
}

//==============================================================================

void LModelAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.

	// ´´½¨Ò»¸ö XML ½Úµã
	juce::XmlElement xml("LM_MONOBANDELAY_Settings");
	/*
	juce::MemoryBlock eqDataBlock;
	eqDataBlock.append(&manager, sizeof(ResonatorManager));
	juce::String base64Data = eqDataBlock.toBase64Encoding();
	xml.setAttribute("VIB_MANAGER", base64Data);//Ìí¼ÓresonanceÊý¾Ý
	*/
	auto state = Params.copyState();
	xml.setAttribute("Knob_Data_MONOBANDELAY", state.toXmlString());//Ìí¼ÓÐýÅ¥Êý¾Ý

	juce::String xmlString = xml.toString();
	destData.append(xmlString.toRawUTF8(), xmlString.getNumBytesAsUTF8());
}

void LModelAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	  // ½« data ×ª»»Îª×Ö·û´®ÒÔ½âÎö XML
	juce::String xmlString(static_cast<const char*>(data), sizeInBytes);

	// ½âÎö XML
	std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(xmlString));
	if (xml == nullptr || !xml->hasTagName("LM_MONOBANDELAY_Settings"))
	{
		DBG("Error: Unable to load XML settings");
		return;
	}

	/*
	juce::String base64Data = xml->getStringAttribute("VIB_MANAGER");
	juce::MemoryBlock eqDataBlock;
	eqDataBlock.fromBase64Encoding(base64Data);
	if (eqDataBlock.getData() != NULL)
	{
		std::memcpy(&manager, eqDataBlock.getData(), sizeof(ResonatorManager));
	}*/

	auto KnobDataXML = xml->getStringAttribute("Knob_Data_MONOBANDELAY");
	Params.replaceState(juce::ValueTree::fromXml(KnobDataXML));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new LModelAudioProcessor();
}
