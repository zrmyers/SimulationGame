#include "MenuManager.hpp"
#include "core/Logger.hpp"
#include <utility>


Menu::MenuManager::MenuManager()
    : m_p_active(nullptr)
    , m_p_requested(nullptr)
    , m_p_title(nullptr) {
}

Menu::MenuManager::MenuManager(MenuManager&& other) noexcept
    : m_p_active(other.m_p_active)
    , m_p_requested(other.m_p_requested)
    , m_p_title(other.m_p_title) {
    other.m_p_active = nullptr;
    other.m_p_requested = nullptr;
    other.m_p_title = nullptr;
    std::swap(m_menus, other.m_menus);
    std::swap(m_stack, other.m_stack);
}


Menu::MenuManager& Menu::MenuManager::operator=(MenuManager&& other) noexcept {
    std::swap(m_menus, other.m_menus);
    std::swap(m_p_active, other.m_p_active);
    std::swap(m_p_requested, other.m_p_requested);
    std::swap(m_p_title, other.m_p_title);
    std::swap(m_stack, other.m_stack);
    return *this;
}


Menu::MenuManager::~MenuManager() {
    if (m_p_active != nullptr) {
        m_p_active->Deactivate();
    }
}

void Menu::MenuManager::AddMenu(const std::string& name, std::unique_ptr<IMenu>&& p_menu) {

    m_menus[name] = std::move(p_menu);
}

void Menu::MenuManager::RequestChangeActiveMenu(const std::string& name) {
    auto menuIter = m_menus.find(name);
    if (menuIter != m_menus.end()) {

        m_p_requested = menuIter->second.get();
    }
    else {
        Core::Logger::Error("MenuManager: Could not transition to " + name + ". Not Found!");
    }
}

void Menu::MenuManager::RequestClearActiveMenu() {

    m_p_requested = nullptr;
}

void Menu::MenuManager::Update() {

    if (m_p_active != m_p_requested) {

        if (m_p_active != nullptr) {
            m_p_active->Deactivate();

            if (!m_request_prev) {
                m_stack.push(m_p_active);
            }
        }

        m_p_active = m_p_requested;

        if (m_p_requested != nullptr) {
            m_p_requested->Activate();
        }

        m_request_prev = false;
    }
}

void Menu::MenuManager::SetTitle(const std::string& name) {

    m_p_title = m_menus.at(name).get();
}

void Menu::MenuManager::ReturnToPreviousMenu() {
    if (!m_stack.empty()) {
        m_p_requested = m_stack.top();
        m_stack.pop();
        m_request_prev = true;
    } else {

        m_p_requested = m_p_title;
    }
}

void Menu::MenuManager::ReturnToTitle() {

    m_stack = {};

    m_p_requested = m_p_title;
}
