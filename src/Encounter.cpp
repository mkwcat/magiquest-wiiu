// Encounter.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Encounter.hpp"
#include "Page_Projector.hpp"
#include "System.hpp"

Encounter::Encounter()
{
}

Encounter::~Encounter()
{
}

u8 Encounter::GetMana(u8 side)
{
    if (side == 0) {
        return m_manaLeft;
    }

    if (side == 1) {
        return m_manaRight;
    }

    assert(!"Invalid mana side");
}

void Encounter::SetMana(u8 side, u8 value)
{
    auto page = System::GetPageStatic<Page_Projector>();
    assert(page != nullptr);

    page->ManaUpdate(side, value);

    if (side == 0) {
        m_manaLeft = value;
        return;
    }

    if (side == 1) {
        m_manaRight = value;
        return;
    }

    assert(!"Invalid mana side");
}

void Encounter::ForceNextMovie()
{
    auto page = System::GetPageStatic<Page_Projector>();
    assert(page != nullptr);

    page->EndMovie();
}
