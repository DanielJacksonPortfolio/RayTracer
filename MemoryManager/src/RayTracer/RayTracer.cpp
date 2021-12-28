#include "RayTracer.h"
#include "Sphere.h"
#include "../Externals/tinyxml2.h"
#include "Animation.h"
#include "SphereAnimation.h" 
#include "../StringTools.h" 
#include "Octree.h" 

#include "../MemoryManagement/Heap.h"

#define NOMINMAX
#include <chrono>
#include <filesystem>
#include <thread>

#ifndef __unix
#include <windows.h>
#endif

using namespace tinyxml2;

std::mutex RayTracer::frameMutex;
std::mutex RayTracer::fileMutex;
ThreadManager* RayTracer::threadManager;

void* RayTracer::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{RayTracer} New Called" << std::endl;);
    return ManagedObject::operator new(size, HeapManager::GetDefaultHeap());
}
void RayTracer::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{RayTracer} Delete Called" << std::endl;);
    ::operator delete(pMem);
}
void* RayTracer::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{RayTracer} New[] Called" << std::endl;);
    return ManagedObject::operator new[](size, HeapManager::GetDefaultHeap());
}
void RayTracer::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{RayTracer} Delete[] Called" << std::endl;);
    ::operator delete[](pMem);
}

RayTracer::RayTracer()
{
    VERBOSE_OUTPUT(std::cout << "{RayTracer} Constructor : " << this-sizeof(Header) << std::endl;);
	threadManager = new ThreadManager();
}

RayTracer::~RayTracer()
{
    VERBOSE_OUTPUT(std::cout << "{RayTracer} Deconstructor" << std::endl;);
	delete threadManager;
}

float RayTracer::Mix(const float& a, const float& b, const float& mix)
{
    return b * mix + a * (1 - mix);
}

//[comment]
// This is the main trace function. It takes a ray as argument (defined by its origin
// and direction). We test if this ray intersects any of the geometry in the scene.
// If the ray intersects an object, we compute the intersection point, the normal
// at the intersection point, and shade this point using this information.
// Shading depends on the surface property (is it transparent, reflective, diffuse).
// The function returns a color for the ray. If the ray intersects an object that
// is the color of the object at the intersection point, otherwise it returns
// the background color.
//[/comment]
Vector3f RayTracer::Trace(
	const Vector3f& rayorig,
	const Vector3f& raydir,
	std::unordered_set<Sphere>& spheres,
	const int& depth)
{
	//if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
	float tnear = INFINITY;
	const Sphere* spherePtr = nullptr;
	// find intersection of this ray with the sphere in the scene
	for (auto sphere : spheres)
	{
		float t0 = INFINITY, t1 = INFINITY;
		if (sphere.Intersect(rayorig, raydir, t0, t1))
		{
			if (t0 < 0)
			{
				t0 = t1;
			}

			if (t0 < tnear)
			{
				tnear = t0;
				spherePtr = &sphere;
			}
		}
	}
	// if there's no intersection return black or background color
	if (!spherePtr)
	{
		return Vector3f(2);
	}

	Vector3f surfaceColor = 0; // color of the ray/surface of the object intersected by the ray
	Vector3f phit = rayorig + raydir * tnear; // point of intersection
	Vector3f nhit = phit - spherePtr->center; // normal at the intersection point
	nhit.normalize(); // normalize normal direction
					  // If the normal and the view direction are not opposite to each other
					  // reverse the normal direction. That also means we are inside the sphere so set
					  // the inside bool to true. Finally reverse the sign of IdotN which we want
					  // positive.
	float bias = 1e-4f; // add some bias to the point from which we will be tracing
	bool inside = false;
	if (raydir.dot(nhit) > 0)
	{
		nhit = -nhit;
		inside = true;
	}
	if ((spherePtr->transparency > 0 || spherePtr->reflection > 0) && depth < MAX_RAY_DEPTH)
	{
		float facingratio = -raydir.dot(nhit);
		// change the mix value to tweak the effect
		float fresneleffect = Mix(pow(1.0f - facingratio, 3.0f), 1.0f, 0.1f);
		// compute reflection direction (not need to normalize because all vectors
		// are already normalized)
		Vector3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
		refldir.normalize();
		Vector3f reflection = Trace(phit + nhit * bias, refldir, spheres, depth + 1);
		Vector3f refraction = 0;
		// if the sphere is also transparent compute refraction ray (transmission)
		if (spherePtr->transparency)
		{
			float ior = 1.1f, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
			float cosi = -nhit.dot(raydir);
			float k = 1 - eta * eta * (1 - cosi * cosi);
			Vector3f refrdir = raydir * eta + nhit * (eta * cosi - sqrt(k));
			refrdir.normalize();
			refraction = Trace(phit - nhit * bias, refrdir, spheres, depth + 1);
		}
		// the result is a mix of reflection and refraction (if the sphere is transparent)
		surfaceColor = (
			reflection * fresneleffect +
			refraction * (1 - fresneleffect) * spherePtr->transparency) * spherePtr->surfaceColor;
	}
	else
	{
		// it's a diffuse object, no need to raytrace any further
		for (auto sphere : spheres)
		{
			if (sphere.emissionColor.x > 0)
			{
				// this is a light
				Vector3f transmission = 1;
				Vector3f lightDirection = sphere.center - phit;
				lightDirection.normalize();
				for (auto sphere2 : spheres)
				{
					if (sphere != sphere2)
					{
						float t0, t1;
						if (sphere.Intersect(phit + nhit * bias, lightDirection, t0, t1))
						{
							transmission = 0;
							break;
						}
					}
				}
				surfaceColor += spherePtr->surfaceColor * transmission *
					std::max(float(0), nhit.dot(lightDirection)) * sphere.emissionColor;
			}
		}
	}

	return surfaceColor + spherePtr->emissionColor;
}

//[comment]
// Main rendering function. We compute a camera ray for each pixel of the image
// trace it and return a color. If the ray hits a sphere, we return the color of the
// sphere at the intersection point, else we return the background color.
//[/comment]
void RayTracer::Render(std::unordered_set<Sphere>& spheres, int iteration, unsigned width, unsigned height, std::string filepath)
{
	Vector3f* image = new Vector3f[width * height], * pixel = image;
	float invWidth = 1 / float(width), invHeight = 1 / float(height);
	float fov = 30.0f, aspectratio = width / float(height);
	float angle = tan(M_PI * 0.5f * fov / 180.0f);
	float angleXAspect = angle * aspectratio;

	Vector3f rayOrigin = Vector3f(0);

	Octree* octree = nullptr;
	if (USE_OCTREE)
	{
		octree = Octree::ConstructTree(Vector3f(100.0f,100.0f,100.0f), Vector3f(-50.0f,-50.0f,-50.0f),spheres);
	}
	// Trace rays
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x, ++pixel)
		{
			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angleXAspect;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Vector3f raydir(xx, yy, -1);
			raydir.normalize();

			if (USE_OCTREE)
			{
				spheres = octree->Intersect(rayOrigin, raydir);
			}

			*pixel = Trace(rayOrigin, raydir, spheres, 0);
		}
	}
	if (USE_OCTREE)
	{
		delete octree;
	}
	std::string outputFolder = ROOT_DIRECTORY + "/Images/" + StringTools::RemoveExtension(StringTools::GetFileFromPath(filepath));
	
	std::error_code error;
	bool directoryExists = std::filesystem::exists(outputFolder, error);
	if (!directoryExists)
	{
		directoryExists = std::filesystem::create_directory(outputFolder, error);
		if (error.value() != 0)
		{
			frameMutex.lock();
			std::cout << error.message() << std::endl;
			frameMutex.unlock();
		}
	}

	// Save result to a PPM image (keep these flags if you compile under Windows)
	if (directoryExists)
	{
		fileMutex.lock();
		FILE* fp = nullptr;

#ifdef __unix
		fp = fopen((outputFolder + "/spheres" + std::to_string(iteration) + ".ppm").c_str(), "wb+");
#else
		fopen_s(&fp, (outputFolder + "/spheres" + std::to_string(iteration) + ".ppm").c_str(), "wb+");
#endif

		if (fp != nullptr)
		{
			std::string metadata = std::string("P6\n" + std::to_string(width) + " " + std::to_string(height) + "\n255\n");
			char color[3];
			fwrite(metadata.c_str(), 1, metadata.size(), fp);
			for (unsigned i = 0; i < height * width; ++i)
			{
				color[0] = static_cast<char>(std::min(1.0f, image[i].x) * 255);
				color[1] = static_cast<char>(std::min(1.0f, image[i].y) * 255);
				color[2] = static_cast<char>(std::min(1.0f, image[i].z) * 255);

				fwrite(&color, 1, 3, fp);
			}
			fclose(fp);
			fileMutex.unlock();
			frameMutex.lock();
			std::cout << "Rendered and saved " << outputFolder + "/spheres" + std::to_string(iteration) + ".ppm" << std::endl;
			frameMutex.unlock();
		}
	}
	else
	{
		frameMutex.lock();
		std::cout << "Failed to Find/Create Directory: " << outputFolder.c_str() << std::endl;
		frameMutex.unlock();
	}

	delete[] image;
}

Animation* RayTracer::LoadScene(std::string filepath)
{
	this->filepath = filepath;

	Animation* animation = new Animation();
	tinyxml2::XMLDocument sceneDocument;

	XMLError eResult = sceneDocument.LoadFile(filepath.c_str());

	if (eResult != XML_SUCCESS)
	{
		std::cout << "Scene File failed to open at location: " << filepath << std::endl;
		return animation;
	}

	try
	{
		XMLNode* pRoot = sceneDocument.FirstChild();
		if (pRoot == nullptr)
		{
			std::cout << "Failed to Load Scene: No root found" << std::endl;
		}
		XMLElement* spheresElement = pRoot->FirstChildElement("Spheres");
		if (spheresElement == nullptr)
		{
			std::cout << "Failed to Load Scene: No spheres found" << std::endl;
		}
		XMLElement* sphereElement = spheresElement->FirstChildElement("Sphere");
		while (sphereElement != nullptr)
		{
			SphereAnimation* sphereAnim = new SphereAnimation();
			XMLElement* keyFrameElement = sphereElement->FirstChildElement("KeyFrame");
			while (keyFrameElement != nullptr)
			{
				float radius = 0.0f;
				float reflection = 0.0f;
				float transparency = 0.0f;
				int frameNum = 0;
				Vector3f centre = Vector3f();
				Vector3f surfaceColor = Vector3f();
				Vector3f emissionColor = Vector3f();

				if (keyFrameElement->QueryIntAttribute("FrameNum", &frameNum) != XMLError::XML_SUCCESS)
				{
					std::cout << "Error Loading FrameNum" << std::endl;
				}
				if (keyFrameElement->QueryFloatAttribute("Radius", &radius) != XMLError::XML_SUCCESS)
				{
					std::cout << "Error Loading Radius" << std::endl;
				}
				if (keyFrameElement->QueryFloatAttribute("Reflection", &reflection) != XMLError::XML_SUCCESS)
				{
					std::cout << "Missing Reflection" << std::endl;
				}
				if (keyFrameElement->QueryFloatAttribute("Transparency", &transparency) != XMLError::XML_SUCCESS)
				{
					std::cout << "Missing Transparency" << std::endl;
				}

				XMLElement* centreElement = keyFrameElement->FirstChildElement("Centre");
				if (centreElement != nullptr)
				{
					if (centreElement->QueryFloatAttribute("X", &centre.x) != XMLError::XML_SUCCESS)
					{
						std::cout << "Error Loading Centre X" << std::endl;
					}
					if (centreElement->QueryFloatAttribute("Y", &centre.y) != XMLError::XML_SUCCESS)
					{
						std::cout << "Error Loading Centre Y" << std::endl;
					}
					if (centreElement->QueryFloatAttribute("Z", &centre.z) != XMLError::XML_SUCCESS)
					{
						std::cout << "Error Loading Centre Z" << std::endl;
					}
				}
				else
				{
					std::cout << "Error Loading Centre" << std::endl;
				}

				XMLElement* surfaceColorElement = keyFrameElement->FirstChildElement("SurfaceColor");
				if (surfaceColorElement != nullptr)
				{
					if (surfaceColorElement->QueryFloatAttribute("X", &surfaceColor.x) != XMLError::XML_SUCCESS)
					{
						std::cout << "Error Loading SurfaceColor X" << std::endl;
					}
					if (surfaceColorElement->QueryFloatAttribute("Y", &surfaceColor.y) != XMLError::XML_SUCCESS)
					{
						std::cout << "Error Loading SurfaceColor Y" << std::endl;
					}
					if (surfaceColorElement->QueryFloatAttribute("Z", &surfaceColor.z) != XMLError::XML_SUCCESS)
					{
						std::cout << "Error Loading SurfaceColor Z" << std::endl;
					}
				}
				else
				{
					std::cout << "Error Loading SurfaceColor" << std::endl;
				}

				XMLElement* emissionColorElement = keyFrameElement->FirstChildElement("EmissionColor");
				if (emissionColorElement != nullptr)
				{
					if (emissionColorElement->QueryFloatAttribute("X", &emissionColor.x) != XMLError::XML_SUCCESS)
					{
						std::cout << "Error Loading EmissionColor X" << std::endl;
					}
					if (emissionColorElement->QueryFloatAttribute("Y", &emissionColor.y) != XMLError::XML_SUCCESS)
					{
						std::cout << "Error Loading EmissionColor Y" << std::endl;
					}
					if (emissionColorElement->QueryFloatAttribute("Z", &emissionColor.z) != XMLError::XML_SUCCESS)
					{
						std::cout << "Error Loading EmissionColor Z" << std::endl;
					}
				}
				else
				{
					std::cout << "Missing EmissionColor" << std::endl;
				}
				Sphere* keyFrame = new Sphere(frameNum, centre, radius, surfaceColor, reflection, transparency, emissionColor);

				sphereAnim->AddNewKeyFrame(keyFrame);
				keyFrameElement = keyFrameElement->NextSiblingElement("KeyFrame");
			}
			animation->AddNewSphereAnimation(sphereAnim);
			sphereElement = sphereElement->NextSiblingElement("Sphere");
		}
	}
	catch (std::exception e)
	{
		std::cout << "Failed to Load Scene: Exception - " << e.what() << std::endl;
	}

	return animation;

}

void RayTracer::RenderFrame(Animation* animation, unsigned width, unsigned height, int frameNum, std::string filepath)
{
	std::unordered_set<Sphere> spheres = std::unordered_set<Sphere>();
	std::vector<SphereAnimation*>* sphereAnims = animation->GetSpheres();
	for (auto anim : *sphereAnims)
	{
		spheres.insert(anim->GetSphereAtFrame(frameNum));
	}

	Render(spheres, frameNum, width, height, filepath);
}

void RayTracer::AnimatedRender(std::string filepath, unsigned width, unsigned height)
{
	std::cout << "Render Begin, Filepath: " << filepath << std::endl;

	Animation* animation = LoadScene(filepath);

	auto start = std::chrono::steady_clock::now();

	int lastFrame = animation->GetLastFrame();
	if (USE_THREADING)
	{
		for (int currentFrame = 0; currentFrame <= lastFrame; ++currentFrame)
		{
			threadManager->StartNewRenderFrameThread(0, RenderFrame, animation, width, height, currentFrame, filepath);
		}
		threadManager->JoinAll();
	}
	else
	{
		for (int currentFrame = 0; currentFrame <= lastFrame; ++currentFrame)
		{
			RenderFrame(animation, width, height, currentFrame, filepath);
		}
	}
	auto finish = std::chrono::steady_clock::now();
	double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
	std::cout << "Render Complete, Duration: " << elapsed_seconds << " Seconds" << std::endl;

	delete animation;

	GenerateVideo(filepath);
}

void RayTracer::GenerateVideo(std::string filepath)
{
#ifndef __unix
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	std::string outputFolder = "./Images/" + StringTools::RemoveExtension(StringTools::GetFileFromPath(filepath));
	std::string cmdStr = "ffmpeg -framerate 25 -y -i " + outputFolder + "/spheres%d.ppm -vcodec mpeg4 " + "./Images/Videos/" + StringTools::RemoveExtension(StringTools::GetFileFromPath(filepath)) + ".mp4";
	const char* commandLine = (cmdStr).c_str();
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	
	ZeroMemory(&pi, sizeof(pi));

	CreateProcessA("./Images/ffmpeg.exe",
		const_cast<char*>(commandLine),
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi
	);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	std::cout << "FFMPEG Compilation Complete" << std::endl;
#endif
}