#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class Envelope : public Component
{
public:
	Envelope()
	{
		attack = 0.1f;
		decay = 1.0f;
		sustain = 0.8f;
		release = 0.8f;
		output = 0.0f;
	}

	void reset() { trigger = 0; }

	void triggerNote() { trigger = 1; }

	void releaseNote() { trigger = 4; }

	bool isPlaying() { return (trigger != 0); }

	float adsr(float sampleRate)
	{
		if (trigger == 1)
		{
			output += ((1.0 / attack) / sampleRate);
			if (output >= 1.0) { output = 1.0; trigger = 2; }
		}
		if (trigger == 2)
		{
			output -= ((1.0 / decay) / sampleRate);
			if (output <= sustain) { output = sustain; trigger = 3; }
		}
		if (trigger == 4)
		{
			output -= ((1.0 / release) / sampleRate);
			if (output <= 0.0) { output = 0.0; reset(); }
		}
		return output;
	}

public:
	float attack, sustain, decay, release;

private:
	int trigger = 0;
	float output;
};
