#pragma once

#include <memory>
#include <string>
#include <unordered_map>
namespace Menu {

class IMenu {

    public:
        IMenu() = default;
        IMenu(const IMenu&) = delete;
        IMenu(IMenu&&) = default;
        virtual ~IMenu() = default;
        IMenu& operator=(const IMenu&) = delete;
        IMenu& operator=(IMenu&&) = default;

        virtual void Activate() = 0;
        virtual void Deactivate() = 0;
};

//! Menu manager is responsible for activating and transitioning between various game menus.
class MenuManager {

    public:

        MenuManager();

        void AddMenu(const std::string& name, std::unique_ptr<IMenu>&& p_menu);

        //! Request to change the active menu during next update.
        void RequestChangeActiveMenu(const std::string& name);

        //! Request to clear the active menu during next update.
        void RequestClearActiveMenu();

        //! Make any updates for frame.
        void Update();

    private:

        std::unordered_map<std::string, std::unique_ptr<IMenu>> m_menus;

        IMenu* m_p_active;

        IMenu* m_p_requested;
};

}