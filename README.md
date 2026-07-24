# Vulkan Renderer

A learning project for building a small Vulkan renderer from scratch in C++.

This project follows [Brendan Galea's Vulkan Game Engine tutorial series](https://www.youtube.com/playlist?list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR), with adjustments for macOS and MoltenVK. The goal is to understand Vulkan's rendering workflow, resource ownership, synchronization, graphics mathematics, and engine architecture step by step.

## Current Progress

Completed through **Tutorial 19: Uniform Buffers**, followed by a migration to selected Vulkan 1.3 features.

### Tutorial 01: Opening a Window

- Created the initial C++ project structure
- Added the application entry point
- Added a `VrWindow` wrapper class
- Initialized GLFW
- Created a GLFW window with Vulkan support
- Disabled the default OpenGL context with `GLFW_NO_API`

### Tutorial 02: Graphics Pipeline Overview

- Studied the high-level Vulkan graphics pipeline
- Reviewed programmable shader stages
- Reviewed fixed-function pipeline stages
- Learned how vertex data eventually becomes pixels on screen

### Tutorial 03: Device Setup & Pipeline Continued

- Added a `VrDevice` wrapper class
- Created the Vulkan instance
- Added validation-layer support
- Created a debug messenger
- Created the window surface
- Selected a physical device
- Created the logical device
- Retrieved graphics and presentation queues
- Created a command pool
- Added macOS and MoltenVK portability support

### Tutorial 04: Fixed-Function Pipeline Stages

- Added a `VrPipeline` wrapper class
- Loaded compiled SPIR-V shader files
- Created vertex and fragment shader modules
- Added `PipelineConfigInfo`
- Configured:
  - input assembly
  - viewport and scissor
  - rasterization
  - multisampling
  - color blending
  - depth and stencil testing
- Created and bound the graphics pipeline

### Tutorial 05 Part 1: Swap Chain Overview

- Added a `VrSwapChain` wrapper class
- Queried swap-chain support
- Selected the surface format, presentation mode, and extent
- Created the swap chain
- Retrieved swap-chain images
- Created image views
- Added the render pass
- Added depth resources
- Created framebuffers

### Tutorial 05 Part 2: Command Buffers

- Allocated command buffers
- Recorded render commands
- Began and ended render passes
- Bound the graphics pipeline
- Issued draw commands
- Added synchronization using:
  - image-available semaphores
  - render-finished semaphores
  - in-flight fences
- Submitted command buffers to the graphics queue
- Presented rendered images
- Fixed semaphore reuse and resource cleanup issues

### Tutorial 06: Vertex Buffers

- Added a `VrModel` wrapper class
- Added a `Vertex` structure
- Created and allocated a Vulkan vertex buffer
- Mapped CPU-visible memory
- Copied vertex data into the buffer
- Added vertex binding descriptions
- Added vertex attribute descriptions
- Updated the graphics pipeline and vertex shader to consume vertex data

### Tutorial 07: Fragment Interpolation

- Added per-vertex color data
- Passed color from the vertex shader to the fragment shader
- Added multiple vertex attributes
- Rendered interpolated colors across triangle surfaces

### Tutorial 08: Swap-Chain Recreation & Dynamic Viewports

- Added window-resize handling
- Tracked framebuffer resize events
- Added swap-chain recreation
- Handled `VK_ERROR_OUT_OF_DATE_KHR`
- Handled `VK_SUBOPTIMAL_KHR`
- Waited for a valid, non-zero window extent
- Reused the old swap chain during recreation
- Added dynamic viewport and scissor state
- Removed the need to recreate the pipeline for ordinary window resizing

### Tutorial 09: Push Constants

- Added a push-constant data structure
- Added a push-constant range to the pipeline layout
- Sent small amounts of per-object data directly through command buffers
- Used push constants for object color and transformations
- Drew multiple objects with different per-object data

### Tutorial 10: 2D Transformations

- Added `VrGameObject`
- Added translation, scale, and rotation properties
- Built 2D transformation matrices
- Applied object transforms through push constants
- Animated object rotation
- Separated shared model data from object-instance data

### Tutorial 11: Renderer & Systems

- Added `VrRenderer`
- Moved swap-chain and command-buffer ownership out of `FirstApp`
- Added frame lifecycle methods:
  - `beginFrame`
  - `endFrame`
  - `beginSwapChainRenderPass`
  - `endSwapChainRenderPass`
- Added `currentFrameIndex` and `currentImageIndex`
- Added frame-state validation
- Added `SimpleRenderSystem`
- Moved pipeline ownership and draw logic into the render system
- Kept `FirstApp` focused on high-level application flow

### Tutorial 12: Euler Angles & Homogeneous Coordinates

- Converted the renderer from 2D to 3D
- Expanded positions and transforms to use `glm::vec3`
- Built 3D translation, rotation, and scale matrices
- Used Euler angles for object rotation
- Added homogeneous coordinates
- Updated push constants and shaders for 3D transformations
- Added depth testing for overlapping geometry

### Tutorial 13: Projection Matrices

- Studied perspective projection mathematics
- Added a `VrCamera` class
- Implemented orthographic projection
- Implemented perspective projection
- Added configurable field of view, aspect ratio, and near/far clipping planes
- Applied the projection matrix in the vertex shader

### Tutorial 14: Camera View Transform

- Implemented camera view matrices
- Added target-based camera orientation
- Added Euler-angle camera orientation
- Added inverse view-matrix calculations
- Separated camera movement from object movement
- Combined projection, view, and model transformations

### Tutorial 15: Game Loop & User Input

- Added frame-time tracking
- Added `KeyboardMovementController`
- Implemented keyboard-controlled camera movement
- Added camera rotation controls
- Made movement frame-rate independent
- Added a viewer game object to store camera position and orientation
- Updated camera transforms each frame

### Tutorial 16: Index & Staging Buffers

- Added index-buffer support
- Reused vertices through indexed drawing
- Added staging buffers for GPU uploads
- Copied data from host-visible staging memory into device-local buffers
- Added single-use command-buffer helpers
- Added general buffer-copy utilities
- Improved vertex-buffer performance by storing final geometry in device-local memory

### Tutorial 17: Loading 3D Models

- Integrated `tinyobjloader`
- Added Wavefront OBJ model loading
- Loaded vertex positions, colors, and normals
- Generated index data
- Deduplicated repeated vertices
- Added hashing support for vertices
- Replaced hardcoded geometry with external model assets

### Tutorial 18: Diffuse Shading

- Added surface normals to vertex data
- Passed normals from the vertex shader to the fragment shader
- Added directional-light calculations
- Implemented diffuse lighting using the dot product
- Added ambient lighting
- Added a normal matrix for correctly transforming normals
- Updated per-object push constants with model and normal matrices
- Added basic shaded 3D model rendering

### Tutorial 19: Uniform Buffers

- Added a reusable `VrBuffer` abstraction
- Centralized Vulkan buffer creation, allocation, mapping, writing, and cleanup
- Added support for aligned buffer instances
- Accounted for Vulkan memory-alignment requirements
- Added handling for non-coherent memory flushing and invalidation
- Added a `GlobalUbo` structure for frame-global rendering data
- Prepared projection and view matrices for transfer through a uniform buffer
- Created one uniform buffer per frame in flight
- Persistently mapped uniform-buffer memory
- Updated the current frame's uniform data each frame
- Prepared the renderer for descriptor sets in Tutorial 20


### Modern Vulkan: Vulkan 1.3 Feature Setup & Dynamic Rendering

- Updated the application API version to Vulkan 1.3
- Queried Vulkan 1.3 feature support through:
  - `VkPhysicalDeviceFeatures2`
  - `VkPhysicalDeviceVulkan13Features`
- Explicitly enabled:
  - `dynamicRendering`
  - `synchronization2`
- Kept legacy `pEnabledFeatures` null when enabling features through the `pNext` chain
- Replaced graphics-pipeline render-pass compatibility with:
  - `VkPipelineRenderingCreateInfo`
  - explicit color and depth attachment formats
- Replaced:
  - `vkCmdBeginRenderPass`
  - `vkCmdEndRenderPass`
- Added:
  - `vkCmdBeginRendering`
  - `vkCmdEndRendering`
  - `VkRenderingAttachmentInfo`
  - `VkRenderingInfo`
- Removed `VkRenderPass` creation and ownership
- Removed `VkFramebuffer` creation and ownership
- Passed the current swap-chain and depth image views directly to Dynamic Rendering
- Added explicit image-layout transitions using:
  - `VkImageMemoryBarrier2`
  - `VkDependencyInfo`
  - `vkCmdPipelineBarrier2`
- Transitioned swap-chain images between:
  - `VK_IMAGE_LAYOUT_UNDEFINED`
  - `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL`
  - `VK_IMAGE_LAYOUT_PRESENT_SRC_KHR`
- Transitioned depth images into:
  - `VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL`
- Preserved dynamic viewport and scissor state
- Kept swap-chain recreation compatible by verifying color and depth formats

#### Architecture Change

```text
Before:

VrSwapChain
├── Swap-chain images and image views
├── Depth images and image views
├── VkRenderPass
└── VkFramebuffer objects
        ↑
VrPipeline depends on VkRenderPass compatibility
        ↑
VrRenderer begins a render pass with a framebuffer


After:

VrSwapChain
├── Swap-chain images and image views
└── Depth images and image views
        ↑
VrRenderer directly selects the current ImageViews
        ↓
Explicit image barriers
        ↓
vkCmdBeginRendering / vkCmdEndRendering

VrPipeline
└── Depends on color and depth attachment formats
```

Dynamic Rendering removes the need to pre-create render-pass and framebuffer objects. Attachment formats are declared when the graphics pipeline is created, while the actual image views, clear operations, and render area are supplied while recording each command buffer.

## Current Architecture

```text
FirstApp
├── VrWindow
├── VrDevice
│   └── Vulkan 1.3 feature configuration
├── VrRenderer
│   ├── VrSwapChain
│   │   ├── Swap-chain images and image views
│   │   ├── Depth images and image views
│   │   └── Synchronization resources
│   ├── Command buffers
│   └── Dynamic Rendering and image transitions
├── SimpleRenderSystem
│   ├── Graphics pipeline
│   └── Pipeline layout
├── VrCamera
├── KeyboardMovementController
├── Game objects
├── Models
└── Per-frame uniform buffers
```

### Main Responsibilities

#### `FirstApp`

- Owns high-level application flow
- Loads models and creates game objects
- Runs the main loop
- Updates camera movement
- Updates per-frame global rendering data
- Calls the renderer and render systems

#### `VrRenderer`

- Owns command buffers
- Owns the swap chain
- Starts and ends frames
- Begins and ends Dynamic Rendering
- Supplies the current color and depth image views
- Records explicit image-layout transitions
- Transitions rendered swap-chain images into presentation layout
- Sets dynamic viewport and scissor state
- Handles swap-chain recreation
- Tracks the current frame and swap-chain image

#### `VrSwapChain`

- Owns swap-chain images and image views
- Owns depth images, memory, and image views
- Stores the active color and depth formats
- Owns synchronization resources
- Acquires and presents swap-chain images
- Recreates presentation resources when the window changes
- No longer owns `VkRenderPass` or `VkFramebuffer` objects

#### `SimpleRenderSystem`

- Owns the graphics pipeline and pipeline layout
- Creates a Dynamic Rendering-compatible graphics pipeline
- Configures expected color and depth attachment formats
- Binds the pipeline
- Sends per-object transformation data
- Binds models
- Issues indexed or non-indexed draw calls

#### `VrModel`

- Loads model geometry
- Owns vertex and index buffers
- Binds geometry buffers
- Issues model draw commands

#### `VrBuffer`

- Wraps `VkBuffer` and `VkDeviceMemory`
- Handles memory mapping
- Writes CPU data into mapped memory
- Handles alignment, flushing, and invalidation

#### `VrCamera`

- Creates projection matrices
- Creates view matrices
- Stores camera-space transformations

#### `KeyboardMovementController`

- Reads keyboard input
- Moves and rotates the camera
- Applies frame-time-independent movement

## Frame Flow

```text
Poll window events
        ↓
Calculate frame time
        ↓
Update camera controller
        ↓
Update camera view and projection
        ↓
Acquire a swap-chain image
        ↓
Begin the current frame's command buffer
        ↓
Select uniform buffer using currentFrameIndex
        ↓
Write current GlobalUbo data
        ↓
Transition color image:
UNDEFINED → COLOR_ATTACHMENT_OPTIMAL
        ↓
Transition depth image:
UNDEFINED → DEPTH_ATTACHMENT_OPTIMAL
        ↓
Begin Dynamic Rendering with current ImageViews
        ↓
Set dynamic viewport and scissor
        ↓
Bind graphics pipeline
        ↓
For each game object:
    update push constants
    bind model buffers
    draw model
        ↓
End Dynamic Rendering
        ↓
Transition color image:
COLOR_ATTACHMENT_OPTIMAL → PRESENT_SRC_KHR
        ↓
End and submit command buffer
        ↓
Present swap-chain image
        ↓
Advance currentFrameIndex
```

## Frame Index vs. Image Index

### `currentFrameIndex`

Identifies the current frame-in-flight resource slot.

Used for resources such as:

```cpp
commandBuffers[currentFrameIndex]
uniformBuffers[currentFrameIndex]
imageAvailableSemaphores[currentFrameIndex]
inFlightFences[currentFrameIndex]
```

### `currentImageIndex`

Identifies the swap-chain image returned by `vkAcquireNextImageKHR`.

Used for resources such as:

```cpp
swapChainImages[currentImageIndex]
swapChainImageViews[currentImageIndex]
depthImages[currentImageIndex]
depthImageViews[currentImageIndex]
imagesInFlight[currentImageIndex]
```

These indices are related, but they are not guaranteed to have the same value.

## Data Sent to Shaders

```text
Vertex buffer
└── Per-vertex data
    ├── position
    ├── color
    └── normal

Index buffer
└── Reuses vertices to construct triangles

Push constants
└── Small per-object data
    ├── model matrix
    └── normal matrix

Uniform buffer
└── Shared per-frame data
    ├── projection matrix
    └── view matrix
```

## Build

### Requirements

- macOS
- CMake
- Vulkan SDK
- GLFW
- GLM
- `tinyobjloader`
- A C++17-compatible compiler

Install common Homebrew dependencies:

```bash
brew install cmake glfw glm tinyobjloader
```

Make sure the Vulkan SDK environment is configured before building.

### Compile and Run

```bash
rm -rf build
mkdir build
cd build
cmake ..
make
./Vulkan-Renderer
```

## Platform Notes

This project is developed on macOS using MoltenVK. It includes platform-specific Vulkan configuration such as:

- `VK_KHR_portability_enumeration`
- `VK_KHR_portability_subset`
- `VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR`
- Metal-compatible GLFW surface creation

Some setup therefore differs from the original Windows-focused tutorial implementation.

## Next Step

Continue with **Tutorial 20: Descriptor Sets** to connect uniform buffers to shaders using:

- descriptor set layouts
- descriptor pools
- descriptor sets
- descriptor writes
- pipeline-layout descriptor bindings

After the descriptor-system work is stable, continue modernizing queue submission by replacing the remaining legacy submission path with:

- `VkSubmitInfo2`
- `VkSemaphoreSubmitInfo`
- `VkCommandBufferSubmitInfo`
- `vkQueueSubmit2`

## Learning Goals

This project is intended to build practical understanding of:

- Vulkan resource creation and ownership
- CPU-to-GPU memory transfer
- frame synchronization
- swap-chain management
- command-buffer recording
- Vulkan 1.3 feature querying and enabling
- Dynamic Rendering
- explicit image-layout transitions
- Synchronization2 barriers
- shader data interfaces
- 3D transformation mathematics
- camera systems
- model loading
- basic lighting
- renderer architecture
