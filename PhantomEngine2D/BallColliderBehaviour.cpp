#include "../Scenes/SceneManager.h"
#include <algorithm>
#include "../ConsoleLogger.h"
#include "BallColliderBehaviour.h"
#include "BallMovementBehaviour.h"
#include "ColliderBehaviour.h"
#include <random>

BallColliderBehaviour::BallColliderBehaviour()
{
    SetName("Ball Collider Behaviour");
}

void BallColliderBehaviour::Start()
{
}

bool BallColliderBehaviour::IsCollidingWith(GameObject& other) {
    if (other.GetBehaviour<ColliderBehaviour>() == nullptr) return false;

    SDL_Rect rect1 = { static_cast<int>(GetGameObject()->GetPosition().x), static_cast<int>(GetGameObject()->GetPosition().y), GetGameObject()->GetSize().x, GetGameObject()->GetSize().y };
    SDL_Rect rect2 = { static_cast<int>(other.GetPosition().x), static_cast<int>(other.GetPosition().y), other.GetSize().x, other.GetSize().y };

    return static_cast<bool>(SDL_HasIntersection(&rect1, &rect2) == SDL_TRUE);
}

void BallColliderBehaviour::Update(float deltaTime)
{
    for (GameObject& object : SceneManager::GetActiveScene()->GetGameObjects()) {
        if (object.GetId() != GetGameObject()->GetId())
        {
            if (IsCollidingWith(object))
            {
                //ConsoleLogger::InfoLog("Collided with " + object.GetName());
                if (object.GetName() == "Left Paddle")
                {
                    std::srand(static_cast<unsigned>(std::time(nullptr))); // Seed the random number generator
                    double randomNum = -0.75 + static_cast<double>(std::rand()) / (RAND_MAX / 1.5);
                    GetGameObject()->GetBehaviour<BallMovementBehaviour>()->velocity = ImVec2(1, randomNum);
                }
                else if (object.GetName() == "Right Paddle")
                {
                    std::random_device rd; // Obtain a random seed from the hardware
                    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
                    std::uniform_real_distribution<double> dis(-0.75, 0.75); // Define the range

                    GetGameObject()->GetBehaviour<BallMovementBehaviour>()->velocity = ImVec2(-1, dis(gen));
                }
                else if (object.GetName() == "Top Wall")
                {
                    std::random_device rd; // Obtain a random seed from the hardware
                    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
                    std::uniform_real_distribution<double> dis(0.25, 0.75); // Define the range

                    GetGameObject()->GetBehaviour<BallMovementBehaviour>()->velocity = ImVec2(GetGameObject()->GetBehaviour<BallMovementBehaviour>()->velocity.x, dis(gen));
                }
                else if (object.GetName() == "Bottom Wall")
                {
                    std::random_device rd; // Obtain a random seed from the hardware
                    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
                    std::uniform_real_distribution<double> dis(-0.75, -0.25); // Define the range

                    GetGameObject()->GetBehaviour<BallMovementBehaviour>()->velocity = ImVec2(GetGameObject()->GetBehaviour<BallMovementBehaviour>()->velocity.x, dis(gen));
                }
                else if (object.GetName() == "Left Wall")
                {
                    GetGameObject()->SetPosition(ImVec2(654, 379));
                    GetGameObject()->GetBehaviour<BallMovementBehaviour>()->velocity = ImVec2(1, -0.25);
                }
                else if (object.GetName() == "Right Wall")
                {
                    GetGameObject()->SetPosition(ImVec2(654, 379));
                    GetGameObject()->GetBehaviour<BallMovementBehaviour>()->velocity = ImVec2(1, -0.25);
                }
            }
        }
    }
}

BallColliderBehaviour::~BallColliderBehaviour()
{
}