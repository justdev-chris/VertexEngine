#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include <vector>
#include <string>

// --- VertexEngine Core Types ---
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
    ImGuizmo::OPERATION currentOp = ImGuizmo::ROTATE;

    void Init() {
        InitWindow(1280, 720, "VertexEngine | NATIVE");
        rlImGuiSetup(true);
        camera = {(Vector3){5, 5, 5}, (Vector3){0, 0, 0}, (Vector3){0, 1, 0}, 45.0f, 0};
        SetTargetFPS(60);
    }

    void Load(const std::string& path) {
        tinygltf::TinyGLTF loader; std::string err, warn;
        if (loader.LoadBinaryFromFile(&model, &err, &warn, path)) {
            tracks.clear();
            for (int i = 0; i < (int)model.nodes.size(); i++) {
                if (!model.nodes[i].name.empty()) tracks.push_back({model.nodes[i].name, i, {}});
            }
        }
    }

    // --- The "Magic" Math: Converting Raylib to Gizmo ---
    void DrawBoneGizmo(int nodeIdx, Matrix parentTransform) {
        auto& node = model.nodes[nodeIdx];
        Matrix local = MatrixTranslate(node.translation.size() == 3 ? node.translation[0] : 0, 
                                       node.translation.size() == 3 ? node.translation[1] : 0, 
                                       node.translation.size() == 3 ? node.translation[2] : 0);
        
        if (node.rotation.size() == 4) {
            local = MatrixMultiply(QuaternionToMatrix({(float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2], (float)node.rotation[3]}), local);
        }

        Matrix global = MatrixMultiply(local, parentTransform);
        Vector3 pos = {global.m12, global.m13, global.m14};
        Vector3 pPos = {parentTransform.m12, parentTransform.m13, parentTransform.m14};

        // Draw Skeleton Lines
        if (nodeIdx != model.scenes[0].nodes[0]) DrawLine3D(pPos, pos, GRAY);
        
        // Find if this node is selected in our track list
        bool isSelected = false;
        int trackIdx = -1;
        for(int i=0; i<tracks.size(); i++) if(tracks[i].nodeIndex == nodeIdx) { trackIdx = i; if(i == selectedTrack) isSelected = true; }

        DrawSphere(pos, isSelected ? 0.12f : 0.04f, isSelected ? YELLOW : MAROON);

        if (isSelected) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetRect(0, 0, GetScreenWidth(), GetScreenHeight());
            
            float view[16], proj[16], matrix[16];
            Matrix matView = GetCameraMatrix(camera);
            Matrix matProj = GetCameraProjectionMatrix(&camera, (float)GetScreenWidth()/GetScreenHeight());
            
            // Gizmo needs float arrays
            memcpy(view, &matView, sizeof(float)*16);
            memcpy(proj, &matProj, sizeof(float)*16);
            memcpy(matrix, &global, sizeof(float)*16);

            if (ImGuizmo::Manipulate(view, proj, currentOp, ImGuizmo::WORLD, matrix)) {
                // Decompose and update the node (This is where the posing happens)
                float t[3], r[3], s[3];
                ImGuizmo::DecomposeMatrixToComponents(matrix, t, r, s);
                // Updating Global to Local is complex, but for simple posing:
                node.translation = {(double)t[0], (double)t[1], (double)t[2]};
            }
        }

        for (int child : node.children) DrawBoneGizmo(child, global);
    }

    void Render() {
        if (!ImGuizmo::IsUsing()) UpdateCamera(&camera, CAMERA_ORBITAL);
        
        BeginDrawing();
        ClearBackground((Color){20, 20, 20, 255});
        BeginMode3D(camera);
            DrawGrid(10, 1.0f);
            if (!model.scenes.empty()) DrawBoneGizmo(model.scenes[0].nodes[0], MatrixIdentity());
        EndMode3D();

        rlImGuiBegin();
        ImGuizmo::BeginFrame();
        ImGui::Begin("VertexEngine | Properties");
            if (ImGui::Button("LOAD SCOUT")) Load("scout.glb");
            ImGui::Separator();
            if (ImGui::RadioButton("Move", currentOp == ImGuizmo::TRANSLATE)) currentOp = ImGuizmo::TRANSLATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", currentOp == ImGuizmo::ROTATE)) currentOp = ImGuizmo::ROTATE;
            
            ImGui::SliderFloat("Timeline", &currentTime, 0.0f, 10.0f);
            if (ImGui::Button("KEY BONE") && selectedTrack != -1) {
                // Snapshot current bone pos/rot into tracks[selectedTrack].keys
            }
            
            ImGui::Text("Bones:");
            ImGui::BeginChild("BoneList", ImVec2(0, 0), true);
                for(int i=0; i<tracks.size(); i++) 
                    if(ImGui::Selectable(tracks[i].name.c_str(), selectedTrack == i)) selectedTrack = i;
            ImGui::EndChild();
        ImGui::End();
        rlImGuiEnd();
        EndDrawing();
    }
};

int main() {
    VertexEngine eng; eng.Init();
    while (!WindowShouldClose()) eng.Render();
    return 0;
}
