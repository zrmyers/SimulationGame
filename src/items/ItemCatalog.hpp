#pragma once

#include "Apparel.hpp"
#include "Materials.hpp"
#include "core/Engine.hpp"
#include <string>
#include <unordered_map>

namespace Items {

    //! Set of all items that can be managed in inventory in game.
    class ItemCatalog {

        public:

            //! Constructor for ItemCatalog.
            ItemCatalog();

            //! Loads the catalog from the given filename.
            void Load(Core::Engine& engine, const std::string& filename);

            //! Get material by name
            Material& GetMaterialByIndex(MaterialIndex_t index);
            Material& GetMaterialByName(const std::string& name);

            //! Get apparel by index or name.
            Apparel& GetApparelByIndex(ApparelIndex_t index);
            Apparel& GetApparelByName(const std::string& name);

        private:

            static ApparelLayers ParseApparelLayer(const std::string& layerStr);

            //! The set of all materials that can be used to make equipment in the game.
            std::vector<Material> m_material_list;

            //! Get a material by name.
            std::unordered_map<std::string, Material*> m_material_map;

            //! The set of all apparel that can be equiped in the game.
            std::vector<Apparel> m_apparel_list;

            //! Map of apparel name to apparel
            std::unordered_map<std::string, Apparel*> m_apparel_map;
    };

}