//
// Created by iMer on 21.04.2018.
//

#include <Engine/Factory.hpp>
#include "LD41.hpp"
#include "Level.hpp"

LD41::LD41(): Game(1024, 576) {
	m_scene = engine::Factory::create<Level>("assets/scripts/level.json", this);
}

LD41::~LD41() {

}

void LD41::Restart() {
	m_loadingScene->Switch(m_scene, engine::Factory::create<Level>("assets/scripts/level.json", this));
}
