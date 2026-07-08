# Vulkan Renderer

A learning project for building a Vulkan renderer from scratch in C++.

This project follows Brendan Galea's Vulkan Game Engine tutorial series, with adjustments for macOS / MoltenVK. The goal is to understand the Vulkan rendering workflow step by step instead of only using a high-level graphics engine.

## Current Progress

### Tutorial 01: Opening a Window

- Created the initial C++ project structure
- Added basic application entry point
- Added `VrWindow` wrapper class
- Initialized GLFW
- Created a GLFW window with Vulkan support
- Disabled the default OpenGL context by using `GLFW_NO_API`

### Tutorial 02: Graphics Pipeline Overview

- Studied the high-level Vulkan graphics pipeline flow
- Reviewed the purpose of programmable shader stages
- Reviewed fixed-function pipeline stages
- Prepared the project for graphics pipeline implementation

### Tutorial 03: Device Setup & Pipeline Continued

- Added `VrDevice` wrapper class
- Created Vulkan instance
- Added validation layer support
- Created debug messenger
- Created window surface
- Selected physical device
- Created logical device
- Retrieved graphics and present queues
- Created command pool
- Added macOS / MoltenVK portability fixes

### Tutorial 04: Fixed Function Pipeline Stages

- Added `VrPipeline` wrapper class
- Loaded compiled SPIR-V shader files
- Created vertex and fragment shader modules
- Added `PipelineConfigInfo` struct
- Set up fixed-function pipeline configuration:
  - input assembly
  - viewport and scissor
  - rasterization
  - multisampling
  - color blending
  - depth/stencil state
- Created graphics pipeline
- Added pipeline bind function

### Tutorial 05 Part 1: Swap Chain Overview

- Added `VrSwapChain` wrapper class
- Queried swap chain support details
- Selected surface format
- Selected present mode
- Selected swap extent
- Created Vulkan swap chain
- Retrieved swap chain images
- Created swap chain image views
- Added render pass
- Added depth resources
- Added framebuffers

### Tutorial 05 Part 2: Command Buffers Overview

- Allocated command buffers
- Began and ended command buffer recording
- Began and ended render pass commands
- Bound graphics pipeline
- Issued draw commands
- Added synchronization objects:
  - image available semaphores
  - render finished semaphores
  - in-flight fences
- Submitted command buffers to the graphics queue
- Presented rendered swap chain images to the present queue
- Fixed semaphore reuse issue for newer Vulkan validation layers by using render-finished semaphores per swap chain image
- Fixed cleanup order for swap chain resources

### Tutorial 06: Vertex Buffers

- Added `VrModel` wrapper class
- Added `Vertex` struct for storing vertex data
- Created and allocated Vulkan vertex buffer
- Mapped CPU-side vertex data into GPU-accessible memory
- Copied triangle vertex positions into the vertex buffer
- Added vertex binding descriptions to describe vertex stride
- Added vertex attribute descriptions to describe vertex input layout
- Updated graphics pipeline to use vertex input state
- Updated vertex shader to read position data from the vertex buffer

### Tutorial 07: Fragment Interpolation

- Extended the `Vertex` struct with color data
- Updated vertex attribute descriptions for multiple vertex attributes
- Connected vertex shader outputs to fragment shader inputs
- Passed color data from vertex shader to fragment shader
- Updated fragment shader to render interpolated vertex colors
- Rendered a colored triangle using per-vertex color attributes

### Tutorial 08: Swap Chain Recreation & Dynamic Viewports

- Added window resize handling
- Added framebuffer resize flag to the window class
- Added swap chain recreation logic
- Handled `VK_ERROR_OUT_OF_DATE_KHR`
- Handled `VK_SUBOPTIMAL_KHR`
- Waited for valid non-zero window extent before recreating swap chain
- Used old swap chain during swap chain recreation
- Added dynamic viewport and scissor state
- Removed pipeline dependency on fixed swap chain width and height
- Allowed resizing without recreating the graphics pipeline when render pass formats stay compatible

### Tutorial 09: Push Constants

- Added push constant data structure
- Added push constant range to pipeline layout
- Sent per-object data to shaders through push constants
- Used push constants for object color, offset, and transform data
- Updated shaders to consume push constant values
- Prepared the renderer for drawing multiple objects with different per-object data

### Tutorial 10: 2D Transformations

- Added `VrGameObject`
- Added 2D transform component data
- Added translation, scale, and rotation values
- Built a 2D transformation matrix
- Applied object transforms through push constants
- Animated object rotation over time
- Separated model data from object instance data

### Tutorial 11: Renderer & Systems

- Added `VrRenderer` class
- Moved swap chain ownership from `FirstApp` into `VrRenderer`
- Moved command buffer ownership into `VrRenderer`
- Moved frame lifecycle logic into `VrRenderer`:
  - `beginFrame`
  - `endFrame`
  - `beginSwapChainRenderPass`
  - `endSwapChainRenderPass`
- Added `currentFrameIndex` for frame-in-flight resources
- Added `currentImageIndex` for swap chain image/framebuffer resources
- Added `isFrameStarted` guard to prevent invalid frame usage
- Added `SimpleRenderSystem`
- Moved graphics pipeline and pipeline layout ownership into `SimpleRenderSystem`
- Moved game object draw logic into `SimpleRenderSystem::renderGameObjects`
- Kept `FirstApp` focused on high-level application flow
- Compared old and new swap chain formats during recreation to check pipeline/render pass compatibility

## Current Architecture

The project is now split into smaller responsibilities:

```txt
FirstApp
  - Owns high-level application flow
  - Owns game objects
  - Runs the main loop
  - Calls renderer and render systems

VrRenderer
  - Owns swap chain
  - Owns command buffers
  - Manages frame begin/end
  - Manages render pass begin/end
  - Handles swap chain recreation

VrSwapChain
  - Owns swap chain images
  - Owns image views
  - Owns render pass
  - Owns depth resources
  - Owns framebuffers
  - Owns synchronization objects

SimpleRenderSystem
  - Owns pipeline layout
  - Owns graphics pipeline
  - Binds pipeline
  - Pushes per-object constants
  - Draws game objects

VrModel
  - Owns vertex buffer
  - Stores vertex count
  - Binds and draws model geometry

VrDevice
  - Owns Vulkan instance/device setup
  - Owns physical and logical device
  - Owns queues
  - Owns command pool
  - Provides helper functions for buffers and images

VrWindow
  - Owns GLFW window
  - Creates Vulkan surface
  - Tracks resize state
```

## Current Renderer Flow

```txt
Program startup
  FirstApp creates:
    VrWindow
    VrDevice
    VrRenderer
      VrRenderer creates:
        VrSwapChain
          swap chain images
          image views
          render pass
          depth resources
          framebuffers
          sync objects
        command buffers

FirstApp::run()
  creates SimpleRenderSystem
    pipeline layout
    graphics pipeline

Each frame:
  glfwPollEvents()

  VrRenderer::beginFrame()
    acquire swap chain image
    store currentImageIndex
    get command buffer using currentFrameIndex
    begin command buffer

  VrRenderer::beginSwapChainRenderPass()
    choose framebuffer using currentImageIndex
    begin render pass
    set dynamic viewport and scissor

  SimpleRenderSystem::renderGameObjects()
    bind graphics pipeline
    update object transform
    push constants
    bind model vertex buffer
    draw model

  VrRenderer::endSwapChainRenderPass()
    end render pass

  VrRenderer::endFrame()
    end command buffer
    submit command buffer
    present swap chain image
    recreate swap chain if needed
    advance currentFrameIndex
```

## Important Concepts Learned

### `currentFrameIndex`

`currentFrameIndex` chooses which frame-in-flight resources are being used.

Use it for resources sized by `MAX_FRAMES_IN_FLIGHT`, such as:

```cpp
commandBuffers[currentFrameIndex]
imageAvailableSemaphores[currentFrameIndex]
inFlightFences[currentFrameIndex]
```

### `currentImageIndex`

`currentImageIndex` is the swap chain image index returned by Vulkan when acquiring the next image.

Use it for resources sized by swap chain image count, such as:

```cpp
framebuffers[currentImageIndex]
imagesInFlight[currentImageIndex]
renderFinishedSemaphores[currentImageIndex]
```

### Difference Between Frame Index and Image Index

```txt
currentFrameIndex = which frame slot / command buffer am I using?
currentImageIndex = which swap chain image / framebuffer did Vulkan give me?
```

Example:

```txt
currentFrameIndex = 0
currentImageIndex = 2

Use:
  commandBuffers[0]
  framebuffers[2]
```

## Build

```bash
rm -rf build
mkdir build
cd build
cmake ..
make
./Vulkan-Renderer
```

## Notes

This project is developed on macOS with MoltenVK, so some Vulkan setup differs from the original tutorial. macOS-specific instance/device extensions and portability flags are required.
