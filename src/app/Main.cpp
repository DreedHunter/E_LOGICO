// SPDX-License-Identifier: AGPL-3.0-or-later
#include "AppController.h"

#include "elogico/core/Version.h"
#include "elogico/gui/MainComponent.h"

#include <juce_gui_extra/juce_gui_extra.h>

namespace elogico
{

class MainWindow final : public juce::DocumentWindow
{
public:
    MainWindow(const juce::String& title, AppController& app)
        : DocumentWindow(title, juce::Colour(0xff15171b), DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(new gui::MainComponent(app.document(), app.engine()), true);
        setResizable(true, true);
        centreWithSize(getWidth(), getHeight());
        setVisible(true);
    }

    void closeButtonPressed() override { juce::JUCEApplication::getInstance()->systemRequestedQuit(); }
};

class ELogicoApplication final : public juce::JUCEApplication, private juce::Timer
{
public:
    const juce::String getApplicationName() override { return kProductName; }
    const juce::String getApplicationVersion() override { return kVersionString; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String& commandLine) override
    {
        controller_ = std::make_unique<AppController>();

        const auto path = commandLine.unquoted().trim();
        if (path.endsWithIgnoreCase(".logicx"))
            if (auto error = controller_->importProject(path.toStdString()); !error.empty())
                juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Import",
                                                       juce::String::fromUTF8(error.c_str()));

        window_ = std::make_unique<MainWindow>(
            juce::String(kProductName) + " — " + juce::String::fromUTF8(controller_->document().project().name.c_str()),
            *controller_);

        if (auto error = controller_->startAudio(); !error.empty())
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Audio",
                                                   juce::String::fromUTF8(error.c_str()));
        startTimerHz(10);
    }

    void shutdown() override
    {
        stopTimer();
        window_.reset();
        controller_.reset();
    }

    void systemRequestedQuit() override { quit(); }

private:
    void timerCallback() override { controller_->idle(); }

    std::unique_ptr<AppController> controller_;
    std::unique_ptr<MainWindow> window_;
};

} // namespace elogico

START_JUCE_APPLICATION(elogico::ELogicoApplication)
