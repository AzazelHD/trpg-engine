#include "engine/renderer/Camera.h"
#include "engine/renderer/TileLayer.h"
#include "engine/renderer/SpriteBatch.h"

// Checklist — mark with [x] once done:
//
// [x] Implement constructor and store config/tileset pointer.
// [x] Implement setTiles(mapWidth, mapHeight, tileIndices).
// [x] Implement render(batch, camera, screenSize):
//       - Iterate in camera-provided back-to-front draw order.
//       - Compute src rect from tile index and tilesPerRow.
//       - Use camera.tileToScreen({x, y}) for projected destination.
//       - Cull tiles fully outside screen bounds.

TileLayer::TileLayer(const Texture *tileset, int tileW, int tileH, int tilesPerRow)
    : m_tileset(tileset),
      m_tileW(tileW > 0 ? tileW : 1),
      m_tileH(tileH > 0 ? tileH : 1),
      m_tilesPerRow(tilesPerRow > 0 ? tilesPerRow : 1)
{
}

void TileLayer::setTiles(int mapWidth, int mapHeight, const std::vector<int> &tileIndices)
{
    if (mapWidth <= 0 || mapHeight <= 0)
    {
        m_mapWidth = 0;
        m_mapHeight = 0;
        m_tiles.clear();
        return;
    }

    const size_t requiredTiles = static_cast<size_t>(mapWidth) * static_cast<size_t>(mapHeight);
    if (tileIndices.size() < requiredTiles)
    {
        m_mapWidth = 0;
        m_mapHeight = 0;
        m_tiles.clear();
        return;
    }

    m_mapWidth = mapWidth;
    m_mapHeight = mapHeight;
    m_tiles.assign(tileIndices.begin(), tileIndices.begin() + static_cast<std::ptrdiff_t>(requiredTiles));
}

void TileLayer::render(SpriteBatch &batch, const Camera &camera, Vec2f screenSize) const
{
    if (!m_tileset || m_mapWidth <= 0 || m_mapHeight <= 0 || m_tiles.empty())
        return;

    const size_t requiredTiles = static_cast<size_t>(m_mapWidth) * static_cast<size_t>(m_mapHeight);
    if (m_tiles.size() < requiredTiles)
        return;

    const int textureWidth = m_tileset->getWidth();
    const int textureHeight = m_tileset->getHeight();
    if (textureWidth <= 0 || textureHeight <= 0)
        return;

    // Tile dimensions scaled by current zoom — dst rects must match screen space.
    const float zoom = camera.getZoom();
    const int dstW = static_cast<int>(m_tileW * zoom);
    const int dstH = static_cast<int>(m_tileH * zoom);

    // Ask the camera which direction to iterate so painter's algorithm is correct
    // for the current battlefield rotation.
    //   R0   -> { 1,  1}   R90  -> { 1, -1}
    //   R180 -> {-1, -1}   R270 -> {-1,  1}
    const Vec2i order = camera.getDrawOrder();

    const int xStart = (order.x > 0) ? 0 : m_mapWidth - 1;
    const int xEnd = (order.x > 0) ? m_mapWidth : -1;
    const int yStart = (order.y > 0) ? 0 : m_mapHeight - 1;
    const int yEnd = (order.y > 0) ? m_mapHeight : -1;

    for (int y = yStart; y != yEnd; y += order.y)
    {
        for (int x = xStart; x != xEnd; x += order.x)
        {
            const int tileIndex = m_tiles[y * m_mapWidth + x];

            // Index 0 = empty slot. Negative values are treated as invalid and skipped.
            if (tileIndex <= 0)
                continue;

            const int atlasIndex = tileIndex - 1;
            const int srcX = (atlasIndex % m_tilesPerRow) * m_tileW;
            const int srcY = (atlasIndex / m_tilesPerRow) * m_tileH;

            // Skip tile IDs whose computed source rect falls outside the tileset.
            if (srcX < 0 || srcY < 0 || srcX + m_tileW > textureWidth || srcY + m_tileH > textureHeight)
                continue;

            // Convert a 1-based tile index to a source rect in the sprite sheet.
            //   col = (index - 1) % tilesPerRow   -> horizontal offset in sheet
            //   row = (index - 1) / tilesPerRow   -> vertical offset in sheet
            const Recti src{
                srcX,
                srcY,
                m_tileW,
                m_tileH};

            // tileToScreen gives the top-left screen pixel of this tile's iso diamond.
            const Vec2f screenPos = camera.tileToScreen(Vec2i{x, y});

            const Rectf dst{
                screenPos.x,
                screenPos.y,
                static_cast<float>(dstW),
                static_cast<float>(dstH)};

            // Culling: skip tiles entirely outside the screen.
            // Checking all four edges avoids submitting invisible draw calls.
            if (dst.x + dst.w < 0 || dst.x > screenSize.x)
                continue;
            if (dst.y + dst.h < 0 || dst.y > screenSize.y)
                continue;

            batch.draw(m_tileset, src, dst, false);
        }
    }
}
