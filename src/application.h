#include <iostream>
#include "imgui.h"

class Application
{
    public:
        void renderUI()
        {
            ImGui::Begin("raytracing");
            ImGui::SeparatorText("System");
            ImGui::Checkbox(": Use Threading", &useThreading);

            ImGui::SeparatorText("ImagePlane");
            ImGui::InputDouble(": Aspect Ratio", &aspectRatio, 0.01f, 1.0f, "%.8f");
            ImGui::InputInt(": Image Plane Width", &imagePlaneWidth);
            
            ImGui::SeparatorText("Samples");
            ImGui::InputDouble(": Samples Per Pixel", &samplesPerPixel, 0.01f, 1.0f, "%.8f");
            ImGui::InputDouble(": Max Depth", &maxDepth, 0.01f, 1.0f, "%.8f");

            ImGui::SeparatorText("Camera Transformations");
            static float cameraLookFrom[3] = {0.0f, 0.0f, 0.0f};
            static float cameraLookAt[3] = {0.0f, 0.0f, 0.0f};
            static float cameraViewUp[3] = {0.0f, 0.0f, 0.0f};
            ImGui::InputDouble(": Camera FOV", &cameraFov, 0.01f, 1.0f, "%.8f");
            ImGui::InputFloat3(": Camera Look From", cameraLookFrom);
            ImGui::InputFloat3(": Camera Look At", cameraLookAt);
            ImGui::InputFloat3(": Camera View Up", cameraViewUp);
            
            ImGui::SeparatorText("Camera Lens");
            static float cameraDefocusAngle = 0.0f;
            static float cameraFocusDistance = 0.0f;
            ImGui::InputFloat(": Camera Defocus Angle", &cameraDefocusAngle, 0.01f, 1.0f, "%.3f");
            ImGui::InputFloat(": Camera Focus Distance", &cameraFocusDistance, 0.01f, 1.0f, "%.3f");
            
            
            ImGui::SeparatorText("Export");
            static char filename[128] = "output.exr";
            ImGui::InputText(": Filename", filename, IM_ARRAYSIZE(filename));
            if (ImGui::Button("Start Render!"))
            {
                startRayTracer();
            }
            ImGui::End();
            ImGui::ShowDemoWindow();
        }
    private:
        void startRayTracer()
        {
            if(!renderInProgress)
            {
                renderInProgress = true;
                std::cout << "Render Started..." << std::endl;
            }
        }
        bool renderInProgress = false;

        // System Config
        bool useThreading = true;
        // ImagePlane Config
        double aspectRatio = 16.0 / 9.0;
        int imagePlaneWidth = 1200;
        // Samples Config
        double samplesPerPixel = 50;
        double maxDepth = 10;
        // Camera Transformation
        double cameraFov = 90;
        static float cameraLookFrom[3];
        static float cameraLookAt[3];
        // Export 
        static char filename[128];
};