# 🚀 Space-Exploration-OpenGL-Project

## 🪐 Project Description  

This project presents the development of a 3D Martian environment using OpenGL in Visual Studio. The simulation renders a realistic Mars research station scene, allowing users to explore a fully textured and interactive world with dynamic lighting and environmental effects.

Users can freely navigate the terrain using the keyboard and mouse, switching between different visualization modes to observe the geometry, lighting, and shaders in action. The scene is loaded from `.obj` models and includes a variety of structures such as a Mars base, a launch rocket, astronauts, a rover, and a mysterious alien UFO.

The project demonstrates the integration of shaders, lighting systems, camera mechanics, and visual effects, showcasing a complete 3D pipeline that interacts directly with the GPU.

![Screenshot 2025-04-05 215322](https://github.com/user-attachments/assets/76c5362b-90a7-480c-a223-57e441efe67a)

## 🧠 Implementation Details  

### OpenGL Pipeline and Shaders  
The scene is rendered using GLSL shaders that implement the **Phong lighting model** for realistic surface illumination. Fragment and vertex shaders are configured to handle both directional and point lighting, as well as additional effects like fog and glow.

Key features:
- **Directional light** 
- **Point lights** 
- **Spotlight** 
- **Fog shader** 
- **SkyBox shader** wraps a cubemap around the scene

All objects are loaded from `.obj` files using a model loader, and appropriate transformations (scaling, rotation, translation) are applied to each.

---

## 🕹️ Controls and Viewing Modes  

The camera allows full 3D movement via keyboard and mouse. Users can explore the Martian base by walking, rotating, or zooming in.

- **W/A/S/D** – Move forward/left/backward/right
- **Mouse** – Rotate view
- **F** – Toggle fog
- **O** – Toggle spotlight
- **P** – Toggle point lights

Additional viewing modes:
- **J** – Object view
- **K** – Wireframe mode
- **L** – Vertex (points) mode

---

## 🛠️ Data Structures and Modules  

- `Camera`: Handles user view, movement, and rotation
- `Model3D`: Loads and draws .obj models
- `Mesh`: Stores vertex/index/texture data
- `Shader`: Compiles and links custom shaders
- `SkyBox`: Loads and displays the sky texture using a cubemap

