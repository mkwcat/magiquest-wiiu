#pragma once

#include "Ctrl_CommonButton.hpp"
#include "Ctrl_Mana.hpp"
#include "Encounter.hpp"
#include "WandHandler.hpp"
#include <gui/GuiFrame.h>

class Page_TouchDuel : public GuiFrame, public WandHandler
{
public:
    enum class EncounterType {
        Dragon,
        SilverDragon,
        Golem,
        Xavier,
    };

    Page_TouchDuel();
    ~Page_TouchDuel() override;

    void process() override;
    void draw(CVideo* v) override;
    void update(GuiController* controller) override;
    void LoadEncounter(EncounterType type);
    void Transition();
    void Cast(Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ) override;

private:
    void Init();

    bool m_initialized = false;
    Encounter* m_encounter = nullptr;
    GuiFrame* m_encounterFrame = nullptr;
    Encounter* m_nextEncounter = nullptr;
    GuiFrame* m_nextEncounterFrame = nullptr;
    Ctrl_Mana m_manaLeft;
    Ctrl_Mana m_manaRight;
    Ctrl_CommonButton m_btnBack;
};