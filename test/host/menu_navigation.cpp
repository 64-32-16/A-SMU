#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include "UI/Pages/MenuPage.h"
#include "UI/Pages/InfoPage.h"
#include "UI/Pages/SourceSettingsPage.h"
#include "UI/Panels/SourceModePopup.h"
#include "UI/Panels/NumberPadPopup.h"
#include "UI/Panels/OutputOffStatePopup.h"
#include "UI/Panels/SourceRangePopup.h"
#include "UI/Pages/MeasureSettingsPage.h"
#include "UI/Pages/SystemSettingsPage.h"
#include "Core/Beeper.h"
#include "UI/Core/Theme.h"

static std::vector<std::string> texts;
static uint16_t lastOptions;
static unsigned customDraws=0;
GDClass GD;
BeeperClass Beeper;
BeeperClass::BeeperClass() {}
void BeeperClass::Click() {}
void BeeperClass::Error() {}
void GDClass::ColorRGB(uint32_t) {}
void GDClass::ColorA(byte) {}
void GDClass::DrawSlider(int16_t,int16_t,int16_t,int16_t,unsigned,uint32_t,uint32_t,uint32_t) {}
void GDClass::RequestBackdropCapture() {}
bool GDClass::DrawCapturedBackdrop() {return true;}
void GDClass::DrawModalWindow(int16_t,int16_t,int16_t,int16_t,int16_t,int16_t,int16_t,int16_t,uint32_t,uint32_t,uint32_t) {}
void GDClass::Begin(Primitive) {}
void GDClass::LineWidth(uint16_t) {}
void GDClass::Vertex2f(int16_t, int16_t) {}
void GDClass::cmd_fgcolor(uint32_t) {}
void GDClass::cmd_text(int16_t, int16_t, byte, uint16_t, const char* s) { texts.push_back(s); }
void GDClass::cmd_button(int16_t, int16_t, uint16_t, uint16_t, byte, uint16_t options, const char* s) { lastOptions=options;texts.push_back(s); }
void GDClass::DrawSolidRoundedRect(int16_t,int16_t,int16_t,int16_t,uint16_t,uint32_t) {}
void GDClass::DrawRoundedGradientRect(int16_t, int16_t, int16_t, int16_t, uint16_t, uint32_t top, uint32_t bottom)
{ ++customDraws; }

static bool Has(const char* text) { return std::find(texts.begin(), texts.end(), text) != texts.end(); }
static void Draw(MenuPage& page) { texts.clear(); page.Draw(); }
static void Tap(Container& page, Control* control)
{
    const int x = control->GetX() + control->GetWidth() / 2;
    const int y = control->GetY() + control->GetHeight() / 2;
    page.TouchStart(x, y); page.TouchEnd(x, y);
}
static SourceRangePopup* sharedRange=nullptr;
static void OpenRange(SourceMode mode) {sharedRange->OpenForMode(mode);}
static SourceModePopup* sharedMode=nullptr;
static NumberPadPopup* sharedNumber=nullptr;
static OutputOffStatePopup* sharedOff=nullptr;
static SystemClass* sourceSystem=nullptr;
static void EditSource(SourceSettingsPage::Setting setting) {
    switch(setting) {
        case SourceSettingsPage::Setting::Function: sharedMode->Open();break;
        case SourceSettingsPage::Setting::Range: sharedRange->OpenForMode(sourceSystem->GetSourceMode());break;
        case SourceSettingsPage::Setting::Limit: sharedNumber->Open(sourceSystem->GetSourceMode(),NumberPadTargetType::LimitValue);break;
        case SourceSettingsPage::Setting::OutputOff: sharedOff->Open();break;
    }
}
static unsigned homeCalls = 0;
static void Home(Button*) { ++homeCalls; }

int main()
{
    MenuPage page(0, 0, 800, 480);
    page.SetOnHome(Home);
    assert(!page.IsVisible());
    page.Open(); Draw(page);
    assert(Has("SOURCE") && Has("MEASURE") && Has("VIEWS") && Has("SYSTEM"));
    assert(!Has("TRIGGER") && !Has("SCRIPTS"));
    const char* groupTitles[] = {"SOURCE", "MEASURE", "VIEWS", "TRIGGER", "SCRIPTS", "SYSTEM"};
    unsigned count = 0;
    for (uint8_t g = 0; g < 6; ++g)
        for (uint8_t i = 0; i < 5; ++i)
        {
            auto* item = static_cast<Button*>(page.GetChild(2 + g * 5 + i));
            if (!item->IsVisible()) continue;
            ++count;
            assert(item->GetWidth() == 186 && item->GetHeight() == 44);
            assert(item->GetX() >= 0 && item->GetX() + item->GetWidth() <= 800);
            assert(item->GetY() >= 0 && item->GetY() + item->GetHeight() <= 480);
            const std::string title = std::string(groupTitles[g]) + " - " + item->GetText();
            item->Draw(); assert((lastOptions & OPT_FLAT)==0 && customDraws==0);
            page.TouchStart(item->GetX() + 10, item->GetY() + 10);
            item->Draw(); assert((lastOptions & OPT_FLAT)!=0 && customDraws==0);
            page.TouchEnd(-1, -1); Draw(page); assert(Has("MENU"));
            Tap(page, item); Draw(page); assert(Has(title.c_str()));
            assert(page.GetChild(0)->IsVisible());
            Tap(page, page.GetChild(1)); assert(homeCalls == count);
            Tap(page, page.GetChild(0)); Draw(page); assert(Has("MENU"));
        }
    assert(count == 12);
    Tap(page, page.GetChild(1)); assert(homeCalls == count+1);
    SystemClass system;system.SetSimulationEnabled(true);
    MeasureSettingsPage settings(0,0,800,480);settings.SetSystem(&system);
    settings.SetOnHome(Home);settings.SetOnBack(Home);settings.Open();
    SourceRangePopup range(0,0,800,480);range.SetSystem(&system);sharedRange=&range;settings.SetOnRange(OpenRange);
    Container settingsRoot(0,0,800,480);settingsRoot.AddChild(&settings);settingsRoot.AddChild(&range);
    const bool output=system.GetOutputOn();
    for(uint8_t i=0;i<MeasurementTiming::Count;++i) {
        Tap(settings,settings.GetChild(2+MeasurementTiming::Count));
        auto* choice=static_cast<Button*>(settings.GetChild(2+i));
        settings.TouchStart(choice->GetX()+8,choice->GetY()+8);
        settings.TouchEnd(-1,-1);assert(system.GetNplcProfile()==(i?i-1:MeasurementTiming::Default));
        texts.clear();settings.Draw();assert(Has("SELECT NPLC"));
        assert(!settings.GetChild(0)->IsVisible());
        assert(choice->GetX()+choice->GetWidth()<=700 && choice->GetY()+choice->GetHeight()<350);
        Tap(settings,choice);assert(system.GetNplcProfile()==i);
        texts.clear();settings.Draw();
        assert(Has("One measurement = one U/I pair, measured in sequence."));
        assert(!choice->IsVisible());
        assert(choice->GetX()+choice->GetWidth()<=800 && choice->GetY()+choice->GetHeight()<350);
        assert(system.GetOutputOn()==output);
    }
    const unsigned valueBase=2+MeasurementTiming::Count;
    Tap(settings,settings.GetChild(valueBase));
    Tap(settings,settings.GetChild(valueBase+3)); // Cancel NPLC
    assert(system.GetNplcProfile()==MeasurementTiming::Count-1);
    Tap(settingsRoot,settings.GetChild(valueBase+1)); // shared voltage dialog
    assert(range.IsOpen());texts.clear();range.Draw();assert(Has("SELECT VOLTAGE RANGE"));
    const auto callsBefore=homeCalls;
    Tap(settingsRoot,settings.GetChild(1));assert(homeCalls==callsBefore && range.IsOpen()); // modal blocks HOME
    Tap(settingsRoot,range.GetChild(1));assert(!range.IsOpen() && system.GetVoltageRangeMode()==RangeMode::Auto);
    Tap(settingsRoot,settings.GetChild(valueBase+1));
    Tap(settingsRoot,range.GetChild(3));assert(system.GetVoltageRange()==VoltageRange::Range30V);
    assert(system.GetVoltageRangeMode()==RangeMode::Manual);
    Tap(settingsRoot,settings.GetChild(valueBase+2));
    texts.clear();range.Draw();assert(Has("SELECT CURRENT RANGE"));
    Tap(settingsRoot,range.GetChild(2));assert(system.GetCurrentRange()==CurrentRange::Range10uA);
    assert(system.GetCurrentRangeMode()==RangeMode::Manual);
    Tap(settingsRoot,settings.GetChild(valueBase+2));
    Tap(settingsRoot,range.GetChild(1));assert(system.GetCurrentRangeMode()==RangeMode::Auto);
    Tap(settingsRoot,settings.GetChild(valueBase+2));
    Tap(settingsRoot,range.GetChild(2+SystemClass::CurrentRangeCount));
    assert(!range.IsOpen() && system.GetCurrentRangeMode()==RangeMode::Auto); // Cancel preserves settings
    assert(system.GetOutputOn()==output);
    settings.SetVisible(false);settings.Open();assert(system.GetNplcProfile()==MeasurementTiming::Count-1);
    Tap(settings,settings.GetChild(0));Tap(settings,settings.GetChild(1));assert(homeCalls==count+3);
    page.SetOnMeasureSettings(Home);page.Open();Tap(page,page.GetChild(7));assert(homeCalls==count+4);
    SystemSettingsPage preferences(0,0,800,480);preferences.SetSystem(&system);preferences.Open();
    Tap(preferences,preferences.GetChild(2));assert(!system.GetUserInterfaceSettings().keyClick);
    Tap(preferences,preferences.GetChild(2));assert(system.GetUserInterfaceSettings().keyClick);
    Tap(preferences,preferences.GetChild(3));assert(!preferences.GetChild(0)->IsVisible());
    preferences.TouchStart(160,229);preferences.TouchMove(0,229);preferences.TouchEnd(0,229);
    assert(system.GetUserInterfaceSettings().backlightPercent==UserInterfaceSettings::MinBacklight);
    assert(!preferences.GetChild(5)->IsEnabled());
    preferences.TouchStart(400,229);preferences.TouchMove(800,229);preferences.TouchEnd(800,229);
    assert(system.GetUserInterfaceSettings().backlightPercent==UserInterfaceSettings::MaxBacklight);
    assert(!preferences.GetChild(6)->IsEnabled());
    Tap(preferences,preferences.GetChild(5));assert(system.GetUserInterfaceSettings().backlightPercent==99);
    texts.clear();preferences.Draw();assert(Has("BACKLIGHT") && !Has("SYSTEM SETTINGS"));
    Tap(preferences,preferences.GetChild(7));assert(preferences.GetChild(0)->IsVisible());
    Tap(preferences,preferences.GetChild(4));
    auto* number=static_cast<Panel*>(preferences.GetChild(8));assert(number->IsVisible());
    auto numberKey=[&](unsigned key){Tap(preferences,number->GetChild(key));};
    numberKey(14);numberKey(11);numberKey(1);numberKey(15);assert(number->IsVisible()); // negative rejected
    numberKey(14);numberKey(1);numberKey(10);numberKey(5);numberKey(15);assert(number->IsVisible()); // fractional rejected
    numberKey(14);numberKey(0);numberKey(15);assert(!number->IsVisible() && system.GetUserInterfaceSettings().dimmerMinutes==0);
    Tap(preferences,preferences.GetChild(4));numberKey(14);numberKey(3);numberKey(0);numberKey(16);
    assert(system.GetUserInterfaceSettings().dimmerMinutes==0); // Cancel
    Tap(preferences,preferences.GetChild(4));numberKey(14);numberKey(3);numberKey(0);numberKey(15);
    assert(system.GetUserInterfaceSettings().dimmerMinutes==30);
    page.SetOnSystemSettings(Home);page.Open();Tap(page,page.GetChild(2+5*5+2));assert(homeCalls==count+5);
    page.SetOnInfo(Home);page.Open();Tap(page,page.GetChild(2+5*5+4));assert(homeCalls==count+6);
    InfoPage info(0,0,800,480);info.SetOnBack(Home);info.SetOnHome(Home);info.Open();
    texts.clear();info.Draw();
    assert(Has("INFO") && Has("A-SMU") && Has("Hobby SMU") && Has("Firmware build"));
    assert(Has("+/- 5 V") && Has("+/- 30 V") && Has("+/- 10 uA") && Has("+/- 1 mA") && Has("+/- 100 mA") && Has("+/- 1 A"));
    Tap(info,info.GetChild(0));Tap(info,info.GetChild(1));assert(homeCalls==count+8);
    page.SetOnSourceSettings(Home);page.Open();Tap(page,page.GetChild(2));assert(homeCalls==count+9);
    SourceSettingsPage source(0,0,800,480);source.SetSystem(&system);source.SetOnSetting(EditSource);
    source.SetOnBack(Home);source.SetOnHome(Home);source.Open();sourceSystem=&system;
    SourceModePopup mode(0,0,800,480);mode.SetSystem(&system);sharedMode=&mode;
    NumberPadPopup limit(0,0,800,480);limit.SetSystem(&system);sharedNumber=&limit;
    OutputOffStatePopup off(0,0,800,480);off.SetSystem(&system);sharedOff=&off;
    Container sourceRoot(0,0,800,480);sourceRoot.AddChild(&source);sourceRoot.AddChild(&range);
    sourceRoot.AddChild(&mode);sourceRoot.AddChild(&limit);sourceRoot.AddChild(&off);
    system.SetSourceMode(SourceMode::Voltage);
    texts.clear();source.Draw();assert(Has("SOURCE SETTINGS") && Has("Current Limit") && Has("Voltage"));
    const auto sourceOutput=system.GetOutputOn();
    Tap(sourceRoot,source.GetChild(2));assert(mode.IsOpen());
    Tap(sourceRoot,source.GetChild(1));assert(homeCalls==count+9 && mode.IsOpen());
    Tap(sourceRoot,mode.GetChild(2));assert(system.GetSourceMode()==SourceMode::Current);
    texts.clear();source.Draw();assert(Has("Voltage Limit") && Has("Current"));
    Tap(sourceRoot,source.GetChild(3));assert(range.IsOpen());
    Tap(sourceRoot,range.GetChild(2));assert(system.GetCurrentRange()==CurrentRange::Range10uA);
    source.Draw();assert(std::string(static_cast<Button*>(source.GetChild(3))->GetText())=="10 uA");
    Tap(sourceRoot,source.GetChild(4));assert(limit.IsOpen());
    Tap(sourceRoot,limit.GetChild(15));Tap(sourceRoot,limit.GetChild(1));Tap(sourceRoot,limit.GetChild(18));
    Tap(sourceRoot,limit.GetChild(16));assert(!limit.IsOpen() && system.GetVoltageLimitValue()==1.0f);
    Tap(sourceRoot,source.GetChild(4));Tap(sourceRoot,limit.GetChild(15));Tap(sourceRoot,limit.GetChild(2));
    Tap(sourceRoot,limit.GetChild(17));assert(system.GetVoltageLimitValue()==1.0f); // Cancel
    Tap(sourceRoot,source.GetChild(2));Tap(sourceRoot,mode.GetChild(1));
    assert(system.GetSourceMode()==SourceMode::Voltage);
    Tap(sourceRoot,source.GetChild(3));Tap(sourceRoot,range.GetChild(1));assert(system.GetVoltageRangeMode()==RangeMode::Auto);
    system.SetCurrentRange(CurrentRange::Range100mA);
    Tap(sourceRoot,source.GetChild(4));assert(limit.IsOpen());
    Tap(sourceRoot,limit.GetChild(15));Tap(sourceRoot,limit.GetChild(1));Tap(sourceRoot,limit.GetChild(19));
    Tap(sourceRoot,limit.GetChild(16));assert(!limit.IsOpen() && system.GetCurrentLimitValue()>0.00099f && system.GetCurrentLimitValue()<0.00101f);
    assert(system.GetVoltageLimitValue()==1.0f); // correct opposite quantity for each mode
    const auto previousOff=system.GetOutputOffState();
    Tap(sourceRoot,source.GetChild(5));assert(off.IsOpen());Tap(sourceRoot,off.GetChild(4));assert(system.GetOutputOffState()==previousOff);
    Tap(sourceRoot,source.GetChild(5));Tap(sourceRoot,off.GetChild(3));
    assert(!off.IsOpen());source.Draw();assert(std::string(static_cast<Button*>(source.GetChild(5))->GetText())==system.GetOutputOffStateText());
    system.SetVoltageRangeMode(RangeMode::Manual);system.SetVoltageRange(VoltageRange::Range5V);
    source.Draw();assert(std::string(static_cast<Button*>(source.GetChild(3))->GetText())=="5 V"); // external changes reflected
    assert(system.GetOutputOn()==sourceOutput);
    Tap(sourceRoot,source.GetChild(0));Tap(sourceRoot,source.GetChild(1));assert(homeCalls==count+11);
    puts("PASS: 12 routes, BACK/HOME, hidden groups, bounds, uniform sizes, press feedback, cancelled click");
}
