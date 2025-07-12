#pragma once

#include "Page_Encounter_Dragon.hpp"

class Page_Encounter_IceDragon : public Page_Encounter_Dragon
{
public:
    Page_Encounter_IceDragon() = default;

private:
    const char* FailClip(const char* clip);

    const char* NextPhase();

public:
    /**
     * NextMovie from Encounter.
     */
    const char* NextMovie() override;

    /**
     * NextFrame from Encounter.
     */
    void NextFrame(u32 frame) override
    {
    }

    /**
     * GetEncounterType from Encounter.
     */
    Encounter::Type GetEncounterType() const override
    {
        return Encounter::Type::IceDragon;
    }
};