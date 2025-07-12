#pragma once

#include "Page_Encounter_Dragon.hpp"

class Page_Encounter_HeroicDragon : public Page_Encounter_Dragon
{
public:
    Page_Encounter_HeroicDragon() = default;

private:
    const char* FailClip(const char* clip);
    const char* NextFailClip(const char* clip);

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

    /**
     * Cast from WandHandler.
     */
    void Cast(Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ) override;

private:
    u32 m_protectionCount = 0;
    u32 m_freezeCount = 0;
    u32 m_revealCount = 0;
};