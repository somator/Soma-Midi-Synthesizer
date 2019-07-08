#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Knob.h"
#include "SynthParameters.h"
#include "Synth.h"

//==============================================================================
class MainComponent : public AudioAppComponent,
	public Slider::Listener,
	private Timer
{
public:
	MainComponent() : synthAudioSource(keyboardState, &parameters),
		keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard)
	{
		Gui();

		addAndMakeVisible(keyboardComponent);
		setAudioChannels(0, 2);

		setSize(600, 360);
		startTimer(400);

		addAndMakeVisible(midiInputListLabel);
		midiInputListLabel.setText("MIDI Input:", dontSendNotification);
		midiInputListLabel.attachToComponent(&midiInputList, true);
		addAndMakeVisible(midiInputList);
		midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
		auto midiInputs = MidiInput::getDevices();
		midiInputList.addItemList(midiInputs, 1);
		midiInputList.onChange = [this] { setMidiInput(midiInputList.getSelectedItemIndex()); };
		for (auto midiInput : midiInputs)
		{
			if (deviceManager.isMidiInputEnabled(midiInput))
			{
				setMidiInput(midiInputs.indexOf(midiInput));
				break;
			}
		}
		if (midiInputList.getSelectedId() == 0)
			setMidiInput(0);
	}

	void setMidiInput(int index)
	{
		auto list = MidiInput::getDevices();
		deviceManager.removeMidiInputCallback(list[lastInputIndex], synthAudioSource.getMidiCollector());
		auto newInput = list[index];
		if (!deviceManager.isMidiInputEnabled(newInput))
			deviceManager.setMidiInputEnabled(newInput, true);
		deviceManager.addMidiInputCallback(newInput, synthAudioSource.getMidiCollector());
		midiInputList.setSelectedId(index + 1, dontSendNotification);
		lastInputIndex = index;
	}

	~MainComponent()
	{
		shutdownAudio();
	}

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
	{
		synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
	}

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		synthAudioSource.getNextAudioBlock(bufferToFill);
	}

	void releaseResources() override
	{
		synthAudioSource.releaseResources();
	}

	//==============================================================================
	void setWaveform(int index)
	{
		waveformList.setSelectedId(index + 1, dontSendNotification);
		synthAudioSource.params->setWaveShape(index);
		synthAudioSource.updateParameters();
	}

	void sliderValueChanged(Slider* slider) override
	{
		float value = (float)(slider->getValue());
		if (slider == &volAttackSlider)
			synthAudioSource.params->attackTime = value;
		else if (slider == &volDecaySlider)
			synthAudioSource.params->decayTime = value;
		else if (slider == &volSustainSlider)
			synthAudioSource.params->sustainLevel = value;
		else if (slider == &volReleaseSlider)
			synthAudioSource.params->releaseTime = value;

		synthAudioSource.updateParameters();
	}

	//==============================================================================
	void Gui()
	{
		// Waveform ComboBox
		addAndMakeVisible(waveformListLabel);
		waveformListLabel.setText("Waveform:", dontSendNotification);
		waveformListLabel.attachToComponent(&waveformList, true);

		addAndMakeVisible(waveformList);
		waveformList.setTextWhenNoChoicesAvailable("No Waveforms Enabled");
		waveformList.addItemList(waveforms, 1);
		waveformList.onChange = [this] { setWaveform(waveformList.getSelectedItemIndex()); };
		for (auto waveform : waveforms)
		{
			setWaveform(waveforms.indexOf(waveform));
			break;
		}
		if (waveformList.getSelectedId() == 0)
			setWaveform(0);

		addVolumeEnvelope();
		addFilterEnvelope();
		addKnobs();
	}

	void addVolumeEnvelope()
	{
		// Envelope Label
		addAndMakeVisible(volEnvLabel);
		volEnvLabel.setFont(Font(14, 9));
		volEnvLabel.setColour(Label::textColourId, Colours::lightgrey);
		volEnvLabel.setText("Amplitude Envelope", dontSendNotification);

		// Attack Slider
		volAttackSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
		volAttackSlider.setRange(0.01f, 10.0f);
		volAttackSlider.setSkewFactorFromMidPoint(1.0f);
		volAttackSlider.setValue(0.1f);
		volAttackSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
		volAttackSlider.addListener(this);
		addAndMakeVisible(volAttackSlider);

		// Decay Slider
		volDecaySlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
		volDecaySlider.setRange(0.01f, 10.0f);
		volDecaySlider.setSkewFactorFromMidPoint(1.0f);
		volDecaySlider.setValue(1.0f);
		volDecaySlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
		volDecaySlider.addListener(this);
		addAndMakeVisible(volDecaySlider);

		// Sustain Slider
		volSustainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
		volSustainSlider.setRange(0.0f, 1.0f);
		volSustainSlider.setValue(0.8f);
		volSustainSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
		volSustainSlider.addListener(this);
		addAndMakeVisible(volSustainSlider);

		// Release Slider
		volReleaseSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
		volReleaseSlider.setRange(0.01f, 10.0f);
		volReleaseSlider.setSkewFactorFromMidPoint(1.0f);
		volReleaseSlider.setValue(0.8f);
		volReleaseSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
		volReleaseSlider.addListener(this);
		addAndMakeVisible(volReleaseSlider);

		// ADSR Labels
		juce::Font boldFont = Font(14, 7);
		volAttackLabel.setFont(boldFont);
		volDecayLabel.setFont(boldFont);
		volSustainLabel.setFont(boldFont);
		volReleaseLabel.setFont(boldFont);

		volAttackLabel.setText("A", dontSendNotification);
		volDecayLabel.setText("D", dontSendNotification);
		volSustainLabel.setText("S", dontSendNotification);
		volReleaseLabel.setText("R", dontSendNotification);

		addAndMakeVisible(volAttackLabel);
		addAndMakeVisible(volDecayLabel);
		addAndMakeVisible(volSustainLabel);
		addAndMakeVisible(volReleaseLabel);
	}

	void addFilterEnvelope()
	{
		// Filter Envelope Label
		addAndMakeVisible(filterEnvLabel);
		filterEnvLabel.setFont(Font(14, 9));
		filterEnvLabel.setColour(Label::textColourId, Colours::lightgrey);
		filterEnvLabel.setText("Filter Envelope", dontSendNotification);

		// Filter Attack
		filterAttackSlider.setRange(0.0, 1.0);
		filterAttackSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
		filterAttackSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

		addAndMakeVisible(filterAttackSlider);
		addAndMakeVisible(filterAttackLabel);

		// Filter Decay
		filterDecaySlider.setRange(0.0, 1.0);
		filterDecaySlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
		filterDecaySlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

		addAndMakeVisible(filterDecaySlider);
		addAndMakeVisible(filterDecayLabel);

		// Filter Sustain
		filterSustainSlider.setRange(0.0, 1.0);
		filterSustainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
		filterSustainSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

		addAndMakeVisible(filterSustainSlider);
		addAndMakeVisible(filterSustainLabel);

		// Filter Release
		filterReleaseSlider.setRange(0.0, 1.0);
		filterReleaseSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
		filterReleaseSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

		addAndMakeVisible(filterReleaseSlider);
		addAndMakeVisible(filterReleaseLabel);

		// Bold
		juce::Font boldFont = Font(14, 7);
		filterAttackLabel.setFont(boldFont);
		filterDecayLabel.setFont(boldFont);
		filterSustainLabel.setFont(boldFont);
		filterReleaseLabel.setFont(boldFont);

		// Set Text
		filterAttackLabel.setText("A", dontSendNotification);
		filterDecayLabel.setText("D", dontSendNotification);
		filterSustainLabel.setText("S", dontSendNotification);
		filterReleaseLabel.setText("R", dontSendNotification);
	}

	void addKnobs()
	{
		addAndMakeVisible(knob1);
		knob1.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		knob1.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		knob1.setLookAndFeel(&knob);

		addAndMakeVisible(knob2);
		knob2.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		knob2.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		knob2.setLookAndFeel(&knob);

		addAndMakeVisible(knob3);
		knob3.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		knob3.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		knob3.setLookAndFeel(&knob);

		addAndMakeVisible(knob4);
		knob4.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		knob4.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		knob4.setLookAndFeel(&knob);
	}

	void paint(Graphics& g) override
	{
		Rectangle<float> areaFill(11, 40, getWidth() - 22, getHeight() - 168);
		g.setColour(Colour(66, 76, 84));
		g.fillRoundedRectangle(areaFill, 2.0f);

		Rectangle<float> moduleArea(10, 39, getWidth() - 20, getHeight() - 166);
		g.setColour(Colour(36, 44, 46));
		g.drawRoundedRectangle(moduleArea, 5.0f, 2.5f);

		Rectangle<float> ampEnvArea(20, 45, 154, getHeight() - 179);
		Rectangle<float> filterEnvArea(426, 45, 154, getHeight() - 179);

		g.setColour(Colours::slategrey);
		g.drawRoundedRectangle(ampEnvArea, 2.0f, 2.5f);
		g.drawRoundedRectangle(filterEnvArea, 2.0f, 2.5f);

		g.setColour(Colours::lightgrey);
		g.fillRoundedRectangle(ampEnvArea, 3.0f);
		g.fillRoundedRectangle(filterEnvArea, 3.0f);

		Rectangle<float> ampEnvLabelArea(24, 48, 146, 20);
		Rectangle<float> filterEnvLabelArea(430, 48, 146, 20);

		g.setColour(Colours::slategrey);
		g.drawRoundedRectangle(ampEnvLabelArea, 3.0f, 2.0f);
		g.drawRoundedRectangle(filterEnvLabelArea, 3.0f, 2.0f);

		g.setColour(Colour(36, 44, 46));
		g.fillRoundedRectangle(ampEnvLabelArea, 3.5f);
		g.fillRoundedRectangle(filterEnvLabelArea, 3.5f);

		// Module tbd, probably filter parameters?
		Rectangle<float> centerModule(183, 45, 234, getHeight() - 179);
		g.setColour(Colours::slategrey);
		g.drawRoundedRectangle(centerModule, 2.0f, 2.5f);
		g.setColour(Colours::lightgrey);
		g.fillRoundedRectangle(centerModule, 3.0f);

		Rectangle<float> centerLabelArea(187, 48, 226, 20);
		g.setColour(Colours::slategrey);
		g.drawRoundedRectangle(centerLabelArea, 3.0f, 2.0f);
		g.setColour(Colour(36, 44, 46));
		g.fillRoundedRectangle(centerLabelArea, 3.5f);
	}

	void resized() override
	{
		midiInputList.setBounds(350, 10, 240, 20);
		waveformList.setBounds(85, 10, 90, 20);
		keyboardComponent.setBounds(10, getHeight() - 120, getWidth() - 20, 110);

		volEnvLabel.setBounds(37, 46, 120, 20);
		volAttackLabel.setBounds(28, 73, 20, 10);
		volDecayLabel.setBounds(68, 73, 20, 10);
		volSustainLabel.setBounds(108, 73, 20, 10);
		volReleaseLabel.setBounds(148, 73, 20, 10);

		volAttackSlider.setBounds(20, 80, 35, getHeight() - 212);
		volDecaySlider.setBounds(60, 80, 35, getHeight() - 212);
		volSustainSlider.setBounds(100, 80, 35, getHeight() - 212);
		volReleaseSlider.setBounds(140, 80, 35, getHeight() - 212);

		filterEnvLabel.setBounds(453, 46, 120, 20);
		filterAttackLabel.setBounds(434, 73, 20, 10);
		filterDecayLabel.setBounds(474, 73, 20, 10);
		filterSustainLabel.setBounds(514, 73, 20, 10);
		filterReleaseLabel.setBounds(554, 73, 20, 10);

		filterAttackSlider.setBounds(426, 80, 35, getHeight() - 212);
		filterDecaySlider.setBounds(466, 80, 35, getHeight() - 212);
		filterSustainSlider.setBounds(506, 80, 35, getHeight() - 212);
		filterReleaseSlider.setBounds(546, 80, 35, getHeight() - 212);

		knob1.setBounds(200, 80, 30, 30);
		knob2.setBounds(250, 80, 30, 30);
		knob3.setBounds(300, 80, 30, 30);
		knob4.setBounds(350, 80, 30, 30);
	}


private:
	void timerCallback() override
	{
		keyboardComponent.grabKeyboardFocus();
		stopTimer();
	}

	//==========================================================================
	SynthAudioSource synthAudioSource;
	MidiKeyboardState keyboardState;
	MidiKeyboardComponent keyboardComponent;
	ComboBox midiInputList;
	Label midiInputListLabel;
	int lastInputIndex = 0;

	SynthParameters parameters;

	//==============================================================================
	Label volEnvLabel;
	Label volAttackLabel, volDecayLabel, volSustainLabel, volReleaseLabel;
	Slider volAttackSlider, volDecaySlider, volSustainSlider, volReleaseSlider;

	Label filterEnvLabel;
	Label filterAttackLabel, filterDecayLabel, filterSustainLabel, filterReleaseLabel;
	Slider filterAttackSlider, filterDecaySlider, filterSustainSlider, filterReleaseSlider;

	Knob knob;
	Slider knob1, knob2, knob3, knob4;

	Label waveformListLabel;
	ComboBox waveformList;
	juce::StringArray waveforms = { "Sine", "Saw", "Square", "Triangle", "Noise" };
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
