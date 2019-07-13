#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Oscillator.h"
#include "Envelope.h"

//==============================================================================
struct SynthSound : public SynthesiserSound
{
	SynthSound() {}

	bool appliesToNote(int) override { return true; }
	bool appliesToChannel(int) override { return true; }

};

//==============================================================================
struct SynthVoice : public SynthesiserVoice
{
	SynthVoice() {}

	bool canPlaySound(SynthesiserSound* sound) override
	{
		return dynamic_cast<SynthSound*> (sound) != nullptr;
	}

	void startNote(int midiNoteNumber, float velocity,
		SynthesiserSound*, int /*currentPitchWheelPosition*/) override
	{
		currentAngle = 0.0;
		level = velocity * 0.15;
		tailOff = 0.0;

		auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		auto cyclesPerSample = cyclesPerSecond / getSampleRate();

		angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;
		
		env.reset();
		env.triggerNote();
	}

	void stopNote(float /*velocity*/, bool allowTailOff) override
	{
		env.releaseNote();

		if (allowTailOff)
		{
			if (tailOff == 0.0)
				tailOff = 1.0;
		}
		else
		{
			clearCurrentNote();
			angleDelta = 0.0;
		}
	}

	void pitchWheelMoved(int) override {}
	void controllerMoved(int, int) override {}

	void renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
	{
		if (angleDelta != 0.0)
		{
			if (tailOff > 0.0)
			{
				while (--numSamples >= 0)
				{
					auto currentSample = (float)(osc.oscillate(currentAngle) * level * tailOff * env.adsr(getSampleRate()));

					for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
						outputBuffer.addSample(i, startSample, currentSample);

					currentAngle += angleDelta;
					++startSample;

					tailOff *= 0.99;

					if (tailOff <= 0.005)
					{
						clearCurrentNote();

						angleDelta = 0.0;
						break;
					}
				}
			}
			else
			{
				while (--numSamples >= 0)
				{
					auto currentSample = (float)(osc.oscillate(currentAngle) * level * env.adsr(getSampleRate()));

					for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
						outputBuffer.addSample(i, startSample, currentSample);

					currentAngle += angleDelta;
					++startSample;
				}
			}
		}
	}

	void updateEnvelope(double attack, double decay, double sustain, double release)
	{
		env.attack = attack;
		env.decay = decay;
		env.sustain = sustain;
		env.release = release;
	}

	void updateWaveform(int index)
	{
		osc.updateWaveshape(index);
	}

private:
	double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;

	Oscillator osc;
	Envelope env;
};

//==============================================================================
class Synth : public Synthesiser
{
public:
	void updateWaveform(int index)
	{
		for (int i = 0; i < voices.size(); ++i)
		{
			SynthVoice* const voice = dynamic_cast<SynthVoice*>(voices.getUnchecked(i));
			voice->updateWaveform(index);
		}
	}

	void updateEnvelope(double attack, double decay, double sustain, double release)
	{
		for (int i = 0; i < voices.size(); ++i)
		{
			SynthVoice* const voice = dynamic_cast<SynthVoice*>(voices.getUnchecked(i));
			voice->updateEnvelope(attack, decay, sustain, release);
		}
	}
};

//==============================================================================
class SynthAudioSource : public AudioSource
{
public:
	SynthAudioSource(MidiKeyboardState& keyState)
		: keyboardState(keyState)
	{

		for (auto i = 0; i < 4; ++i)
			synth.addVoice(new SynthVoice());

		synth.addSound(new SynthSound());
	}

	void updateWaveform(int index)
	{
		synth.updateWaveform(index);
	}

	void updateEnvelope(double attack, double decay, double sustain, double release)
	{
		synth.updateEnvelope(attack, decay, sustain, release);
	}

	void setUsingSineWaveSound()
	{
		synth.clearSounds();
	}

	void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
	{
		synth.setCurrentPlaybackSampleRate(sampleRate);
		midiCollector.reset(sampleRate);
	}

	void releaseResources() override {}

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		bufferToFill.clearActiveBufferRegion();

		MidiBuffer incomingMidi;
		midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);
		keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample,
			bufferToFill.numSamples, true);

		synth.renderNextBlock(*bufferToFill.buffer, incomingMidi,
			bufferToFill.startSample, bufferToFill.numSamples);
	}

	MidiMessageCollector* getMidiCollector()
	{
		return &midiCollector;
	}

private:
	MidiKeyboardState & keyboardState;
	Synth synth;
	MidiMessageCollector midiCollector;
};
