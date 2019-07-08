#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class Oscillator : public Component
{
public:

	Oscillator()
	{
		currentWaveShape = sineWave;
		time = 0.0;
		deltaTime = 0.0;
		output = 0.0;
	}

	float oscillate()
	{
		switch (currentWaveShape)
		{
		case sineWave:
			return sine();
		case sawWave:
			return saw();
		case squareWave:
			return square();
		case triangleWave:
			return triangle();
		case noiseWave: return noise();
		default: return sine();
		}
	}

	float sine()
	{
		// To Do
		return 0.0;
	}

	float saw()
	{
		// To Do
		return 0.0;
	}

	float square()
	{
		// To Do
		return 0.0;
	}

	float triangle()
	{
		// To do
		return 0.0;
	}

	float noise()
	{
		return 0.0;
	}

	void setTime(float t) { time = t; }
	void setDeltaTime(float dt) { deltaTime = dt; }
	void setOutput(float v) { output = v; }

public:
	WaveShape currentWaveShape;

private:
	float time;
	float deltaTime;
	float output;
};
