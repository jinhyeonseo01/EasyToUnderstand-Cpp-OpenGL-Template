#pragma once
#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <iomanip>

#include <chrono>
#include <Thread>
#include <time.h>
#include <windows.h>
#include <WinUser.h>

#include <fstream>

//imgui
#include "imgui/imgui.h"
#include "imgui_local/imgui_impl_glut.h"
#include "imgui_local/imgui_impl_opengl3.h"

//#include "image_loader/stb_image.h"
#include "image_loader/SOIL.h"

//----------

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtx/quaternion.hpp>

#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "assimpEx.h"

#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

#pragma comment(lib)

// -------------

#include <random>
#include <math.h>
#include <algorithm>
#include <map>
#include <string>


#define D2R 0.01745329251994327f
#define R2D 57.2957795130823799f

#define Max(a, b) (a>b?a:b)
#define Min(a, b) (a<b?a:b)