#include "Page_Encounter_HeroicDragon.hpp"
#include "Config.hpp"

const char* Page_Encounter_HeroicDragon::FailClip(const char* clip)
{
    SetMana(0, 0);
    m_nextPhase = Phase::End;
    m_buttons[u32(Spell::Protection)].SetSelectable(false);
    m_buttons[u32(Spell::Freeze)].SetSelectable(false);
    m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
    m_buttons[u32(Spell::Reveal)].SetSelectable(false);
    return clip;
}

const char* Page_Encounter_HeroicDragon::NextFailClip(const char* clip)
{
    m_nextPhase = Phase::HeroicFail;
    m_buttons[u32(Spell::Protection)].SetSelectable(false);
    m_buttons[u32(Spell::Freeze)].SetSelectable(false);
    m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
    m_buttons[u32(Spell::Reveal)].SetSelectable(false);
    return clip;
}

const char* Page_Encounter_HeroicDragon::NextPhase()
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
        m_nextPhase = Phase::HeroicExpectProtection;
        m_magiHitCount = 0;
        m_dragonHitCount = 0;
        m_protectionCount = 0;
        m_freezeCount = 0;
        m_revealCount = 0;
        return "Dragon/Dragon0002";

    case Phase::HeroicExpectProtection:
        if (m_castProtection) {
            // Red Dragon
            damage = GetDamageTable(true, false)[std::min(m_magiHitCount++, 9u)];
            SetMana(0, GetMana(0) - damage);
            m_allowProtection = false;
            m_castProtection = false;
            m_protectionCount++;

            if (m_protectionCount == 1) {
                m_nextPhase = Phase::HeroicExpectFreeze;
                m_allowFreeze = true;
                return "Dragon/Dragon0003";
            } else if (m_protectionCount == 2) {
                // 2: Switch to Red Dragon on 2nd protection
                m_nextPhase = Phase::HeroicExpectFreeze;
                m_allowFreeze = true;
                return "HeroicDragon/AdvHeroicDragonID8D12A";
            } else if (m_protectionCount == 3 && Config::EnableUnusedClips) {
                // 5: Switch to Ice Dragon and 6: switch back on 4th ice arrow
                m_nextPhase = Phase::HeroicExpectProtection;
                m_allowProtection = true;
                return "HeroicDragon/AdvHeroicDragonD15AID12D15REV1";
            } else {
                // Inaccuracy: Cannot switch to Ice Dragon here due to corrupted clip later on,
                // so Charlock must be frozen first
                m_nextPhase = Phase::HeroicExpectFreeze;
                m_allowFreeze = true;
                return "Dragon/Dragon0015A";
            }
        } else {
            if (m_protectionCount == 1) {
                return FailClip("IceDragon/ID4D");
            } else {
                return FailClip("Dragon/Dragon0010A");
            }
        }

    case Phase::HeroicExpectReveal:
        if (m_castReveal) {
            // Ice Dragon
            m_castReveal = false;
            m_allowReveal = false;
            m_revealCount++;

            if (m_revealCount == 1) {
                m_nextPhase = Phase::HeroicExpectFreeze;
                m_allowFreeze = true;
                return "IceDragon/ID5";
            } else if (m_revealCount == 2) {
                m_nextPhase = Phase::HeroicExpectFreeze;
                m_allowFreeze = true;
                return "IceDragon/ID9";
            } else if (m_revealCount == 3) {
                // 8: Switch to Red Dragon on 3rd reveal
                m_nextPhase = Phase::HeroicExpectIceArrow;
                m_allowIceArrow = true;
                return "HeroicDragon/AdvHeroicDragonID13D17";
            }
        } else {
            if (m_revealCount == 0) {
                return FailClip("IceDragon/ID4A");
            } else if (m_revealCount == 1) {
                return FailClip("IceDragon/ID4E");
            } else {
                return FailClip("IceDragon/ID4I");
            }
        }

    case Phase::HeroicExpectFreeze:
        if (m_castFreeze) {
            m_castFreeze = false;
            m_allowFreeze = false;
            m_freezeCount++;

            if (m_freezeCount == 1) {
                // Red Dragon: Freeze 1
                m_nextPhase = Phase::HeroicExpectIceArrow;
                m_allowIceArrow = true;
                return "Dragon/Dragon0011";
            } else if (m_freezeCount == 2) {
                // Ice Dragon: Freeze 1
                m_nextPhase = Phase::HeroicExpectIceArrow;
                m_allowIceArrow = true;
                return "IceDragon/ID6";
            } else if (m_freezeCount == 3) {
                // Red Dragon: Freeze 2
                // 3: Switch to Ice Dragon on 3rd freeze
                m_nextPhase = Phase::HeroicExpectReveal;
                m_allowReveal = true;
                return "HeroicDragon/AdvHeroicDragonD14ID8";
            } else if (m_freezeCount == 4) {
                // Ice Dragon: Freeze 2
                m_nextPhase = Phase::HeroicExpectIceArrow;
                m_allowIceArrow = true;
                return "IceDragon/ID10";
            } else if (!Config::EnableUnusedClips || m_freezeCount == 5) {
                // Red Dragon: Freeze 3
                // 7: Switch to Ice Dragon on 5th freeze
                m_nextPhase = Phase::HeroicExpectReveal;
                m_allowReveal = true;
                return "HeroicDragon/AdvHeroicDragonD17ID12";
            } else if (m_freezeCount == 6) {
                // Ice Dragon: Freeze 3 (custom)
                m_nextPhase = Phase::HeroicExpectIceArrow;
                m_allowIceArrow = true;
                return "IceDragon/ID14";
            }
        } else {
            // Lose
            if (m_freezeCount == 0) {
                return NextFailClip("Dragon/Dragon0004");
            } else if (m_freezeCount == 1) {
                return FailClip("IceDragon/ID4B");
            } else if (m_freezeCount == 2) {
                return NextFailClip("Dragon/Dragon0013");
            } else if (m_freezeCount == 3) {
                return FailClip("IceDragon/ID4F");
            } else if (m_freezeCount == 4) {
                return NextFailClip("Dragon/Dragon0016");
            } else {
                return FailClip("IceDragon/ID4J");
            }
        }

    case Phase::HeroicExpectIceArrow:
        if (m_castIceArrow) {
            m_castIceArrow = false;
            m_allowIceArrow = false;
            m_dragonHitCount++;

            if (m_dragonHitCount == 1) {
                // Red Dragon: Ice Arrow 1
                // 1: Switch to Ice Dragon on 1st ice arrow
                SetMana(1, Config::EnableUnusedClips ? GetMana(1) - 3 : 14);
                m_nextPhase = Phase::HeroicExpectReveal;
                m_allowReveal = true;
                return "HeroicDragon/AdvHeroicDragonD12ID3";
            } else if (m_dragonHitCount == 2) {
                // Ice Dragon: Ice Arrow 1
                SetMana(1, Config::EnableUnusedClips ? GetMana(1) - 3 : 12);
                m_nextPhase = Phase::HeroicExpectProtection;
                m_allowProtection = true;
                return "IceDragon/ID7";
            } else if (m_dragonHitCount == 3) {
                // Ice Dragon: Ice Arrow 2
                // 4: Switch to Red Dragon on 3rd ice arrow
                SetMana(1, Config::EnableUnusedClips ? GetMana(1) - 3 : 8);
                m_nextPhase = Phase::HeroicExpectIceArrow;
                m_allowIceArrow = true;
                return "HeroicDragon/AdvHeroicDragonID11D14";
            } else if (m_dragonHitCount == 4) {
                if (!Config::EnableUnusedClips) {
                    // Red Dragon: Ice Arrow 2
                    // 5: Switch to Ice Dragon and 6: switch back on 4th ice arrow
                    SetMana(1, 4);
                    m_nextPhase = Phase::HeroicExpectProtection;
                    m_allowProtection = true;
                    return "HeroicDragon/AdvHeroicDragonD15AID12D15";
                } else {
                    // Use the original Dragon clip for this and delay the switch by one phase
                    SetMana(1, GetMana(1) - 3);
                    m_nextPhase = Phase::HeroicExpectProtection;
                    m_allowProtection = true;
                    return "Dragon/Dragon0015";
                }
            } else if (!Config::EnableUnusedClips) {
                // Red Dragon: Ice Arrow 3
                SetMana(1, 0);
                m_nextPhase = Phase::End;
                m_buttons[u32(Spell::Protection)].SetSelectable(false);
                m_buttons[u32(Spell::Freeze)].SetSelectable(false);
                m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
                m_buttons[u32(Spell::Reveal)].SetSelectable(false);
                return "HeroicDragon/AdvHeroicDragonWin";
            } else if (m_dragonHitCount == 5) {
                // Red Dragon: Ice Arrow 3
                // 9: Switch to Ice Dragon on 5th ice arrow (custom, using unused clips)
                SetMana(1, GetMana(1) - 3);
                m_nextPhase = Phase::HeroicExpectFreeze;
                m_allowFreeze = true;
                return "HeroicDragon/AdvHeroicDragonD20ID13";
            } else if (m_dragonHitCount == 6) {
                // Ice Dragon: Ice Arrow 3 (custom edited clip)
                SetMana(1, 0);
                m_nextPhase = Phase::End;
                m_buttons[u32(Spell::Protection)].SetSelectable(false);
                m_buttons[u32(Spell::Freeze)].SetSelectable(false);
                m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
                m_buttons[u32(Spell::Reveal)].SetSelectable(false);
                return "HeroicDragon/AdvHeroicDragonID15D20Custom";
            }
        } else {
            // Lose
            if (m_dragonHitCount == 0) {
                return NextFailClip("Dragon/Dragon0004");
            } else if (m_dragonHitCount == 1) {
                return FailClip("IceDragon/ID4C");
            } else if (m_dragonHitCount == 2) {
                return NextFailClip("IceDragon/ID4G");
            } else if (m_dragonHitCount == 3) {
                return FailClip("Dragon/Dragon0014");
            } else if (!Config::EnableUnusedClips || m_dragonHitCount == 4) {
                return NextFailClip("Dragon/Dragon0016");
            } else {
                return FailClip("IceDragon/ID4K");
            }
        }

    case Phase::HeroicFail:
        SetMana(0, 0);
        m_nextPhase = Phase::End;
        return "Dragon/Dragon0010A";

    default:
        PANIC("Unknown phase");
    }
}

const char* Page_Encounter_HeroicDragon::NextMovie()
{
    if (m_currentPhase != Phase::Idle &&
        (m_currentPhase != Phase::Start || m_nextPhase == Phase::HeroicExpectProtection) &&
        m_currentPhase != Phase::HeroicFail && m_currentPhase != Phase::End) {
        m_buttons[u32(Spell::Protection)].SetSelectable(true);
        m_buttons[u32(Spell::Freeze)].SetSelectable(true);
        m_buttons[u32(Spell::IceArrow)].SetSelectable(true);
        m_buttons[u32(Spell::Reveal)].SetSelectable(true);
    }

    m_currentPhase = m_nextPhase;

    auto name = NextPhase();

    if (name == nullptr) {
        // Default to the Dragon idle screen
        strcpy(m_phaseMoviePath, RES_ROOT "/Movie/Dragon/Dragon0001.mp4");
    } else {
        snprintf(m_phaseMoviePath, sizeof(m_phaseMoviePath), RES_ROOT "/Movie/%s.mp4", name);
    }

    return m_phaseMoviePath;
}

void Page_Encounter_HeroicDragon::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    LOG(LogSystem, "Cast");

    if (m_nextPhase == Phase::Idle) {
        SetMana(0, 16);
        SetMana(1, 16);
        DeselectAll();
        m_imgMq.Deselect();
        m_nextPhase = Phase::Start;

        // Prepare input for the next phase immediately
        m_allowFreeze = false;
        m_allowIceArrow = false;
        m_allowProtection = true;
        m_allowReveal = false;
        m_buttons[u32(Spell::Protection)].SetSelectable(true);
        m_buttons[u32(Spell::Freeze)].SetSelectable(true);
        m_buttons[u32(Spell::IceArrow)].SetSelectable(true);
        m_buttons[u32(Spell::Reveal)].SetSelectable(true);
        m_castProtection = false;
        m_castFreeze = false;
        m_castIceArrow = false;
        m_castReveal = false;
        return;
    }

    if (castMode == Wand::CastMode::WiiRemoteCastRune && curValid &&
        (curX < 640 && curX > -640 && curY < 450 && curY > -450)) {
        for (u32 i = 0; i < SpellCount; i++) {
            if (!m_buttons[i].IsSelectable()) {
                continue;
            }

            auto x = m_buttons[i].getCenterX();
            auto y = m_buttons[i].getCenterY();
            if (curX > x - 240 && curX < x + 240 && curY > y - 240 && curY < y + 240) {
                DeselectAll();
                m_buttons[i].Select();
            }
        }
        return;
    }

    Spell nowSpell = Spell::None;

    if (m_buttons[u32(Spell::Protection)].IsSelected()) {
        m_castProtection = true;
        m_buttons[u32(Spell::Protection)].SetSelectable(false);
        nowSpell = Spell::Protection;
    } else if (m_buttons[u32(Spell::Freeze)].IsSelected()) {
        m_castFreeze = true;
        m_buttons[u32(Spell::Freeze)].SetSelectable(false);
        nowSpell = Spell::Freeze;
    } else if (m_buttons[u32(Spell::IceArrow)].IsSelected()) {
        m_castIceArrow = true;
        m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
        nowSpell = Spell::IceArrow;
    } else if (m_buttons[u32(Spell::Reveal)].IsSelected()) {
        m_castReveal = true;
        m_buttons[u32(Spell::Reveal)].SetSelectable(false);
        nowSpell = Spell::Reveal;
    } else {
        // No spell selected
        return;
    }

    // Uncast previous selected spells
    if (m_castProtection && nowSpell != Spell::Protection) {
        m_castProtection = false;
        m_buttons[u32(Spell::Protection)].SetSelectable(true);
    }
    if (m_castFreeze && nowSpell != Spell::Freeze) {
        m_castFreeze = false;
        m_buttons[u32(Spell::Freeze)].SetSelectable(true);
    }
    if (m_castIceArrow && nowSpell != Spell::IceArrow) {
        m_castIceArrow = false;
        m_buttons[u32(Spell::IceArrow)].SetSelectable(true);
    }
    if (m_castReveal && nowSpell != Spell::Reveal) {
        m_castReveal = false;
        m_buttons[u32(Spell::Reveal)].SetSelectable(true);
    }

    DeselectAll();
}