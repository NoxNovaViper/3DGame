# 🗡️ SAO Game — Project Overview

This repository hosts the **SAO Game Engine**, a C++17-based project leveraging OpenGL 3.3 Core for rendering, GLFW for window/input handling, and GLM for math operations.

## 🌱 Key Features

- **World Generation**: Procedural chunk-based world with biome-specific rendering.
- **Biome Support**: Integration of biome-specific textures (e.g., `textures/forest.png`, `textures/desert.png`).
- **Chunk Management**: Dynamic loading/unloading of chunks with proper texture assignment.

## 📝 Project Context

For detailed architecture, constraints, and current systems, refer to [`project-context.md`](project-context.md).

## 🔧 Dependencies

```
C++17 | OpenGL 3.3 Core | GLFW 3 | GLAD | GLM
```

## 📄 Texture Files

Biome-specific textures must be stored in the `textures/` directory:
- `textures/forest.png`
- `textures/desert.png`
- `textures/plains.png`

## 🚀 Recent Updates

### Biome-Specific Rendering Pipeline

The biome-specific rendering functionality has been successfully integrated into the following classes:

- **`WorldRenderer`**: Updated `RenderChunkWithBiome` to render chunks with biome-specific textures.
- **`ChunkManager`**: Added methods (`GetChunk`, `LoadChunk`, `UnloadChunk`, `RebuildDirtyChunks`) and ensured proper `worldGen` initialization.
- **`Chunk`**: Modified `Draw` method to support biome-specific rendering.

### Changes Made

1. **`ChunkManager.h`/`ChunkManager.cpp`**:
   - Added missing method declarations.
   - Updated `Render` method to ensure correct texture rendering.

2. **`Chunk.h`/`Chunk.cpp`**:
   - Modified `Draw` method for biome compatibility.

3. **`WorldRenderer.cpp`**:
   - Ensured `RenderChunkWithBiome` correctly calls `chunk.Draw()` without a `Shader` parameter.
   - Maintained `RenderChunks` to iterate over chunks with biome-specific textures.

4. **Texture Loading**: Confirmed `stb_image.h` inclusion in `WorldRenderer.cpp`.

## 🧪 Next Steps

- **Verify biome texture files** are present in the `textures/` directory.
- **Test rendering** in a controlled environment to confirm biome-specific textures are applied correctly.

---