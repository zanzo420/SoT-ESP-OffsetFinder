#include "config.h"
#include "json.h"
#include <experimental/filesystem>

nlohmann::json json;

namespace fs = std::experimental::filesystem;

void assign_item(item &item) {
	if (json[item.name.c_str()].empty())
		return;

	if (item.type == "float")
		* reinterpret_cast<float*>(item.value) = json[item.name.c_str()].get< float >();
	if (item.type == "int")
		* reinterpret_cast<int*>(item.value) = json[item.name.c_str()].get< int >();
	if (item.type == "bool")
		* reinterpret_cast<bool*>(item.value) = json[item.name.c_str()].get< bool >();
	if (item.type == "int [4]")
		for (int i = 0; i < 4; i++)
			reinterpret_cast<int*>(item.value)[i] = json[item.name.c_str()][std::to_string(i).c_str()].get< float >();

}

void save_item(item &item) {

	if (item.type == "float")
		json[item.name.c_str()] = *reinterpret_cast<float*>(item.value);
	if (item.type == "int")
		json[item.name.c_str()] = *reinterpret_cast<int*>(item.value);
	if (item.type == "bool")
		json[item.name.c_str()] = *reinterpret_cast<bool*>(item.value);
	if (item.type == "int [4]")
		for (int i = 0; i < 4; i++)
			json[item.name.c_str()][std::to_string(i).c_str()] = reinterpret_cast<int*>(item.value)[i];

}

void reset_item(item &item) {
	if (json[item.name.c_str()].empty())
		return;

	if (json[item.name.c_str()].is_array())
		for (int i = 0; i < 4; i++)
			json[item.name.c_str()][i].empty();

	json[item.name.c_str()].clear();
}

bool c_config::init() {

	m_directory = std::experimental::filesystem::current_path().string();

	if (!std::experimental::filesystem::exists(m_directory)) {
		if (!std::experimental::filesystem::create_directory(m_directory)) {
			_RPT1(_CRT_WARN, "Failed to create profile directory. Ignoring this error will result in not being able to create or save profiles.\n\n%s", m_directory);
			return false;
		}
	}

	item(Offsets.UWorld.OwningGameInstance, "UWorld.OwningGameInstance", m_items);
	item(Offsets.UWorld.PersistentLevel, "UWorld.PersistentLevel", m_items);

	item(Offsets.UGameInstance.LocalPlayers, "UGameInstance.LocalPlayers", m_items);

	item(Offsets.ULocalPlayer.PlayerController, "ULocalPlayer.PlayerController", m_items);

	item(Offsets.APlayerController.CameraManager, "APlayerController.CameraManager", m_items);
	item(Offsets.APlayerController.Pawn, "APlayerController.Pawn", m_items);
	item(Offsets.APlayerController.ControlRotation, "APlayerController.ControlRotation", m_items);

	item(Offsets.AActor.actorId, "AActor.actorId", m_items);
	item(Offsets.AActor.HealthComponent, "AActor.HealthComponent", m_items);
	item(Offsets.AActor.WieldedItemComponent, "AActor.WieldedItemComponent", m_items);
	item(Offsets.AActor.rootComponent, "AActor.rootComponent", m_items);
	item(Offsets.AActor.PlayerState, "AActor.PlayerState", m_items);


	item(Offsets.APlayerCameraManager.CameraCache, "APlayerCameraManager.CameraCache", m_items);

	item(Offsets.APlayerState.PlayerName, "APlayerState.PlayerName", m_items);
	item(Offsets.AItemProxy.AItemInfo, "AItemProxy.AItemInfo", m_items);

	item(Offsets.AItemInfo.UItemDesc, "AItemInfo.UItemDesc", m_items);

	item(Offsets.ABootyItemInfo.BootyType, "ABootyItemInfo.BootyType", m_items);
	item(Offsets.ABootyItemInfo.Rarity, "ABootyItemInfo.Rarity", m_items);



	item(Offsets.AShip.CrewOwnershipComponent,"AShip.CrewOwnershipComponent" ,m_items);
	item(Offsets.UCrewOwnershipComponent.CrewId,"UCrewOwnershipComponent.CrewId" ,m_items);

	item(Offsets.ACrewService.Crews,"ACrewService.Crews",m_items);

	item(Offsets.AMapTable.MapPins,"AMapTable.MapPins" ,m_items);
	item(Offsets.AMapTable.ServerCenter,"AMapTable.ServerCenter" ,m_items);
	item(Offsets.AMapTable.TrackedShips,"AMapTable.TrackedShips" ,m_items);

	item(Offsets.UWieldedItemComponent.WieldedItem,"UWieldedItemComponent.WieldedItem" ,m_items);
	item(Offsets.AWieldableItem.ItemInfo, "AWieldableItem.ItemInfo",m_items);

	item(Offsets.AFauna.Name,"AFauna.Name" ,m_items);

	return true;
}

bool c_config::save(const std::string &file) {
	try {
		std::ofstream output_file = std::ofstream(m_directory + "/" + file);

		if (!output_file.good())
			return false;

		for (auto &item : m_items)
			save_item(item);

		output_file << std::setw(4) << json << std::endl;
		output_file.close();

		return true;
	}
	catch (std::ofstream::failure &ex) {
		_RPT1(_CRT_WARN, "Failed to save the default profile. Ignoring this warning will most likely result in future profiles not being saved correctly.\n\n%s", ex.what());
		return false;
	}
}

bool c_config::reset(const std::string &file) {
	std::ofstream output_file = std::ofstream(m_directory + "/" + file);

	if (!output_file.good())
		return false;

	for (auto &item : m_items)
		reset_item(item);

	output_file << std::setw(4) << json << std::endl;
	output_file.close();

	return true;
}

void c_config::remove(const std::string &file) const {
	std::string path = m_directory + "/" + file;
	std::remove(path.c_str());
}

std::vector< std::string > c_config::get_configs() const {
	std::vector< std::string > output{ };

	for (auto &file_path : std::experimental::filesystem::directory_iterator(m_directory)) {
		if (file_path.path().string().empty())
			continue;

		if (file_path.path().string().find('.') != std::string::npos)
			continue;

		output.emplace_back(file_path.path().string().erase(0, m_directory.length() + 1));
	}

	return output;
}

