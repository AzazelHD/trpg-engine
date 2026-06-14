#pragma once
#include <vector>
#include "engine/math/Vec2.h"
#include "engine/renderer/Texture.h"

// Forward declarations — avoid pulling full headers into every consumer.
class SpriteBatch;
class Camera;

// TileLayer renders a 2D grid of tiles from a tileset texture using isometric projection.
// It asks the Camera to convert each tile's logical position to a screen position.
//
// --- ISOMETRIC DRAW ORDER ---
// Tiles must be drawn back-to-front (painter's algorithm) so nearer tiles visually
// overlap farther ones. The correct order depends on the current camera rotation.
// Camera::getDrawOrder() returns {xDir, yDir} (+1 = ascending, -1 = descending)
// so the layer always iterates in the right direction regardless of rotation.
//
// --- CULLING ---
// render() skips any tile whose projected dst rect is fully outside the screen.
// This keeps draw call count proportional to visible tiles, not total map size.
// Pass the current screen size to render() so it can do the bounds test.
//
// Checklist — mark with [x] once done:
//
// [x] Constructor: TileLayer(const Texture* tileset, int tileW, int tileH, int tilesPerRow)
//       tilesPerRow tells you how to convert a tile index to a src rect in the sheet.
//       tileW/tileH here are the pixel dimensions of one tile in the sprite sheet.
//
// [x] setTiles(int mapWidth, int mapHeight, const std::vector<int>& tileIndices)
//       Stores the map data. tileIndices[y * mapWidth + x] = tile index at (x, y).
//       Index 0 = empty, skip drawing. Positive indices are 1-based tileset IDs.
//
// [x] render(SpriteBatch& batch, const Camera& camera, Vec2f screenSize)
//       Iterate in camera-provided back-to-front order.
//       For each non-empty tile:
//           src  = compute source rect from tile index and tilesPerRow
//           dst  = camera.tileToScreen({x, y})
//           batch.draw(m_tileset, src, dst)
//       Culling: skip tiles whose projected dst rect is outside screen bounds.
class TileLayer
{
public:
    // tileset     — sprite sheet containing all tile frames.
    // tileW/H     — pixel dimensions of one tile frame in the sheet.
    // tilesPerRow — how many tile frames are in one row of the sheet.
    //               Used to convert a 1-based tile index into a (col, row) src rect:
    //                   col = (index - 1) % tilesPerRow
    //                   row = (index - 1) / tilesPerRow
    TileLayer(const Texture *tileset, int tileW, int tileH, int tilesPerRow);

    // Store map data. tileIndices[y * mapWidth + x] = tile index at (x, y).
    // Index 0 = empty tile, skipped during render. Positive indices are 1-based.
    void setTiles(int mapWidth, int mapHeight, const std::vector<int> &tileIndices);

    // Draw all visible tiles into the batch using the camera's current transform.
    // screenSize is needed for culling — pass the window/viewport dimensions.
    void render(SpriteBatch &batch, const Camera &camera, Vec2f screenSize) const;

private:
    const Texture *m_tileset = nullptr;
    int m_mapWidth = 0;
    int m_mapHeight = 0;
    int m_tileW = 64;
    int m_tileH = 32;
    int m_tilesPerRow = 1;
    std::vector<int> m_tiles;
    // [x] store a reference or pointer to the tileset Texture
};
