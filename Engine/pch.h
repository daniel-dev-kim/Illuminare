#pragma once
#define GLM_ENABLE_EXPERIMENTAL

// 컨테이너
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>

// C++ 표준 라이브러리
#include <iostream>
#include <memory>
#include <typeindex>
#include <functional>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <execution>  // C++17 parallel execution

// 멀티쓰레딩
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>

// 디버깅
#include <stdexcept>

// 외부 라이브러리
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLAD/glad.h>
#include <GLFW/GLFW/include/GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
