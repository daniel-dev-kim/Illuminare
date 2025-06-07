#pragma once
#define GLM_ENABLE_EXPERIMENTAL

// �����̳�
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>

// C++ ǥ�� ���̺귯��
#include <iostream>
#include <memory>
#include <typeindex>
#include <functional>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <execution>  // C++17 parallel execution

// ��Ƽ������
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>

// �����
#include <stdexcept>

// �ܺ� ���̺귯��
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
