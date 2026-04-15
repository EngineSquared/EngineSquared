#include "Engine.hpp"
#include "Event.hpp"
#include "Object.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace Example {
class PreRender : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

class Render : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

class PostRender : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

using FontContainer = Object::Resource::ResourceManager<sf::Font>;

struct Text {
    sf::Text text;
};

class SfmlPlugin : public Engine::APlugin {
  public:
    using APlugin::APlugin;
    ~SfmlPlugin() = default;

    void Bind(void) override
    {
        RequirePlugins<Event::Plugin>();

        RegisterScheduler<PreRender>();
        RegisterScheduler<Render>();
        RegisterScheduler<PostRender>();

        this->GetCore().SetSchedulerAfter<PreRender, Engine::Scheduler::Update>();
        this->GetCore().SetSchedulerAfter<Render, PreRender>();
        this->GetCore().SetSchedulerAfter<PostRender, Render>();
        this->GetCore().SetSchedulerBefore<PostRender, Engine::Scheduler::Shutdown>();
        // Startup -> (Update -> PreRender -> Render -> PostRender) -> Shutdown

        RegisterResource(FontContainer{});

        RegisterSystems<Engine::Scheduler::Startup>(
            [](Engine::Core &core) {
                const int screenWidth = 800;
                const int screenHeight = 450;
                core.RegisterResource(
                    sf::RenderWindow{sf::VideoMode{{screenWidth, screenHeight}}, "Engine Squared x SFML"});
                core.GetResource<sf::RenderWindow>().setFramerateLimit(60);
            },
            [](Engine::Core &core) {
                core.GetResource<Event::Resource::EventManager>().RegisterCallback<sf::Event>(
                    [&core](const sf::Event &event) {
                        if (event.is<sf::Event::Closed>())
                        {
                            core.Stop();
                        }
                    });
            });

        RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &core) {
            auto &window = core.GetResource<sf::RenderWindow>();
            while (const auto event = window.pollEvent())
            {
                core.GetResource<Event::Resource::EventManager>().PushEvent(event.value());
            }
        });

        RegisterSystems<PreRender>([](Engine::Core &core) {
            if (core.GetResource<sf::RenderWindow>().isOpen() == false)
            {
                core.Stop();
            }
        });

        RegisterSystems<PostRender>([](Engine::Core &core) { core.GetResource<sf::RenderWindow>().display(); });

        RegisterSystems<Engine::Scheduler::Shutdown>([](Engine::Core &) {});
    }
};

class App : public Engine::APlugin {
  public:
    using APlugin::APlugin;
    ~App() = default;

    void Bind(void) override
    {
        RegisterSystems<Engine::Scheduler::Startup>([](Engine::Core &core) {
            auto &window = core.GetResource<sf::RenderWindow>();
            auto &fontContainer = core.GetResource<FontContainer>();
            const std::string fontPath = "examples/Sfml/asset/font/Tomorrow-Regular.ttf";
            const entt::hashed_string fontName = "Tomorrow-Regular";

            if (!fontContainer.Contains(fontName))
            {
                fontContainer.Add(fontName, fontPath);
            }
            Engine::Entity text = core.CreateEntity();

            auto &textComponent = text.AddComponent<Text>(sf::Text{fontContainer.Get(fontName), "Hello, world!", 20});
            textComponent.text.setPosition({400, 225});
            textComponent.text.setOrigin(textComponent.text.getLocalBounds().getCenter());
        });
        RegisterSystems<Render>([](Engine::Core &core) {
            auto &window = core.GetResource<sf::RenderWindow>();

            window.clear(sf::Color::Black);

            core.GetRegistry().view<Text>().each([&window](auto entity, auto &text) { window.draw(text.text); });
        });
    }
};
} // namespace Example

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Example::SfmlPlugin>();

    core.AddPlugins<Example::App>();

    core.Run();

    return 0;
}
