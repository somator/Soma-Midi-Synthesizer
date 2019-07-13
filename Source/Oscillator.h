#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
//==============================================================================
enum WaveShape { sineWave, sawWave, squareWave, triangleWave, noiseWave };

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

	void updateWaveshape(int index)
	{
		switch (index)
		{
		case 1:
			currentWaveShape = sineWave;
		case 2:
			currentWaveShape = sawWave;
		case 3:
			currentWaveShape = squareWave;
		case 4:
			currentWaveShape = triangleWave;
		case 5:
			currentWaveShape = noiseWave;
		}
	}

	double oscillate(double currentAngle)
	{
		switch (currentWaveShape)
		{
		case sineWave:
			return sine(currentAngle);
		case sawWave:
			return saw(currentAngle);
		case squareWave:
			return square(currentAngle);
		case triangleWave:
			return triangle(currentAngle);
		case noiseWave: return noise(currentAngle);
		default: 
			return sine(currentAngle);
		}
	}

	double sine(double currentAngle)
	{
		return std::sin(currentAngle);
	}

	double saw(double currentAngle)
	{
		// To Do
		return 0.0;
	}

	double square(double currentAngle)
	{
		// Not Working
		if (std::fmod(currentAngle, 2.0 * MathConstants<double>::pi) >= MathConstants<double>::pi)
		{
			return -1.0;
		}
		else {
			return 1.0;
		}
	}

	double triangle(double currentAngle)
	{
		// To do
		return 0.0;
	}

	double noise(double currentAngle)
	{
		float r = rand() / (float)RAND_MAX;
		output = r * 2 - 1;
		return(output);
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
