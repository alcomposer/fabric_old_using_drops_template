// TODO Add license

#ifndef DROPSUI_HPP_INCLUDED
#define DROPSUI_HPP_INCLUDED

#include "external/src/nanosvg.h"
#include "external/src/nanosvgrast.h"

#include "DistrhoUI.hpp"
#include "NanoVG.hpp"
#include "Window.hpp"
#include <vector>

#include "DropsGeometry.hpp"
#include "DropsPlugin.hpp"
// #include "TextButton.hpp"
#include "FileOpenButton.hpp"
#include "ScrollBar.hpp"
#include "Knob.hpp"
#include "DropDown.hpp"
#include "Menu.hpp"
#include "Slider.hpp"
// #include "RadioButton.hpp"
#include "SVGButton.hpp"
#include "HBox.hpp"
#include "VBox.hpp"
#include "SVGImage.hpp"
#include "PopUp.hpp"
#include "DropsColors.hpp"
#include "SVG_Icons.hpp"
#include "SVGImage.hpp"
#include "fonts.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

class DropsUI : public UI,
                // public TextButton::Callback,
                public ScrollBar::Callback,
                public Knob::Callback,
                public DropDown::Callback,
                public Menu::Callback,
                public Slider::Callback,
                public FileOpenButton::Callback,
                // public RadioButton::Callback,
                public SVGButton::Callback
{
public:
    DropsUI();
    ~DropsUI();

protected:
    void parameterChanged(uint32_t index, float value) override;
    void onNanoDisplay() override;
    void uiFileBrowserSelected(const char *filename) override;
    void stateChanged(const char *key, const char *value) override;
    bool onMouse(const MouseEvent &ev) override;
    bool onScroll(const ScrollEvent &) override;
    bool onMotion(const MotionEvent &) override;
    // void onTextButtonClicked(TextButton *textButton) override;
    void onFileOpenButtonClicked(FileOpenButton *fileOpenButton) override;
    void onScrollBarClicked(ScrollBar *scrollBar, bool dragging) override;
    void onDropDownClicked(DropDown *dropdown) override;
    void knobDragStarted(Knob *knob) override;
    void knobDragFinished(Knob *knob) override;
    void knobValueChanged(Knob *knob, float value) override;
    void onSliderValueChanged(Slider *slider, float value) override;
    void onMenuClicked(Menu *menu, uint id, std::string item);
    // void onRadioButtonClicked(RadioButton *radio);
    void onSVGButtonClicked(SVGButton *button);

private:
    template <class T>
    const T &clamp(const T &x, const T &upper, const T &lower)
    {
        return std::min(upper, std::max(x, lower));
    }

    DropsPlugin *plugin;
    ScopedPointer<FileOpenButton> fileopen_button;
    ScopedPointer<ScrollBar> fScrollBarHandle, fLoopStart, fLoopEnd, fSampleIn,
        fSampleOut, fScrollBarLeft, fScrollBarRight;

    ScopedPointer<HBox> hbox_sample;
    ScopedPointer<Slider> fSamplePitch;
    ScopedPointer<DropDown> fSamplePitchKeyCenter,
        fSamplePlayMode, fSamplePlayDirection, fSampleOversampling;
    ScopedPointer<Menu> fNormalizeMenu, fKeyCenterMenu, fPlayModeMenu,
        fDirectionMenu, fOversamplingMenu;
    // amp
    ScopedPointer<VBox> vbox_amp;
    ScopedPointer<HBox> hbox_amp_row_1, hbox_amp_row_2, hbox_amp_row_3;
    ScopedPointer<Knob>
        fAmpEgAttack, fAmpEgDecay, fAmpEgSustain, fAmpEgRelease;
    ScopedPointer<DropDown> fAmpLFOType;
    ScopedPointer<Menu> fAmpLFOTypeMenu;
    ScopedPointer<Knob> fAmpLFOFreq, fAmpLFODepth,fAmpLFOFade;

    // filter
    ScopedPointer<VBox> vbox_filter;
    ScopedPointer<HBox> hbox_filter_row_1, hbox_filter_row_2, hbox_filter_row_3,
    hbox_filter_row_3_spacer;
    ScopedPointer<Knob> fFilterEGDepth, fFilterEgAttack, fFilterEgDecay, fFilterEgSustain, fFilterEgRelease,
        fFilterCutOff, fFilterResonance;
    ScopedPointer<DropDown> fFilterLFOType;
    ScopedPointer<Menu> fFilterLFOTypeMenu;
    ScopedPointer<Knob> fFilterLFOFreq, fFilterLFODepth,fFilterLFOFade;
    // ScopedPointer<DropDown> fFilterLFOSync;
    // ScopedPointer<Menu> fFilterLFOSyncMenu;
    // ScopedPointer<RadioButton> fFilterLFOFreqBeat;

    // pitch tab
    ScopedPointer<VBox> vbox_pitch, vbox_pitch_lfo;
    ScopedPointer<HBox> hbox_pitch_row_1, hbox_pitch_row_2, hbox_pitch_row_3;
    ScopedPointer<Knob> fPitchEgDepth,
        fPitchEgAttack, fPitchEgDecay, fPitchEgSustain, fPitchEgRelease;
    ScopedPointer<DropDown> fPitchLFOType;
    ScopedPointer<Menu> fPitchLFOTypeMenu;
    ScopedPointer<Knob> fPitchLFOFreq, fPitchLFODepth, fPitchLFOFade;
    // // ScopedPointer<DropDown> fPitchLFOSync;
    // ScopedPointer<Menu> fPitchLFOSyncMenu;
    // ScopedPointer<RadioButton> fPitchLFOFreqBeat;

    ScopedPointer<SVGImage> dropsLogo, clearlyBrokenLogo, loopLeft, loopRight,
        zoomIn, zoomOut, zoomAll, zoomLoop;
    ScopedPointer<HBox> hbox_zoom_icons;
    ScopedPointer<SVGButton> fZoomOut, fZoomIn, fZoomAll, fZoomInOut,
        fFilterBandpass, fFilterLowpass, fFilterHighpass;
    ScopedPointer<PopUp> fPopUp;

    void initWidgets();
    void initTabAmp();
    void initTabPitch();
    void initTabFilter();
    void makeIcons();
    std::string dirnameOf(const std::string &fname);
    int loadSample();
    void drawWaveform();
    void drawMinimap();
    void drawLoopMarkers();
    void drawInOutMarkers();
    void scrollWaveform(bool leftright);
    void setMarkers();
    void setScrollbarWidgets();
    void zoomButtons(float zoom_delta);
    bool scrollbarDragging, loopstartDragging, loopendDragging, sampleInDragging, sampleOutDragging;
    bool sig_sampleLoaded;
    bool showWaveForm;
    std::string filepath;
    sf_count_t sampleLength;
    int sampleChannels;
    float scale;
    // sample
    sf_count_t sampleIn, sampleOut, sampleLoopStart, sampleLoopEnd;

    std::vector<signed char> *waveForm;
    std::vector<char> *miniMap;
    sf_count_t viewStart;
    sf_count_t viewEnd;
    float viewZoom;
    float viewMaxZoom;
    int mouseX;
    Rectangle<int> display;
    std::string sampleDir, fileName;
    FontId mainFont;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DropsUI)
};

END_NAMESPACE_DISTRHO

#endif