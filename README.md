# Vulkan Renderer

A learning project for building a Vulkan renderer from scratch in C++.

This project follows the general structure of the Little Vulkan Engine tutorial, with adjustments for macOS / MoltenVK. The goal is to understand the Vulkan rendering workflow step by step instead of only using a high-level graphics engine.

## Current Progress

### Tutorial 1: Project Setup

- Created the initial C++ project structure
- Added CMake build setup
- Linked Vulkan, GLFW, and GLM
- Added basic application entry point
- Added window creation through GLFW

### Tutorial 4: Graphics Pipeline Setup

- Added `VrPipeline` wrapper class
- Loaded compiled SPIR-V shader files
- Created vertex and fragment shader modules
- Set up basic graphics pipeline configuration
- Added pipeline layout support
- Added macOS-specific Vulkan setup fixes

### Tutorial 5: Draw Triangle and Swap Chain Setup

- Added `VrSwapChain` wrapper class
- Created Vulkan swap chain
- Created swap chain image views
- Added render pass
- Added depth resources
- Added framebuffers
- Added command buffer allocation and recording
- Added synchronization objects:
  - image available semaphores
  - render finished semaphores
  - in-flight fences
- Fixed semaphore reuse issue for newer Vulkan validation layers by using render-finished semaphores per swap chain image
- Fixed cleanup order for swap chain resources

## Current Renderer Flow

The current renderer setup is roughly:

```txt
Window
  ↓
Vulkan Instance
  ↓
Surface
  ↓
Physical Device
  ↓
Logical Device
  ↓
Swap Chain
  ↓
Image Views
  ↓
Render Pass
  ↓
Depth Resources
  ↓
Framebuffers
  ↓
Pipeline Layout
  ↓
Graphics Pipeline
  ↓
Command Buffers
  ↓
Draw / Present Loop