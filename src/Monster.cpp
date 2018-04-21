//
// Created by iMer on 21.04.2018.
//

#include "Monster.hpp"

Monster::Monster(engine::Scene* scene) : SpriteNode(scene), m_pathSegment(0) {

}

void Monster::OnUpdate(sf::Time interval) {
	engine::SpriteNode::OnUpdate(interval);

}

bool Monster::initialize(Json::Value& root) {
	if (!SpriteNode::initialize(root)) {
		return false;
	}
	return true;
}
