#include "MenuManager.hpp"
#include "core/Logger.hpp"


Menu::MenuManager::MenuManager()
    : m_p_active(nullptr)
    , m_p_requested(nullptr) {
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
        }

        m_p_active = m_p_requested;

        if (m_p_requested != nullptr) {
            m_p_requested->Activate();
        }
    }
}
