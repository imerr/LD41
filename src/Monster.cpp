//
// Created by iMer on 21.04.2018.
//

#include <Engine/Factory.hpp>
#include <Engine/util/Random.hpp>
#include "Monster.hpp"
#include "Level.hpp"
#include "misc.hpp"

Monster::Monster(engine::Scene* scene) : SpriteNode(scene), m_pathSegment(0), m_deathSpawnCount(0), m_dead(false), m_immune(0.5),
										 m_onDone([this](engine::Tween<sf::Vector2f>*) {
											 StartPathTween();
										 }, this) {
	auto r = engine::RandomFloat<float>(-15, 15);
	m_randomOffset = sf::Vector2f(r(), r());
}

void Monster::OnUpdate(sf::Time interval) {
	engine::SpriteNode::OnUpdate(interval);
	if (m_dead) {
		return;
	}
	m_immune -= interval.asSeconds();
	if (m_hp <= 0) {
		Dead();
	}
	m_body->SetAngularVelocity(0);

}

bool Monster::Damage(int damage) {
	if (m_immune > 0) {
		return false;
	}
	if (m_hp > 0 && m_hp - damage <= 0) {
		static_cast<Level*>(m_scene)->ChangeMoney(m_money);
	}
	m_hp -= damage;
	return true;
}

bool Monster::initialize(Json::Value& root) {
	if (!SpriteNode::initialize(root)) {
		return false;
	}
	m_hp = root.get("hp", 1).asInt();
	m_speed = root.get("speed", 1).asFloat();
	m_deathSpawn = root.get("spawn_death", "").asString();
	m_deathSpawnCount = root.get("spawn_death_count", 1).asUInt();
	m_deathDecal = root["death_decal"];
	m_money = root.get("money", 1).asInt();
	return true;
}

void Monster::OnInitializeDone() {
	Node::OnInitializeDone();
	auto level = static_cast<Level*>(m_scene);
	const auto& path = level->GetPath();
	SetPosition(m_randomOffset + path[0]);
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
				auto node = engine::Factory::CreateChild(script, m_parent);
				auto m = static_cast<Monster*>(node);
				m->SetPosition(GetPosition() - m_randomOffset + m->m_randomOffset);
				m->m_pathSegment = m_pathSegment - 1;
				m->StartPathTween();
			}
		}
	}
	if (!m_deathDecal.isNull()) {
		auto n = engine::Factory::CreateChild(m_deathDecal, m_scene);
		if (n) {
			n->SetPosition(GetPosition());
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
	sf::Vector2f prev = GetPosition();
	sf::Vector2f next = m_randomOffset + path[m_pathSegment];
	float distance = engine::distance(prev, next);
	auto tween = MakeTween<sf::Vector2f>(true, prev, next, distance / m_speed, [this](const sf::Vector2f& pos) {
		SetPosition(pos);
	});
	tween->OnDone.AddHandler(&m_onDone);
	float angle = engine::b2Angle(prev, next) * 180 / engine::fPI;
	// dont make the things twirl around, even though its funny
	if (angle >= 180) {
		angle -= 360;
	}
	MakeTween<float>(true, GetRotation(), angle, 0.3f,
					 [this](const float& angle) {
						 SetRotation(angle);
					 });
}

void Monster::ReachEnd() {
	Dead();
	auto cat = static_cast<engine::SpriteNode*>(m_scene->GetChildByID("cat"));
	if (cat) {
		auto col = cat->GetColor();
		if (col.g > 20) {
			col.g -= 20;
			col.b -= 20;
			cat->SetColor(col);
		} else {
			static_cast<Level*>(m_scene)->GameOver();
		}
	}
}

uint8_t Monster::GetType() const {
	return static_cast<uint8_t>(LD41Type::Monster);
}

size_t Monster::GetPathSegment() {
	return m_pathSegment;
}

float Monster::GetSegmentProgress() {
	sf::Vector2f prev = m_randomOffset + static_cast<Level*>(m_scene)->GetPath()[m_pathSegment - 1];
	return engine::distance(prev, GetPosition());
}
