#include "Page_TouchDuel.hpp"
#include "Ctrl_CommonButton.hpp"
#include "Page_Encounter_Dragon.hpp"
#include "Page_Encounter_Golem.hpp"
#include "Page_Encounter_HeroicDragon.hpp"
#include "Page_Encounter_IceDragon.hpp"
#include "Page_Encounter_SilverDragon.hpp"
#include "Page_Encounter_Xavier.hpp"
#include "Page_Projector.hpp"

Page_TouchDuel::Page_TouchDuel()
{
}

Page_TouchDuel::~Page_TouchDuel()
{
}

void Page_TouchDuel::Init()
{
    m_manaLeft.Update(Ctrl_Mana::Left, 0);
    m_manaRight.Update(Ctrl_Mana::Right, 0);

    m_manaLeft.setPosition(-(1920 / 2) + 40, -(1080 / 2) + 35);
    m_manaLeft.setAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
    m_manaLeft.setScaleX((1080 / m_manaLeft.getHeight()) * 0.85);
    m_manaLeft.setScaleY((1080 / m_manaLeft.getHeight()) * 0.85);

    m_manaRight.setPosition((1920 / 2) - 40, -(1080 / 2) + 35);
    m_manaRight.setAlignment(ALIGN_RIGHT | ALIGN_BOTTOM);
    m_manaRight.setScaleX((1080 / m_manaRight.getHeight()) * 0.85);
    m_manaRight.setScaleY((1080 / m_manaRight.getHeight()) * 0.85);

    append(&m_manaLeft);
    append(&m_manaRight);

    m_btnBack.InitDefault(Ctrl_CommonButton::Type::MiddleMenu);
    m_btnBack.setPosition(0, -(1080.0 / 2) + 120);
    m_btnBack.setAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
    m_btnBack.SetOnSelectGotoPage(System::GetPageID(this), System::PageID::SelectAdventureCategory);
    append(&m_btnBack);
}

void Page_TouchDuel::process()
{
    if (!m_initialized) {
        Init();
        m_initialized = true;
    }

    if (m_encounterFrame) {
        m_encounterFrame->process();
    }

    bool btnVisible = false;
    if (m_encounter) {
        auto page = System::GetPageStatic<Page_Projector>();
        assert(page != nullptr);

        m_manaLeft.Update(Ctrl_Mana::Left, m_encounter->GetMana(Ctrl_Mana::Left));
        m_manaRight.Update(Ctrl_Mana::Right, m_encounter->GetMana(Ctrl_Mana::Right));

        if (sys()->GetDisplay(System::GetPageID(this)) != System::Display::None) {
            auto* background = System::GetPageStatic<Page_Background>();
            assert(background != nullptr);
            background->SetImage(m_encounter->GetBackgroundType());
        }

        btnVisible = m_encounter->BackButtonAllowed();
    }

    m_btnBack.setVisible(btnVisible);
    m_btnBack.setClickable(btnVisible);

    GuiFrame::process();
}

void Page_TouchDuel::draw(CVideo* v)
{
    if (m_encounterFrame && m_encounter && m_encounter->UIVisible()) {
        m_encounterFrame->draw(v);
    }

    GuiFrame::draw(v);
}

void Page_TouchDuel::update(GuiController* controller)
{
    if (m_encounterFrame && m_encounter && m_encounter->UIVisible()) {
        m_encounterFrame->update(controller);
    }

    GuiFrame::update(controller);
}

void Page_TouchDuel::LoadEncounter(Encounter::Type type)
{
    if (m_nextEncounter) {
        delete m_nextEncounter;
        m_nextEncounter = nullptr;
    }

#define CASE_ENCOUNTER(_TYPE, _CLASS)                                                              \
    case Encounter::Type::_TYPE: {                                                                 \
        auto* obj = new _CLASS();                                                                  \
        m_nextEncounter = obj;                                                                     \
        m_nextEncounterFrame = obj;                                                                \
        LOG(LogSystem, "Loading encounter: %s", #_TYPE);                                           \
        break;                                                                                     \
    }

    switch (type) {
        CASE_ENCOUNTER(Dragon, Page_Encounter_Dragon)
        CASE_ENCOUNTER(IceDragon, Page_Encounter_IceDragon)
        CASE_ENCOUNTER(HeroicDragon, Page_Encounter_HeroicDragon)
        CASE_ENCOUNTER(SilverDragon, Page_Encounter_SilverDragon)
        CASE_ENCOUNTER(Golem, Page_Encounter_Golem)
        CASE_ENCOUNTER(Xavier, Page_Encounter_Xavier)
    default:
        PANIC("Unknown encounter type");
    }
#undef CASE_ENCOUNTER

    m_nextEncounterFrame->process();
}

void Page_TouchDuel::Transition()
{
    if (m_encounter) {
        delete m_encounter;
        m_encounter = nullptr;
        m_encounterFrame = nullptr;
    }

    if (m_nextEncounter) {
        m_encounter = m_nextEncounter;
        m_nextEncounter = nullptr;
        m_encounterFrame = m_nextEncounterFrame;
        m_nextEncounterFrame = nullptr;
    }

    if (m_encounter) {
        auto movie = System::GetPageStatic<Page_Projector>();
        assert(movie != nullptr);

        movie->SetEncounter(m_encounter);
        m_encounter->Transition();
    } else {
        LOG(LogSystem, "No encounter loaded for transition");
    }
}

void Page_TouchDuel::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    // Ignore if this page is not visible.
    if (!System::s_instance->GetSetting(System::GetPageID(this))->drc) {
        return;
    }

    if (m_encounter) {
        m_encounter->Cast(castMode, curValid, curX, curY, curZ);
    }
}