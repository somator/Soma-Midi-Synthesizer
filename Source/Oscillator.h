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
		time = 0.0;
		deltaTime = 0.0;
		output = 0.0;
	}

	void updateWaveshape(int index)
	{
		waveIndex = index;
	}

	void setDeltaTime(double dt) {
		deltaTime = dt;
	}

	double getDeltaTime() {
		return deltaTime;
	}

	void clearDeltaTime() {
		deltaTime = 0.0;
	}

	void clearTime() {
		time = 0.0;
	}

	double oscillate()
	{
		switch (waveIndex)
		{
		case 0:
			return sine();
		case 1:
			return saw();
		case 2:
			return square();
		case 3:
			return triangle();
		case 4: return noise();
		default: 
			return sine();
		}
	}

	double sine()
	{
		output = std::sin(time * 2.0 * MathConstants<double>::pi);
		time += (deltaTime);
		return(output);
	}

	double saw()
	{
		output = (time / MathConstants<double>::pi);
		if (time >= MathConstants<double>::pi) time -= (2.0 * MathConstants<double>::pi);
		time += (deltaTime);
		return(output);
	}

	double square()
	{
		if (time < MathConstants<double>::pi) output = -1;
		if (time > MathConstants<double>::pi) output = 1;
		if (time >= 2.0 * MathConstants<double>::pi) time -= 2.0 * MathConstants<double>::pi;
		time += (deltaTime);
		return(output);
	}

	double triangle()
	{
		if (time >= 2.0 * MathConstants<double>::pi) time -= 2.0 * MathConstants<double>::pi;
		time += (deltaTime);
		if (time <= MathConstants<double>::pi) {
			output = ((time / (2.0 * MathConstants<double>::pi)) - 0.25) * 4;
		}
		else {
			output = (1.0 - (time / (2.0 * MathConstants<double>::pi)) - 0.25) * 4;
		}
		return(output);
	}

	double noise()
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
	int waveIndex = 0;

private:
	float time;
	float deltaTime;
	float output;
};
