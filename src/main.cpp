#include <Engine/Game.hpp>
#include <Engine/Factory.hpp>
#include "LD41.hpp"
#include "Level.hpp"
#include "BeatDisplay.hpp"

int main() {
	engine::Factory::RegisterType("beat_display", engine::Factory::CreateChildNode<BeatDisplay>);
	LD41 game;
	game.run();
	return 0;
}