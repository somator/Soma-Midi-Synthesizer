#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class Knob : public LookAndFeel_V4
{
public:
	void drawRotarySlider(Graphics &g, int x, int y, int width, int height,
		float sliderPos, float rotaryStartAngle, float rotaryEndAngle, Slider &slider)
	{
		float diameter = jmin(width, height);
		float radius = diameter / 2;
		float centreX = x + width / 2;
		float centreY = y = height / 2;
		float rx = centreX - radius;
		float ry = centreY - radius;
		float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));

		Rectangle<float> dialArea(rx + 1.0f, ry + 1.0f, diameter - 2.0f, diameter - 2.0f);

		g.setColour(Colours::lightgrey);
		g.fillEllipse(dialArea);

		g.setColour(Colour(51, 60, 65));

		Path dialTick;
		dialTick.addRectangle(0, -radius + 1, 2.0f, radius * 0.5);

		g.fillPath(dialTick, AffineTransform::rotation(angle).translated(centreX, centreY));

		g.setColour(Colour(106, 111, 116));
		g.drawEllipse(rx + 1.0f, ry + 1.0f, diameter - 2.0f, diameter - 2.0f, 1.8f);
	}
};
