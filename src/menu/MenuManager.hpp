#pragma once

#include <memory>
#include <stack>
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
            MenuManager(const MenuManager&) = delete;
            MenuManager(MenuManager&& other) noexcept;
            MenuManager& operator=(const MenuManager&) = delete;
            MenuManager& operator=(MenuManager&& other) noexcept;
            ~MenuManager();

            void AddMenu(const std::string& name, std::unique_ptr<IMenu>&& p_menu);

            //! Request to change the active menu during next update.
            void RequestChangeActiveMenu(const std::string& name);

            //! Request to clear the active menu during next update.
            void RequestClearActiveMenu();

            //! Make any updates for frame.
            void Update();

            //! Set title screen
            void SetTitle(const std::string& name);

            //! Return to previous screen.
            void ReturnToPreviousMenu();

            //! Return to title screen.
            void ReturnToTitle();

        private:

            std::unordered_map<std::string, std::unique_ptr<IMenu>> m_menus;

            IMenu* m_p_active;

            IMenu* m_p_requested;

            IMenu* m_p_title;

            std::stack<IMenu*> m_stack;

            bool m_request_prev {false};
    };

}