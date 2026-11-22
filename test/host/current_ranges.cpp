#include <assert.h>
#include <initializer_list>
#include <math.h>
#include <stdio.h>
#include "System.h"

static void near(float actual, float expected) {
    assert(fabsf(actual - expected) <= fmaxf(fabsf(expected) * 1e-6f, 1e-12f));
}
int main() {
    SystemClass system;
    const CurrentRange ranges[] = {CurrentRange::Range10uA, CurrentRange::Range1mA,
                                  CurrentRange::Range100mA, CurrentRange::Range1A};
    const float limits[] = {10e-6f, 1e-3f, 0.1f, 1.0f};
    for (int i = 0; i < 4; ++i) {
        system.Reset();
        system.SetCurrentRange(ranges[i]);
        near(system.GetCurrentSourceMax(), limits[i]);
        near(system.GetCurrentSourceMin(), -limits[i]);
        near(system.GetCurrentLimitMax(), 1.0f);
        near(system.GetCurrentLimitMin(), 0);
        system.SetCurrentSourceValue(-limits[i]);
        near(system.GetCurrentSourceValue(), -limits[i]);
        system.SetCurrentSourceValue(-limits[i] * 1.01f);
        near(system.GetCurrentSourceValue(), -limits[i]);
        system.SetCurrentSourceValue(limits[i]);
        system.SetCurrentSourceValue(limits[i] * 1.01f);
        near(system.GetCurrentSourceValue(), limits[i]);
        system.SetCurrentLimitValue(limits[i]);
        system.SetCurrentLimitValue(1.01f);
        system.SetCurrentLimitValue(-limits[i]);
        near(system.GetCurrentLimitValue(), limits[i]);
        assert(system.GetCurrentRange() == ranges[i]);

        system.Reset();
        system.SetCurrentRangeMode(RangeMode::Auto);
        system.SetCurrentSourceValue(-limits[i]);
        assert(system.GetCurrentRange() == ranges[i]);
        if (i < 3) {
            system.SetCurrentSourceValue(-limits[i] * 1.01f);
            assert(system.GetCurrentRange() == ranges[i + 1]);
        }
    }
    // A smaller measurement range must preserve compliance; source bounds remain separate.
    system.Reset();
    system.SetCurrentRange(CurrentRange::Range1A);
    system.SetCurrentSourceValue(0.2f);
    system.SetCurrentLimitValue(0.3f);
    system.SetCurrentRange(CurrentRange::Range10uA);
    near(system.GetCurrentSourceValue(), 0);
    near(system.GetCurrentLimitValue(), 0.3f);
    // AUTO respects all current values, and can shrink after they change.
    system.Reset();
    system.SetCurrentRangeMode(RangeMode::Auto);
    system.SetCurrentSourceValue(0.2f);
    system.SetCurrentLimitValue(1e-6f);
    system.SetCurrent(0.0f);
    assert(system.GetCurrentRange() == CurrentRange::Range1A);
    system.SetCurrentSourceValue(0.0f);
    assert(system.GetCurrentRange() == CurrentRange::Range10uA);
    system.SetCurrent(-0.002f);
    assert(system.GetCurrentRange() == CurrentRange::Range100mA);
    system.SetCurrent(0);
    assert(system.GetCurrentRange() == CurrentRange::Range10uA);
    system.SetCurrentSourceValue(1.01f);
    near(system.GetCurrentSourceValue(), 0);
    // Existing resistance excitation must fit its resolved current range.
    for (auto range : {ResistanceRange::Range10Ohm, ResistanceRange::Range100Ohm,
                       ResistanceRange::Range1kOhm, ResistanceRange::Range100kOhm}) {
        system.SetResistanceRange(range);
        assert(system.GetCurrentLimitValue() <= system.GetCurrentLimitMax());
    }
    puts("PASS: manual bounds, signed input, AUTO transitions, range reset, resistance presets");
}
