#pragma once

#include "gl_pipeline.h"
#include "gl_pipeline.hpp"


void splitLine()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cerr << "──────────────────────────────────────────────────────────────────────────────────────────\n";
}
std::ostream& Log()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return std::cerr;
}
std::ostream& ErrorLog(std::ostream& o = std::cerr, const char* log = "Error", int depth = 0)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    for (int i = depth - 1; i >= 0; i--)
        o << ((i == 0) ? "       ├ " : "       │ ");
    o << std::setw(8);
    o << log;
    o << ": ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    return o;
}
std::ostream& NormalLog(std::ostream& o = std::cerr, const char* log = "Log", int depth = 0)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    for (int i = depth - 1; i >= 0; i--)
        o << ((i == 0) ? "       ├ " : "       │ ");
    o << std::setw(8);
    o << log;
    o << ": ";
    if (strcmp(log, "Try") == 0)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
    if (strcmp(log, "Info") == 0)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
    return o;
}
std::ostream& WarringLog(std::ostream& o = std::cerr, const char* log = "Warring", int depth = 0)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    for (int i = depth - 1; i >= 0; i--)
        o << ((i == 0) ? "       ├ " : "       │ ");
    o << std::setw(8);
    o << log;
    o << ": ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    return o;
}

char* filetobuf(const char* file)
{
    std::ifstream in(file, std::ios_base::binary);
    if (!in)
    {
        ErrorLog(std::cerr) << file << "Shader File Load - Falled : 경로 잘못됨\n";
        return nullptr;
    }
    in.seekg(0, std::ios_base::end);
    long len = in.tellg();
    char* buf = new char[len + 1];
    in.seekg(0, std::ios_base::beg);
    int cnt = -1;
    while (in >> std::noskipws >> buf[++cnt]) {}
    buf[len] = 0;
    return buf;
}

bool randomInit = false;
std::default_random_engine engine;
int RandomInt()
{
    if (randomInit == false)
    {
        randomInit = true;
        engine.seed(1234);
    }
    return engine();
}

int rand2()
{
    return RandomInt();
}

struct hash_str {
    size_t operator()(const char* s) const {
        size_t v = 0;
        while (char c = *s++) {
            v = (v << 6) + (v << 16) - v + c;
            return std::hash<int>()(v);
        }
    }
};
struct equal_str {
    bool operator () (const char* a, const char* b) const {
        return strcmp(a, b) == 0;
    }
};



int frameCount = 0;
float frameTotal = 0;
int frame[100];
std::chrono::steady_clock::time_point frameStartCheck;
std::chrono::steady_clock::time_point frameCheck[100];
void FrameInit(int count)
{
    frameCount = count;
    for (int i = 0; i < frameCount; i++)
    {
        frame[i] = -1;
        frameCheck[i] = std::chrono::steady_clock::now();
    }
    frameStartCheck = std::chrono::steady_clock::now();
}
void FrameUpdate()
{
    float tempFrameTotal = 0;
    for (int i = 0; i < frameCount; i++)
    {
        if (frame[i] != -1 || (frame[i] == -1 && std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - frameStartCheck).count() >= ((1000000.0f / frameCount) * i)))
        {
            auto now = std::chrono::steady_clock::now();
            if (frame[i] == -1)
                frameCheck[i] = now;
            if (std::chrono::duration_cast<std::chrono::microseconds>(now - frameCheck[i]).count() >= 1000000)
            {
                float weight = 1 / ((float)frameCount * 2.0f);
                frameTotal = (frameTotal * (1 - weight) + frame[i] * weight);
                frameCheck[i] = now;
                frame[i] = 0;
            }
            frame[i]++;
        }
    }
}




static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void ImGUIInit()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init();
    //ImGui_ImplGLUT_InstallFuncs();
    NormalLog(std::cerr, "Log") << "ImGUI 초기화 Completed\n";
    splitLine();
}

void ImGUIUpdate()
{
    if (CorePipeline::isEditorMode)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();


        ImGui::Render(); // IMGUI에 누적시키는 기능인가봄.
    }
}

void ImGUIRender()
{
    if (CorePipeline::isEditorMode)
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void ImGUIClose()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}