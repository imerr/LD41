//
// Created by iMer on 21.04.2018.
//

#include <Engine/Factory.hpp>
#include "Monster.hpp"
#include "Level.hpp"

Monster::Monster(engine::Scene* scene) : SpriteNode(scene), m_pathSegment(0), m_deathSpawnCount(0), m_dead(false),
										 m_onDone([this](engine::Tween<sf::Vector2f>*) {
											 StartPathTween();
										 }, this) {

}

void Monster::OnUpdate(sf::Time interval) {
	engine::SpriteNode::OnUpdate(interval);
	if (m_dead) {
		return;
	}

}

void Monster::Damage(int damage) {
	m_hp -= damage;
	if (m_hp <= 0) {
		Dead();
	}
}

bool Monster::initialize(Json::Value& root) {
	if (!SpriteNode::initialize(root)) {
		return false;
	}
	m_hp = root.get("hp", 1).asInt();
	m_speed = root.get("speed", 1).asFloat();
	m_deathSpawn = root.get("spawn_death", "").asString();
	m_deathSpawnCount = root.get("spawn_death_count", 1).asUInt();
	return true;
}

void Monster::OnInitializeDone() {
	Node::OnInitializeDone();
	StartPathTween();
}

void Monster::Dead() {
	if (m_dead) {
		return;
	}
	m_dead = true;
	if (!m_deathSpawn.empty() && m_deathSpawnCount > 0) {
		Json::Value script;
		if (engine::Factory::LoadJson(m_deathSpawn, script)) {
			for (size_t i = 0; i < m_deathSpawnCount; i++) {
				engine::Factory::CreateChild(script, m_parent);
			}
		}
	}
	Delete();
}

void Monster::StartPathTween() {
	m_pathSegment++;
	auto level = static_cast<Level*>(m_scene);
	const auto& path = level->GetPath();
	if (m_pathSegment >= path.size()) {
		ReachEnd();
		return;
	}
	sf::Vector2f prev = path[m_pathSegment - 1];
	sf::Vector2f next = path[m_pathSegment];
	float distance = engine::distance(prev, next);
	auto tween = MakeTween<sf::Vector2f>(true, prev, next, distance / m_speed, [this](const sf::Vector2f& pos) {
		SetPosition(pos);
	});
	tween->OnDone.AddHandler(&m_onDone);

}

void Monster::ReachEnd() {
	Delete();
	// TODO: do damage or sth
}
