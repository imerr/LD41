//
// Created by iMer on 22.04.2018.
//

#include <Engine/Scene.hpp>
#include "Projectile.hpp"
#include "misc.hpp"
#include "Monster.hpp"

Projectile::Projectile(engine::Scene* scene) : SpriteNode(scene) {
	m_collisionHandler.reset(m_scene->OnContactPreSolve.MakeHandler([this](b2Contact* contact, const b2Manifold*){
		auto a = static_cast<engine::Node*>(contact->GetFixtureA()->GetBody()->GetUserData());
		auto b = static_cast<engine::Node*>(contact->GetFixtureB()->GetBody()->GetUserData());
		engine::Node* other = nullptr;
		if (a == this) {
			other = b;
		} else if (b == this) {
			other = a;
		} else {
			return;
		}
		contact->SetEnabled(false);
		if (other->GetType() == static_cast<uint8_t>(LD41Type::Monster)) {
			this->Damage(static_cast<Monster*>(other));
		}

	}, this));
}

Projectile::~Projectile() {
	m_scene->OnContactPreSolve.RemoveHandler(m_collisionHandler.get());
}

uint8_t Projectile::GetType() const {
	return static_cast<uint8_t>(LD41Type::Projectile);
}

void Projectile::Damage(Monster* monster) {
	if (m_hits == 0 || monster->IsDead() || m_damaged.find(monster) != m_damaged.end()) {
		return;
	}
	if (!monster->Damage(m_damage)) {
		return;
	}
	m_damaged.insert(monster);
	m_hits--;
	if (m_hits == 0) {
		Delete();
	}
}

bool Projectile::initialize(Json::Value& root) {
	if (!SpriteNode::initialize(root)) {
		return false;
	}
	m_damage = root.get("damage", 1).asInt();
	m_hits = root.get("hits", 1).asInt();
	return true;
}
