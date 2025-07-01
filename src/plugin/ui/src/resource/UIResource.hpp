#pragma once

#include "Engine.hpp"
#include "EventListener.hpp"
#include "RenderInterface.hpp"
#include "SystemInterface.hpp"

namespace ES::Plugin::UI::Resource {
class UIResource {
  private:
    Rml::Context *_context;
    Rml::ElementDocument *_document;
    std::unique_ptr<ES::Plugin::UI::Utils::SystemInterface> _systemInterface;
    std::unique_ptr<ES::Plugin::UI::Utils::RenderInterface> _renderInterface;
    std::unique_ptr<ES::Plugin::UI::Utils::EventListener> _event;

  public:
    /**
     * Constructor.
     */
    UIResource() = default;

    UIResource(const UIResource &) = delete;
    UIResource &operator=(const UIResource &) = delete;
    UIResource(UIResource &&) noexcept = default;
    UIResource &operator=(UIResource &&) noexcept = default;

    /**
     * Destructor.
     */
    ~UIResource() = default;

    enum class TransformType {
        Rotate,
        TranslateX,
        TranslateY,
    };

    struct TransformParam {
        TransformType type;
        float value;
    };

    /**
     * @brief Initialize the backend
     *
     * @param core The core of the engine.
     *
     * @return void
     */
    void Init(ES::Engine::Core &core);

    /**
     * @brief Bind the RmlUi event callback to the input manager
     *
     * @return void
     */
    void BindEventCallback();

    /**
     * @brief Update the mouse position event
     *
     * @param core The core of the engine.
     *
     * @return void
     */
    void UpdateMouseMoveEvent(ES::Engine::Core &core);

    /**
     * @brief Destroy the backend and clear its resources
     *
     * @return void
     */
    void Destroy();

    /**
     * @brief Update the backend
     *
     * This function needs to be called before the Render function
     *
     * @param core The core of the engine.
     *
     * @return void
     */
    void Update(ES::Engine::Core &core);

    /**
     * @brief Render the backend context
     *
     * This function needs to be called after the Update function
     *
     * @return void
     */
    void Render();

    /**
     * @brief Add a new font file to the resource
     *
     * Fonts need to be set before rendering any texts
     *
     * @param fontPath The location of the font file.
     *
     * @return void
     */
    void SetFont(const std::string &fontPath);

    /**
     * @brief Intialize the RML document
     *
     * Fonts need to be set before rendering any texts
     *
     * @param docPath The location of the RML document file.
     *
     * @return void
     */
    void InitDocument(const std::string &docPath);

    /**
     * @brief Modify in the loaded RML document `childId` node by `content`
     *
     * A valid document must exist before trying to modify its content.
     * The `content` parameter will overide the entire child node in specific context.
     * This is safe to use for simple texts.
     *
     * @param childId The node id to modify
     * @param content The content to replace the node id by
     *
     * @return void
     */
    void UpdateInnerContent(const std::string &childId, const std::string &content);

    /**
     * @brief Sets a local property override on the element to a pre-parsed value.
     *
     * This is useful to add animations efficiently to elements
     *
     * @param childId The node id to modify
     * @param transforms vector of transform params to apply
     *
     * @return void
     */
    void SetTransformProperty(const std::string &childId, const std::vector<TransformParam> &transforms);

    /**
     * @brief Attach the event listener handlers
     *
     * @param childId The node id to modify
     *
     * @return void
     */
    void AttachEventHandlers(const std::string &elementId, const std::string &eventType,
                             ES::Plugin::UI::Utils::EventListener::EventCallback callback);

    /**
     * @brief Check if the UI plugin is ready
     *
     * @return bool false if not ready to render, true otherwise
     */
    bool IsReady() const;

    /**
     * @brief Get the element value
     *
     * @param elementId The node id to modify
     *
     * @return bool false if not ready to render, true otherwise
     */
    std::string GetValue(const std::string &elementId) const;
};
} // namespace ES::Plugin::UI::Resource
