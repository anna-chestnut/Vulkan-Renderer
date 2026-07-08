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

### Tutorial 6: Vertex Buffers

- Added `VrModel` wrapper class
- Added `Vertex` struct for storing vertex data
- Created and allocated a Vulkan vertex buffer
- Mapped CPU-side vertex data into GPU-accessible memory
- Copied triangle vertex positions into the vertex buffer
- Added vertex binding descriptions to describe vertex stride
- Added vertex attribute descriptions to describe vertex input layout
- Updated graphics pipeline to use vertex input state
- Updated vertex shader to read position data from the vertex buffer

### Tutorial 7: Vertex Colors and Fragment Shader Input

- Extended the `Vertex` struct with color data
- Updated vertex attribute descriptions for multiple vertex attributes
- Connected vertex shader outputs to fragment shader inputs
- Passed color data from the vertex shader to the fragment shader
- Updated fragment shader to render interpolated vertex colors
- Rendered a colored triangle using per-vertex color attributes

### Tutorial 8: 2D Transformations and Push Constants

- Added `VrGameObject` to represent renderable scene objects
- Added per-object transform data:
  - translation
  - scale
  - rotation
- Added per-object color data
- Added push constants for small per-draw data
- Updated the vertex shader to apply a 2D transform matrix and offset
- Updated the render loop to draw objects through a simple render system
- Animated the triangle by updating its rotation every frame

### Tutorial 9: Renderer and Systems Refactor

- Added `VrRenderer` to manage frame lifecycle logic
- Moved swap chain ownership from `FirstApp` into `VrRenderer`
- Moved command buffer allocation/freeing into `VrRenderer`
- Added `beginFrame()` and `endFrame()` to clearly define one frame of work
- Added `beginSwapChainRenderPass()` and `endSwapChainRenderPass()` to isolate render pass recording
- Added `SimpleRenderSystem` to own the graphics pipeline and pipeline layout
- Moved object drawing logic out of `FirstApp` and into `SimpleRenderSystem`
- Simplified `FirstApp::run()` so it mainly coordinates the application loop
- Added frame state checks with `isFrameStarted` to catch invalid frame usage
- Added separate tracking for:
  - `currentFrameIndex`: the current frame-in-flight resource slot
  - `currentImageIndex`: the actual swap chain image acquired from Vulkan

### Tutorial 10: Swap Chain Recreation Refactor

- Added old swap chain support when recreating the swap chain
- Passed the previous swap chain into `VkSwapchainCreateInfoKHR::oldSwapchain`
- Added `compareSwapFormats()` to verify color/depth format compatibility after recreation
- Kept the graphics pipeline alive across resize when the new render pass is compatible
- Recreated swap chain resources when the window is resized or Vulkan reports the swap chain as out of date/suboptimal
- Improved synchronization ownership:
  - image-available semaphores are per frame in flight
  - in-flight fences are per frame in flight
  - render-finished semaphores are per swap chain image

## Current Architecture

The project is now split into smaller renderer components:

```txt
FirstApp
  owns: window, device, renderer, game objects
  job: application setup and main loop coordination

VrRenderer
  owns: swap chain and command buffers
  job: begin/end frames and begin/end swap chain render passes

VrSwapChain
  owns: swap chain images, image views, render pass, depth resources,
        framebuffers, and synchronization objects
  job: acquire images, submit command buffers, present images, recreate swap chain resources

SimpleRenderSystem
  owns: graphics pipeline and pipeline layout
  job: bind the pipeline, push per-object constants, and draw game objects

VrPipeline
  owns: Vulkan graphics pipeline
  job: load SPIR-V shaders and create pipeline state

VrModel
  owns: vertex buffer and vertex memory
  job: bind and draw vertex data
```

## Current Renderer Flow

```txt
Program startup
  ↓
Create window
  ↓
Create Vulkan instance / surface / device
  ↓
Create VrRenderer
  ↓
Create VrSwapChain
  ↓
Create swap chain images, image views, render pass, depth resources, framebuffers
  ↓
Create command buffers
  ↓
Load game objects
  ↓
Create SimpleRenderSystem
  ↓
Create pipeline layout and graphics pipeline
```

Each frame:

```txt
glfwPollEvents()
  ↓
VrRenderer::beginFrame()
  - wait for the current frame fence
  - acquire a swap chain image
  - begin recording the current command buffer
  ↓
VrRenderer::beginSwapChainRenderPass()
  - select framebuffer using currentImageIndex
  - begin the render pass
  - set viewport and scissor
  ↓
SimpleRenderSystem::renderGameObjects()
  - bind graphics pipeline
  - update object rotation
  - push transform/color constants
  - bind model vertex buffer
  - draw model
  ↓
VrRenderer::endSwapChainRenderPass()
  ↓
VrRenderer::endFrame()
  - end command buffer recording
  - submit command buffer
  - present the acquired swap chain image
  - recreate swap chain if needed
  - advance currentFrameIndex
```

## Important Concepts Learned

### Swap Chain Image vs Frame in Flight

`currentImageIndex` and `currentFrameIndex` track different things:

```txt
currentImageIndex
  = which swap chain image Vulkan gave us this frame
  = used for framebuffers, images-in-flight, and per-image render-finished semaphores

currentFrameIndex
  = which CPU/GPU frame slot we are currently using
  = used for command buffers, image-available semaphores, and in-flight fences
```

They do not always have the same value because the swap chain may have more images than the number of frames allowed in flight.

### Why the Pipeline Can Survive Resize

The swap chain recreation now creates a new swap chain, image views, depth resources, render pass, and framebuffers. The graphics pipeline can continue to be used only if the new render pass is compatible with the old one. The code checks this by comparing the swap chain color format and depth format.

If the formats change, the program throws an error instead of silently using an incompatible pipeline.

## Build and Run

From the project root:

```bash
rm -rf build
mkdir build
cd build
cmake ..
make
./Vulkan-Renderer
```

## Current Output

The renderer currently displays a colored triangle using vertex buffers, vertex colors, push constants, and a simple render system.

## Notes

- This project is primarily for learning Vulkan architecture and renderer structure.
- The code is being refactored step by step while following the tutorial series.
- macOS requires MoltenVK-specific setup, including portability-related Vulkan extensions.
