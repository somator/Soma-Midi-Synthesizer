#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
enum WaveShape { sineWave, sawWave, squareWave, triangleWave, noiseWave };

//==============================================================================
class SynthParameters : public Component
{
public:
	SynthParameters()
	{
		setDefaultParams();
	}

	void setDefaultParams()
	{
		selectedWave = sineWave;
		attackTime = 0.01;
		decayTime = 0.5;
		sustainLevel = 0.2;
		releaseTime = 0.3;
	}

	void setWaveShape(int index)
	{
		switch (index)
		{
		case 0:
			selectedWave = sineWave;
		case 1:
			selectedWave = sawWave;
		case 2:
			selectedWave = squareWave;
		case 3:
			selectedWave = triangleWave;
		case 4:
			selectedWave = noiseWave;
		default:
			selectedWave = sineWave;
		}
	}

public:
	WaveShape selectedWave;
	float attackTime, decayTime, sustainLevel, releaseTime;
};

