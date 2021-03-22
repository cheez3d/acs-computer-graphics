#include "Playground.h"

#include "Balloon.h"
#include "Bow.h"
#include "Heart.h"
#include "Tema3/Random.h"
#include "Shuriken.h"
#include "Transform2D.h"

#include <iostream>

using BowAndArrow::Bow;
using BowAndArrow::Playground;

void Playground::Init() {
    using namespace std::chrono;

    Scene2D::Init();

    auto bow = std::make_unique<Bow>();
    bow->SetPosition({ GetWorld().GetMin().x + 1, 0 });

    this->bow = bow.get();
    AddObject(std::move(bow));

    for (auto i = 0; i < 3; ++i) {
        auto heart = std::make_unique<Heart>();
        heart->SetPosition({
            GetWorld().GetMax().x - 0.6f - 0.8f * i,
            GetWorld().GetMax().y - 0.6f
        });

        hearts.push_back(heart.get());
        AddObject(std::move(heart));
    }

    lastBowInvincibility = steady_clock::now()
                         - duration_cast<steady_clock::duration>(duration<float>(BOW_INVINCIBILITY_DELAY));

    lastBalloonAdd = steady_clock::now()
                   - duration_cast<steady_clock::duration>(duration<float>(BALLOON_ADD_DELAY));

    lastShurikenAdd = steady_clock::now();

    score = 0;

    std::cout << std::endl;
    ShowScore();
}

void Playground::OnInputUpdate(float dt, int mods) {
    using namespace std::chrono;
    using namespace Easing;
    using namespace Transform2D;

    float f = dt / (1.0f / 60);
    auto t = GetTime();

    vec3 color;

    if (!IsPaused()) {
        float c[4];
        glGetFloatv(GL_COLOR_CLEAR_VALUE, c);

        color = { c[0], c[1], c[2] };
        color = Lerp(color, this->color, f * 0.4f);

        glClearColor(color.r, color.g, color.b, 1);
    }

    Scene2D::OnInputUpdate(dt, mods);

    if (!IsPaused()) {
        if (all(greaterThanEqual(color, this->color - vec3 { epsilon<float>() }))) {
            this->color = COLOR_NORMAL;
        }
    }

    if (!bow->HasCollisisons() &&
        duration_cast<duration<float>>(t - lastBowInvincibility).count() >= BOW_INVINCIBILITY_DELAY)
    {
        bow->SetCollisions(true);
    }

    if (duration_cast<duration<float>>(t - lastBalloonAdd).count() >= BALLOON_ADD_DELAY) {
        Balloon::Type type = Random::Int() % (10 - (max)(static_cast<int>(round(8 * score / 1000.0f)), 8))
                           ? Balloon::Type::RED
                           : Balloon::Type::YELLOW;

        auto balloon = std::make_unique<Balloon>(type, Random::Float(0.75f, 1.25f));

        balloon->SetPosition({
            Random::Float(bow->GetPosition().x + 2, GetWorld().GetMax().x - 0.2f),
            Random::Float(GetWorldBoundary().GetMin().y + 0.2f, GetWorld().GetMin().y - 1),
        });

        balloon->SetVelocity({
            Random::Float(-0.01f, 0.01f),
            Random::Float(0.005f, 0.04f),
        });

        balloon->SetPhysics(true);

        AddObject(std::move(balloon));

        lastBalloonAdd = t;
    }

    if (duration_cast<duration<float>>(t - lastShurikenAdd).count() >= SHURIKEN_ADD_DELAY) {
        for (auto i = 0; i < clamp(static_cast<int>(round(score / 200.0f)), 1, 10); ++i) {
            auto shuriken = std::make_unique<Shuriken>();

            shuriken->SetPosition({
                Random::Float(0, GetWorld().GetMax().x - 0.2f),
                Random::Float(GetWorld().GetMin().y + 0.2f, GetWorld().GetMax().y - 0.2f),
            });

            shuriken->SetRotation(Random::Float(0, glm::half_pi<float>()));
            shuriken->SetScale(0);

            shuriken->SetAlive(false);

            AddObject(std::move(shuriken));

            lastShurikenAdd = t;
        }
    }
}

Bow* Playground::GetBow() const {
    return bow;
}

void Playground::ShowDamage() {
    this->color = COLOR_DAMAGE;
}

void Playground::TakeHeart() {
    if (hearts.size() == 0) {
        //Exit();

        std::cout << "Your score was " << score << "." << std::endl;

        return;
    }

    lastBowInvincibility = GetTime();

    auto it = hearts.begin();
    std::advance(it, hearts.size() - 1);

    auto heart = *it;

    heart->SetPhysics(true);
    hearts.erase(it);
}

unsigned Playground::GetScore() const {
    return score;
}

void Playground::ShowScore() const {
    std::cout << "Your score is " << score << ".\r" << std::flush;
}

void Playground::AddScore(int score) {
    if (score < 0) {
        score = -glm::min<int>(glm::abs(score), this->score);
    }

    this->score += score;

    ShowScore();
}
