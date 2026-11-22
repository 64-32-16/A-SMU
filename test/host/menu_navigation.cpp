#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include "UI/Pages/MenuPage.h"
#include "Core/Beeper.h"

static std::vector<std::string> texts;
static uint16_t lastOptions;
static unsigned customDraws=0;
GDClass GD;
BeeperClass Beeper;
BeeperClass::BeeperClass() {}
void BeeperClass::Click() {}
void GDClass::ColorRGB(uint32_t) {}
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
static void Tap(MenuPage& page, Control* control)
{
    const int x = control->GetX() + control->GetWidth() / 2;
    const int y = control->GetY() + control->GetHeight() / 2;
    page.TouchStart(x, y); page.TouchEnd(x, y);
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
    assert(count == 16);
    Tap(page, page.GetChild(1)); assert(homeCalls == 17);
    puts("PASS: 16 routes, BACK/HOME, hidden groups, bounds, uniform sizes, press feedback, cancelled click");
}
