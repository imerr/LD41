#include <Engine/Game.hpp>
#include <Engine/Factory.hpp>
#include "LD41.hpp"
#include "Level.hpp"
#include "BeatDisplay.hpp"
#include "Monster.hpp"
#include "Menu.hpp"
#include "ShopItem.hpp"
#include "Tower.hpp"

int main() {
	engine::Factory::RegisterType("beat_display", engine::Factory::CreateChildNode<BeatDisplay>);
	engine::Factory::RegisterType("monster", engine::Factory::CreateChildNode<Monster>);
	engine::Factory::RegisterType("menu", engine::Factory::CreateChildNode<Menu>);
	engine::Factory::RegisterType("shop_item", engine::Factory::CreateChildNode<ShopItem>);
	engine::Factory::RegisterType("tower", engine::Factory::CreateChildNode<Tower>);
	LD41 game;
	game.run();
	return 0;
}