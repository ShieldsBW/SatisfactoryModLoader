#pragma once

#include "Modules/ModuleManager.h"
#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include "mod/ModInfo.h"
#include "util/bootstrapper_exports.h"

using namespace std::experimental::filesystem;

namespace SML {
	namespace Mod {
		struct FModLoadingEntry {
			bool isValid;
			FModInfo modInfo;
			std::wstring virtualModFilePath;
			std::wstring dllFilePath;
			std::vector<std::wstring> pakFilePaths;
			bool isRawMod = false;
		};

		struct FModContainer {
			const FModInfo modInfo;
			IModuleInterface* moduleInterface;
		};

		struct FModPakLoadEntry {
			std::wstring modid;
			UClass* modInitClass;
			UClass* menuInitClass;
		};
		
		/**
		* Holds important information about the loaded mods.
		* It should never be accessed by mods directly.
		*/ 
		SML_API class FModHandler {
		private:
			std::vector<FModLoadingEntry> sortedModLoadList;
			std::unordered_map<std::wstring, FModLoadingEntry> loadingEntries;
			std::vector<FModPakLoadEntry> modPakInitializers;
			std::vector<std::wstring> loadingProblems;

			std::unordered_map<std::wstring, FModContainer*> loadedMods;
			std::vector<FModContainer*> loadedModsList;
			std::vector<std::wstring> loadedModsModIDs;
		public:
			FModHandler();
			
			bool isModLoaded(const std::wstring& modId) const;

			/**
			* Returns a module definition for the specified modid
			* Throws std::invalid_argument if mod with specified ID is not loaded
			*/
			const FModContainer& GetLoadedMod(const std::wstring& modId) const;

			/**
			* Returns a map of all loaded mod ids
			*/
			const std::vector<std::wstring>& getLoadedMods() const;
		private:
			FModLoadingEntry& createRawModLoadingEntry(const std::wstring& modId, const path& filePath);
			FModLoadingEntry& createLoadingEntry(const FModInfo& modInfo, const path& filePath);
			
			bool checkAndNotifyRawMod(const path& filePath);
			void reportBrokenZipMod(const path& filePath, const std::wstring& reason);
			void checkStageErrors(const  TCHAR* stageName);
			
			void constructZipMod(const path& filePath);
			void constructPakMod(const path& filePath);
			void constructDllMod(const path& filePath);
			
		public:
			/**
			* Load all mods from the given path.
			*/
			void discoverMods();

			/**
			* Ensure that all dependencies of mods exist.
			*/
			void checkDependencies();

			/**
			* Loads the mods into the game
			*/
			void loadMods(const BootstrapAccessors& accessors);

			void onGameModePostLoad(UWorld* world, bool isMenuWorld);
		};
	};
};