#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

//==============================================================================
class SomaMidiSynthApplication  : public JUCEApplication
{
public:
    //==============================================================================
    SomaMidiSynthApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        mainWindow.reset (new MainWindow ("SomaMidiSynthApplication", new MainComponent(), *this));
    }

    void shutdown() override
    {
        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {

    }

    //==============================================================================
private:
	class MainWindow : public DocumentWindow
	{
	public:
		MainWindow(const String& name, Component* c, JUCEApplication& a)
			: DocumentWindow(name, Desktop::getInstance().getDefaultLookAndFeel()
				.findColour(ResizableWindow::backgroundColourId),
				DocumentWindow::allButtons),
			app(a)
		{
			setUsingNativeTitleBar(true);
			setContentOwned(c, true);

#if JUCE_ANDROID || JUCE_IOS
			setFullScreen(true);
#else
			setResizable(true, false);
			setResizeLimits(300, 250, 10000, 10000);
			centreWithSize(getWidth(), getHeight());
#endif

			setVisible(true);
		}

		void closeButtonPressed() override
		{
			app.systemRequestedQuit();
		}

	private:
		JUCEApplication & app;

		//==============================================================================
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
	};

	std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
START_JUCE_APPLICATION (SomaMidiSynthApplication)
