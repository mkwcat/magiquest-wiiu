#include "Page_Encounter_IceDragon.hpp"

const char* Page_Encounter_IceDragon::FailClip(const char* clip)
{
    SetMana(0, 0);
    m_nextPhase = Phase::End;
    m_buttons[u32(Spell::Protection)].SetSelectable(false);
    m_buttons[u32(Spell::Freeze)].SetSelectable(false);
    m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
    m_buttons[u32(Spell::Reveal)].SetSelectable(false);
    return clip;
}

const char* Page_Encounter_IceDragon::NextPhase()
{
    int damage;

    switch (m_currentPhase) {
    case Phase::End:
        m_nextPhase = Phase::Idle;
        SetMana(0, 0);
        SetMana(1, 0);
        m_imgMq.Select();
        for (u32 i = 0; i < SpellCount; i++) {
            m_buttons[i].SetSelectable(false);
        }
        // Fall through

    case Phase::Idle:
        // Dragon idle screen
        return nullptr;

    case Phase::Start:
        m_nextPhase = Phase::Fireball;
        m_magiHitCount = 0;
        m_dragonHitCount = 0;
        return "ID1";

    case Phase::Fireball:
    case Phase::ArrowFireball:
        if (!m_castProtection) {
            // Lose
            if (m_dragonHitCount == 0) {
                return FailClip("ID2");
            } else if (m_dragonHitCount == 1) {
                return FailClip("ID4D");
            } else {
                return FailClip("ID4H");
            }
        }

        damage = GetDamageTable(true, false)[std::min(m_magiHitCount++, 9u)];
        SetMana(0, std::max(int(GetMana(0)) - damage, 0));

        m_allowProtection = false;
        m_castProtection = false;

        m_nextPhase = Phase::TailSweep;
        m_allowReveal = true;
        if (m_dragonHitCount == 0) {
            return "ID3";
        } else if (m_dragonHitCount == 1) {
            return "ID8";
        } else {
            return "ID12";
        }

    case Phase::TailSweep:
        if (m_castReveal) {
            m_castReveal = false;
            m_allowReveal = false;
            m_allowFreeze = true;
            m_nextPhase = Phase::IceDragonReveal;
            if (m_dragonHitCount == 0) {
                return "ID5";
            } else if (m_dragonHitCount == 1) {
                return "ID9";
            } else {
                return "ID13";
            }
        } else {
            // Lose
            if (m_dragonHitCount == 0) {
                return FailClip("ID4A");
            } else if (m_dragonHitCount == 1) {
                return FailClip("ID4E");
            } else {
                return FailClip("ID4I");
            }
        }

    case Phase::IceDragonReveal:
        if (m_castFreeze) {
            m_castFreeze = false;
            m_allowFreeze = false;
            m_allowIceArrow = true;
            m_nextPhase = Phase::Freeze;

            if (m_dragonHitCount == 0) {
                return "ID6";
            } else if (m_dragonHitCount == 1) {
                return "ID10";
            } else {
                return "ID14";
            }
        } else {
            // Lose
            if (m_dragonHitCount == 0) {
                return FailClip("ID4B");
            } else if (m_dragonHitCount == 1) {
                return FailClip("ID4F");
            } else {
                return FailClip("ID4J");
            }
        }

    case Phase::Freeze:
        if (m_castIceArrow) {
            m_castIceArrow = false;
            m_allowIceArrow = false;
            m_dragonHitCount++;

            m_nextPhase = Phase::ArrowFireball;
            m_allowProtection = true;
            m_buttons[u32(Spell::Protection)].SetSelectable(true);
            m_buttons[u32(Spell::Freeze)].SetSelectable(true);
            m_buttons[u32(Spell::IceArrow)].SetSelectable(true);
            m_buttons[u32(Spell::Reveal)].SetSelectable(true);

            if (m_dragonHitCount == 1) {
                SetMana(1, 10);
                return "ID7";
            } else if (m_dragonHitCount == 2) {
                SetMana(1, 4);
                return "ID11";
            } else {
                SetMana(1, 0);
                m_nextPhase = Phase::End;
                m_buttons[u32(Spell::Protection)].SetSelectable(false);
                m_buttons[u32(Spell::Freeze)].SetSelectable(false);
                m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
                m_buttons[u32(Spell::Reveal)].SetSelectable(false);
                return "ID15";
            }
        } else {
            // Lose
            if (m_dragonHitCount == 0) {
                return FailClip("ID4C");
            } else if (m_dragonHitCount == 1) {
                return FailClip("ID4G");
            } else {
                return FailClip("ID4K");
            }
        }

    default:
        PANIC("Unknown phase");
    }
}

const char* Page_Encounter_IceDragon::NextMovie()
{
    m_currentPhase = m_nextPhase;

    auto name = NextPhase();

    if (name == nullptr) {
        // Default to the Dragon idle screen
        strcpy(m_phaseMoviePath, RES_ROOT "/Movie/Dragon/Dragon0001.mp4");
    } else {
        snprintf(
          m_phaseMoviePath, sizeof(m_phaseMoviePath), RES_ROOT "/Movie/IceDragon/%s.mp4", name);
    }

    return m_phaseMoviePath;
}