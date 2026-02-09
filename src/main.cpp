#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
#include "ImGuizmo.h"

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

// --- Data Structures ---
struct Keyframe {
    float time;
    Vector3 translation;
    Quaternion rotation;
};

struct BoneTrack {
    std::string name;
    int nodeIndex;
    std::vector<Keyframe> keys;
};

class VertexEngine {
public:
    tinygltf::Model model;
    std::vector<BoneTrack> tracks;
    Camera3D camera;
    float currentTime = 0.0f;
    int selectedTrack = -1;
    bool isPlaying = false;
    char loadPath[256] = "model.glb";
    ImGuizmo::OPERATION currentOp = ImGuizmo::ROTATE;

    void Init() {
        InitWindow(1280, 720, "VertexEngine | FULL UNIVERSAL");
        rlImGuiSetup(true);
        camera = {(Vector3){5.0f, 5.0f, 5.0f}, (Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 45.0f, 0};
        SetTargetFPS(60);
    }

    void LoadUniversal(const std::string& path) {
        tinygltf::TinyGLTF loader;
        std::string err, warn;
        if (loader.LoadBinaryFromFile(&model, &err, &warn, path)) {
            tracks.clear();
            selectedTrack = -1;
            for (int i = 0; i < (int)model.nodes.size(); i++) {
                std::string bName = model.nodes[i].name.empty() ? ("Node_" + std::to_string(i)) : model.nodes[i].name;
                tracks.push_back({bName, i, {}});
            }
        }
    }

    void SaveAnimUniversal(const std::string& path) {
        std::ofstream file(path);
        file << "{\n  \"animation\": [\n";
        for (size_t i = 0; i < tracks.size(); i++) {
            if (tracks[i].keys.empty()) continue;
            file << "    { \"bone_idx\": " << tracks[i].nodeIndex << ", \"keys\": [\n";
            for (size_t j = 0; j < tracks[i].keys.size(); j++) {
                auto& k = tracks[i].keys[j];
                file << "      { \"t\":" << k.time 
                     << ", \"p\":[" << k.translation.x << "," << k.translation.y << "," << k.translation.z << "]"
                     << ", \"r\":[" << k.rotation.x << "," << k.rotation.y << "," << k.rotation.z << "," << k.rotation.w << "] }";
                if (j < tracks[i].keys.size() - 1) file << ",";
                file << "\n";
            }
            file << "    ] }" << (i < tracks.size() - 1 ? "," : "") << "\n";
        }
        file << "  ]\n}";
        file.close();
    }

    void DrawNodeRecursive(int nodeIdx, Matrix parentTransform) {
        auto& node = model.nodes[nodeIdx];
        
        Matrix local = MatrixIdentity();
        if (node.scale.size() == 3) local = MatrixMultiply(local, MatrixScale((float)node.scale[0], (float)node.scale[1], (float)node.scale[2]));
        if (node.rotation.size() == 4) local = MatrixMultiply(local, QuaternionToMatrix({(float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2], (float)node.rotation[3]}));
        if (node.translation.size() == 3) local = MatrixMultiply(local, MatrixTranslate((float)node.translation[0], (float)node.translation[1], (float)node.translation[2]));
        
        Matrix global = MatrixMultiply(local, parentTransform);
        Vector3 pos = {global.m12, global.m13, global.m14};
        Vector3 pPos = {parentTransform.m12, parentTransform.m13, parentTransform.m14};

        if (nodeIdx != model.scenes[0].nodes[0]) DrawLine3D(pPos, pos, GRAY);
        
        bool isSelected = (selectedTrack >= 0 && tracks[selectedTrack].nodeIndex == nodeIdx);
        DrawSphere(pos, isSelected ? 0.12f : 0.04f, isSelected ? YELLOW : MAROON);

        if (isSelected) {
            ImGuizmo::SetRect(0, 0, (float)GetScreenWidth(), (float)GetScreenHeight());
            float view[16], proj[16], matrix[16];
            Matrix matView = GetCameraMatrix(camera);
            Matrix matProj = GetCameraProjectionMatrix(&camera, (float)GetScreenWidth() / (float)GetScreenHeight());
            
            memcpy(view, &matView, 16 * sizeof(float));
            memcpy(proj, &matProj, 16 * sizeof(float));
            memcpy(matrix, &global, 16 * sizeof(float));

            if (ImGuizmo::Manipulate(view, proj, currentOp, ImGuizmo::WORLD, matrix)) {
                Matrix newWorld = *(Matrix*)matrix;
                Matrix parentInverse = MatrixInvert(parentTransform);
                Matrix newLocal = MatrixMultiply(newWorld, parentInverse);

                float localArr[16];
                memcpy(localArr, &newLocal, 16 * sizeof(float));
                float t[3], r[3], s[3];
                ImGuizmo::DecomposeMatrixToComponents(localArr, t, r, s);

                node.translation = {(double)t[0], (double)t[1], (double)t[2]};
                Quaternion q = QuaternionFromEuler(r[0] * DEG2RAD, r[1] * DEG2RAD, r[2] * DEG2RAD);
                node.rotation = {(double)q.x, (double)q.y, (double)q.z, (double)q.w};
                node.scale = {(double)s[0], (double)s[1], (double)s[2]};
            }
        }
        for (int child : node.children) DrawNodeRecursive(child, global);
    }

    void Update() {
        if (!ImGuizmo::IsUsing()) UpdateCamera(&camera, CAMERA_ORBITAL);
        
        if (isPlaying) {
            currentTime += GetFrameTime();
            if (currentTime > 10.0f) currentTime = 0.0f;
        }

        if (IsKeyPressed(KEY_K) && selectedTrack != -1) {
            auto& n = model.nodes[tracks[selectedTrack].nodeIndex];
            Vector3 t = (n.translation.size() == 3) ? (Vector3){(float)n.translation[0], (float)n.translation[1], (float)n.translation[2]} : Vector3Zero();
            Quaternion r = (n.rotation.size() == 4) ? (Quaternion){(float)n.rotation[0], (float)n.rotation[1], (float)n.rotation[2], (float)n.rotation[3]} : QuaternionIdentity();
            tracks[selectedTrack].keys.push_back({currentTime, t, r});
            std::sort(tracks[selectedTrack].keys.begin(), tracks[selectedTrack].keys.end(), [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
        }
    }

    void Render() {
        BeginDrawing();
        ClearBackground((Color){30, 30, 30, 255});
        BeginMode3D(camera);
            DrawGrid(20, 1.0f);
            if (!model.scenes.empty() && !model.nodes.empty()) {
                for (int rootNode : model.scenes[0].nodes) {
                    DrawNodeRecursive(rootNode, MatrixIdentity());
                }
            }
        EndMode3D();

        rlImGuiBegin();
        ImGuizmo::BeginFrame();
        ImGui::Begin("Universal VertexEngine");
            ImGui::InputText("Model Path", loadPath, 256);
            if (ImGui::Button("LOAD GLB")) LoadUniversal(loadPath);
            ImGui::SameLine();
            if (ImGui::Button("SAVE ANIM")) SaveAnimUniversal("export.anim");
            
            ImGui::Separator();
            if (ImGui::RadioButton("Translate", currentOp == ImGuizmo::TRANSLATE)) currentOp = ImGuizmo::TRANSLATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", currentOp == ImGuizmo::ROTATE)) currentOp = ImGuizmo::ROTATE;
            
            ImGui::SliderFloat("Time", &currentTime, 0.0f, 10.0f);
            ImGui::Checkbox("Play Preview", &isPlaying);
            
            if (ImGui::Button("Add Keyframe (K)")) {
                // Trigger keyframe logic via UI
            }

            ImGui::Text("Hierarchy:");
            ImGui::BeginChild("NodesList", ImVec2(0, 0), true);
                for(int i = 0; i < (int)tracks.size(); i++) {
                    if (ImGui::Selectable(tracks[i].name.c_str(), selectedTrack == i)) selectedTrack = i;
                }
            ImGui::EndChild();
        ImGui::End();
        rlImGuiEnd();
        EndDrawing();
    }
};

int main() {
    VertexEngine eng;
    eng.Init();
    while (!WindowShouldClose()) {
        eng.Update();
        eng.Render();
    }
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
