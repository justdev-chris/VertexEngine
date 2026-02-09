#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include <vector>
#include <string>
#include <map>

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
    tinygltf::TinyGLTF loader;
    std::vector<BoneTrack> tracks;
    bool modelLoaded = false;
    float currentTime = 0.0f;
    bool isPlaying = false;
    int selectedTrack = -1;
    Camera3D camera;

    void Init() {
        InitWindow(1280, 720, "VertexEngine | C++ Native");
        rlImGuiSetup(true);
        SetTargetFPS(60);
        camera = { (Vector3){10, 10, 10}, (Vector3){0, 0, 0}, (Vector3){0, 1, 0}, 45.0f, 0 };
    }

    void Load(const std::string& path) {
        std::string err, warn;
        if (loader.LoadBinaryFromFile(&model, &err, &warn, path)) {
            modelLoaded = true;
            tracks.clear();
            for (int i = 0; i < (int)model.nodes.size(); i++) {
                if (!model.nodes[i].name.empty()) {
                    tracks.push_back({ model.nodes[i].name, i, {} });
                }
            }
        }
    }

    // --- The "SFM" Interpolation Math ---
    Quaternion SampleRotation(int trackIdx) {
        auto& keys = tracks[trackIdx].keys;
        if (keys.empty()) return QuaternionIdentity();
        if (currentTime <= keys[0].time) return keys[0].rotation;
        if (currentTime >= keys.back().time) return keys.back().rotation;

        for (size_t i = 0; i < keys.size() - 1; i++) {
            if (currentTime >= keys[i].time && currentTime <= keys[i+1].time) {
                float t = (currentTime - keys[i].time) / (keys[i+1].time - keys[i].time);
                return QuaternionSlerp(keys[i].rotation, keys[i+1].rotation, t);
            }
        }
        return QuaternionIdentity();
    }

    void DrawSkeleton(int nodeIdx, Matrix parentTransform) {
        auto& node = model.nodes[nodeIdx];
        
        // Calculate local transform
        Matrix local = MatrixIdentity();
        if (node.rotation.size() == 4) {
            Quaternion q = {(float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2], (float)node.rotation[3]};
            local = QuaternionToMatrix(q);
        }
        if (node.translation.size() == 3) {
            local = MatrixMultiply(local, MatrixTranslate((float)node.translation[0], (float)node.translation[1], (float)node.translation[2]));
        }

        Matrix global = MatrixMultiply(local, parentTransform);
        Vector3 pos = { global.m12, global.m13, global.m14 }; // Extract world position
        Vector3 parentPos = { parentTransform.m12, parentTransform.m13, parentTransform.m14 };

        // Highlight if selected
        bool isSelected = false;
        for(int i=0; i<tracks.size(); i++) if(tracks[i].nodeIndex == nodeIdx && i == selectedTrack) isSelected = true;

        if (nodeIdx != model.scenes[0].nodes[0]) DrawLine3D(parentPos, pos, isSelected ? YELLOW : GRAY);
        DrawSphere(pos, isSelected ? 0.15f : 0.05f, isSelected ? GOLD : MAROON);

        for (int child : node.children) DrawSkeleton(child, global);
    }

    void Update() {
        UpdateCamera(&camera, CAMERA_ORBITAL);
        if (isPlaying) {
            currentTime += GetFrameTime();
            if (currentTime > 5.0f) currentTime = 0.0f;
            
            // Apply animated rotations back to GLTF nodes
            for (int i = 0; i < (int)tracks.size(); i++) {
                if (!tracks[i].keys.empty()) {
                    Quaternion q = SampleRotation(i);
                    model.nodes[tracks[i].nodeIndex].rotation = {(double)q.x, (double)q.y, (double)q.z, (double)q.w};
                }
            }
        }
    }

    void RenderUI() {
        rlImGuiBegin();
        ImGui::Begin("VertexEngine Editor");
            if (ImGui::Button("Load scout.glb")) Load("scout.glb");
            ImGui::Checkbox("Play Animation", &isPlaying);
            ImGui::SliderFloat("Timeline", &currentTime, 0.0f, 5.0f);
            
            ImGui::Separator();
            ImGui::Text("Bones Hierarchy");
            ImGui::BeginChild("List", ImVec2(0, 200), true);
                for (int i = 0; i < (int)tracks.size(); i++) {
                    if (ImGui::Selectable(tracks[i].name.c_str(), selectedTrack == i)) selectedTrack = i;
                }
            ImGui::EndChild();

            if (ImGui::Button("KEY BONE") && selectedTrack != -1) {
                auto& n = model.nodes[tracks[selectedTrack].nodeIndex];
                Quaternion q = (n.rotation.size() == 4) ? (Quaternion){(float)n.rotation[0],(float)n.rotation[1],(float)n.rotation[2],(float)n.rotation[3]} : QuaternionIdentity();
                tracks[selectedTrack].keys.push_back({currentTime, Vector3Zero(), q});
            }
        ImGui::End();
        rlImGuiEnd();
    }
};

int main() {
    VertexEngine engine;
    engine.Init();
    while (!WindowShouldClose()) {
        engine.Update();
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode3D(engine.camera);
                DrawGrid(10, 1.0f);
                if (engine.modelLoaded) engine.DrawSkeleton(engine.model.scenes[0].nodes[0], MatrixIdentity());
            EndMode3D();
            engine.RenderUI();
        EndDrawing();
    }
    return 0;
}
