#include "objects.hpp"
#include "globals.hpp"
#include <fancy.hpp>
#include <functional>
#include <optional>

namespace Soundux::Objects
{
    Tab Data::addTab(Tab tab)
    {
        tab.id = tabs.size();
        tabs.emplace_back(tab);
        std::unique_lock lock(Globals::gSoundsMutex);
        std::unique_lock favLock(Globals::gFavoritesMutex);

        for (auto &sound : tabs.back().sounds)
        {
            Globals::gSounds.insert({sound.id, sound});
            if (sound.isFavorite)
            {
                Globals::gFavorites.insert({sound.id, sound});
            }
        }

        return tabs.back();
    }
    void Data::removeTabById(const std::uint32_t &index)
    {
        std::unique_lock lock(Globals::gSoundsMutex);
        std::unique_lock favLock(Globals::gFavoritesMutex);
        if (tabs.size() > index)
        {
            auto &tab = tabs.at(index);
            for (auto &sound : tab.sounds)
            {
                Globals::gSounds.erase(sound.id);
                if (sound.isFavorite)
                {
                    Globals::gFavorites.erase(sound.id);
                }
            }

            tabs.erase(tabs.begin() + index);

            for (std::size_t i = 0; tabs.size() > i; i++)
            {
                tabs.at(i).id = i;
            }
        }
        else
        {
            Fancy::fancy.logTime().warning() << "Tried to remove non existent tab" << std::endl;
        }
    }
    void Data::setTabs(const std::vector<Tab> &newTabs)
    {
        tabs = newTabs;
        std::unique_lock lock(Globals::gSoundsMutex);
        std::unique_lock favLock(Globals::gFavoritesMutex);

        Globals::gSounds.clear();
        Globals::gFavorites.clear();
        for (std::size_t i = 0; tabs.size() > i; i++)
        {
            auto &tab = tabs.at(i);
            tab.id = i;

            for (auto &sound : tab.sounds)
            {
                Globals::gSounds.insert({sound.id, sound});
                if (sound.isFavorite)
                {
                    Globals::gFavorites.insert({sound.id, sound});
                }
            }
        }
    }
    std::vector<Tab> Data::getTabs() const
    {
        return tabs;
    }
    std::optional<Tab> Data::getTab(const std::uint32_t &id) const
    {
        if (tabs.size() > id)
        {
            return tabs.at(id);
        }

        Fancy::fancy.logTime().warning() << "Tried to access non existent tab " << id << std::endl;
        return std::nullopt;
    }
    std::optional<std::reference_wrapper<Sound>> Data::getSound(const std::uint32_t &id)
    {
        std::shared_lock lock(Globals::gSoundsMutex);

        if (Globals::gSounds.find(id) != Globals::gSounds.end())
        {
            return Globals::gSounds.at(id);
        }

        Fancy::fancy.logTime().warning() << "Tried to access non existent sound " << id << std::endl;
        return std::nullopt;
    }
    std::optional<Tab> Data::setTab(const std::uint32_t &id, const Tab &tab)
    {
        if (tabs.size() > id)
        {
            auto &realTab = tabs.at(id);

            std::unique_lock lock(Globals::gSoundsMutex);
            std::unique_lock favLock(Globals::gFavoritesMutex);
            for (const auto &sound : realTab.sounds)
            {
                Globals::gSounds.erase(sound.id);
                if (sound.isFavorite)
                {
                    Globals::gFavorites.erase(sound.id);
                }
            }

            realTab = tab;

            for (auto &sound : realTab.sounds)
            {
                Globals::gSounds.insert({sound.id, sound});
                if (sound.isFavorite)
                {
                    Globals::gFavorites.insert({sound.id, sound});
                }
            }
            return realTab;
        }

        Fancy::fancy.logTime().warning() << "Tried to access non existent Tab " << id << std::endl;
        return std::nullopt;
    }
    void Data::set(const Data &other)
    {
        tabs = other.tabs;
        width = other.width;
        height = other.height;
        soundIdCounter = other.soundIdCounter;

        std::unique_lock lock(Globals::gSoundsMutex);
        std::unique_lock favLock(Globals::gFavoritesMutex);
        Globals::gSounds.clear();
        Globals::gFavorites.clear();

        for (std::size_t i = 0; tabs.size() > i; i++)
        {
            auto &tab = tabs.at(i);
            tab.id = i;

            for (auto &sound : tab.sounds)
            {
                Globals::gSounds.insert({sound.id, sound});
                if (sound.isFavorite)
                {
                    Globals::gFavorites.insert({sound.id, sound});
                }
            }
        }
    }
    void Data::markFavorite(const std::uint32_t &id, bool favourite)
    {
        auto sound = getSound(id);
        if (sound)
        {
            sound->get().isFavorite = favourite;
            if (favourite)
            {
                std::unique_lock lock(Globals::gFavoritesMutex);
                Globals::gFavorites.insert({id, sound->get()});
            }
            else
            {
                std::unique_lock lock(Globals::gFavoritesMutex);
                if (Globals::gFavorites.find(id) != Globals::gFavorites.end())
                {
                    Globals::gFavorites.erase(id);
                }
            }
        }
    }
    std::vector<std::uint32_t> Data::getFavoriteIds()
    {
        std::shared_lock lock(Globals::gFavoritesMutex);

        std::vector<std::uint32_t> rtn;
        rtn.reserve(Globals::gFavorites.size());

        for (const auto &sound : Globals::gFavorites)
        {
            rtn.emplace_back(sound.first);
        }

        return rtn;
    }
    std::vector<Sound> Data::getFavorites()
    {
        std::shared_lock lock(Globals::gFavoritesMutex);

        std::vector<Sound> rtn;
        rtn.reserve(Globals::gFavorites.size());

        for (const auto &sound : Globals::gFavorites)
        {
            rtn.emplace_back(sound.second);
        }

        return rtn;
    }
} // namespace Soundux::Objects