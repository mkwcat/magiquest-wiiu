#include "Ctrl_Mana.hpp"
#include "Page_Projector.hpp"
#include "ResourceManager.hpp"
#include "System.hpp"

void Ctrl_Mana::Update(u8 side, u8 mana)
{
    assert(side == Left || side == Right);

    static const float posVtx[4 * 3] = {
      -1.0f, -1.0f, 0.0f, // Bottom left
      1.0f, -1.0f, 0.0f, // Bottom right
      1.0f, 1.0f, 0.0f, // Top right
      -1.0f, 1.0f, 0.0f, // Top left
    };

    static const float texCoordsLeft[8] = {
      0.0f, 1.0f, // Bottom left
      1.0f, 1.0f, // Bottom right
      1.0f, 0.0f, // Top right
      0.0f, 0.0f, // Top left
    };

    static const float texCoordsRight[8] = {
      1.0f, 1.0f, // Bottom left
      0.0f, 1.0f, // Bottom right
      0.0f, 0.0f, // Top right
      1.0f, 0.0f, // Top left
    };

    setPrimitiveVertex(GX2_PRIMITIVE_MODE_QUADS, posVtx,
      side == Left ? texCoordsLeft : texCoordsRight, 4);

    setImageData(ResourceManager::GetManaImage(mana));
}
