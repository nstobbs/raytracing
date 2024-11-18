#include <iostream>
#include "imgui.h"

// Raytracer
#include "raytracer/rtweekend.h"

#include "raytracer/camera.h"
#include "raytracer/hittable.h"
#include "raytracer/hittable_list.h"
#include "raytracer/material.h"
#include "raytracer/sphere.h"

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
            static int cameraLookFrom[3] = {13, 2, 3};
            static int cameraLookAt[3] = {0, 0, 0};
            static int cameraViewUp[3] = {0, 1, 0};
            ImGui::InputDouble(": Camera FOV", &cameraFov, 0.01f, 1.0f, "%.8f");
            ImGui::InputInt3(": Camera Look From", cameraLookFrom);
            ImGui::InputInt3(": Camera Look At", cameraLookAt);
            ImGui::InputInt3(": Camera View Up", cameraViewUp);
            
            ImGui::SeparatorText("Camera Lens");
            static float cameraDefocusAngle = 0.6f;
            static float cameraFocusDistance = 10.0f;
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
        bool renderInProgress = false;
        // System Config
        bool useThreading = true;
        // ImagePlane Config
        double aspectRatio = 16.0 / 9.0;
        int imagePlaneWidth = 1200;
        // Samples Config
        double samplesPerPixel = 500;
        double maxDepth = 50;
        // Camera Transformation
        double cameraFov = 20;
        static int cameraLookFrom[3];
        static int cameraLookAt[3];
        static int cameraViewUp[3];
        // Camera Lens
        static float cameraDefocusAngle;
        static float cameraFocusDistance;
        // Export 
        static char filename[128];

        void startRayTracer()
        {
            if(!renderInProgress)
            {
                renderInProgress = true;
                std::cout << "Render Started..." << std::endl;

                hittable_list world;

                auto groundMaterial = make_shared<diffuse>(color(0.5,0.5,0.5));
                world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, groundMaterial));

                for (int a = -11; a < 11; a++)
                {
                    for (int b = -11; b < 11; b++)
                    {
                        auto chooseMat = randomDouble();
                        point3 center(a + 0.9*randomDouble(), 0.2, b + 0.9*randomDouble());
                        if ((center - point3(4, 0.2, 0)).length() > 0.9)
                        {
                            shared_ptr<material> sphereMaterial;

                            if (chooseMat < 0.8)
                            {
                                auto albedo = color::random() * color::random();
                                sphereMaterial = make_shared<diffuse>(albedo);
                                world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
                            } else if (chooseMat < 0.95){
                                auto albedo = color::random();
                                auto fuzz = randomDouble(0, 0.5);
                                sphereMaterial = make_shared<metal>(albedo, fuzz);
                                world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
                            } else {
                                sphereMaterial = make_shared<glass>(1.5);
                                world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
                            }
                        }
                    }
                }

                auto material1 = make_shared<glass>(1.5);
                world.add(make_shared<sphere>(point3(0,1,0), 1.0, material1));

                auto material2 = make_shared<diffuse>(color(0.4, 0.2, 0.1));
                world.add(make_shared<sphere>(point3(-4,1,0), 1.0, material2));

                auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
                world.add(make_shared<sphere>(point3(4,1,0), 1.0, material3));

                camera cam;
                cam.aspectRatio = aspectRatio;
                cam.imagePlaneWidth = imagePlaneWidth;
                cam.samplesPerPixel = samplesPerPixel;
                cam.maxDepth = maxDepth;
                
                cam.viewFov = cameraFov;
                cam.lookFrom = point3(cameraLookFrom[0], cameraLookFrom[1], cameraLookFrom[2]);
                cam.lookAt = point3(cameraLookAt[0], cameraLookAt[1], cameraLookAt[2]);
                cam.vUp = vec3(cameraViewUp[0],cameraViewUp[1],cameraViewUp[2]);

                cam.defocusAngle = cameraDefocusAngle;
                cam.focusDist = cameraFocusDistance;

                if (useThreading)
                {
                    cam.parallelRender(world);
                } else {
                    cam.render(world);
                };
                renderInProgress = false;
            }
        };
};