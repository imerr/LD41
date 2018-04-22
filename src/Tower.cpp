//
// Created by iMer on 22.04.2018.
//

#include "Tower.hpp"
#include "Level.hpp"
#include <Engine/Scene.hpp>
#include <Engine/Game.hpp>

Tower::Tower(engine::Scene* scene) : SpriteNode(scene), m_placeMode(false), m_placementBlocked(false) {
	m_clickHandler = std::make_unique<engine::BaseEventHandler>(m_scene->GetGame()->OnMouseClick.MakeHandler([this](const sf::Mouse::Button& button, const sf::Vector2f&, bool down){
		return m_placeMode && !m_placementBlocked && !down && (button == sf::Mouse::Left || button == sf::Mouse::Right);
	}, [this](const sf::Mouse::Button& button, const sf::Vector2f&, bool down){
		if (button == sf::Mouse::Right) {
			Delete();
			static_cast<Level*>(m_scene)->ChangeMoney(m_refund);
		}
		m_placeMode = false;
		return true;
	}, this));
}

void Tower::OnUpdate(sf::Time interval) {
	if (m_placeMode) {
		m_placementBlocked = false;
		auto pos = m_scene->GetGame()->GetMousePosition();
		SetPosition(pos);
		if (pos.x < 0 || pos.x > m_scene->GetSize().x || pos.y < 0 || pos.y > m_scene->GetSize().y) {
			m_placementBlocked = true;
			return;
		}
		// TODO: check if the tower is colliding with anything else
	} else {
		// TODO
	}
}

void Tower::OnDraw(sf::RenderTarget& target, sf::RenderStates states, float delta) {
	if (m_placeMode) {
		sf::CircleShape c;
		if (m_placementBlocked) {
			c.setFillColor(sf::Color(100,0,0, 50));
		} else {
			c.setFillColor(sf::Color(0,0,0, 50));
		}
		c.setPosition(-m_range + getOrigin().x, -m_range + getOrigin().y);
		c.setRadius(m_range);
		target.draw(c, states);
	}
	SpriteNode::OnDraw(target, states, delta);
}

bool Tower::initialize(Json::Value& root) {
	if (!SpriteNode::initialize(root)) {
		return false;
	}
	m_attackSpeed = root.get("speed", 10).asFloat();
	m_range = root.get("range", 50).asFloat();
	m_damageDelay = root.get("damage_delay", 0).asFloat();
	m_damage = root.get("damage", 1).asInt();
	return true;
}

void Tower::SetBuildMode() {
	m_placeMode = true;
}

void Tower::SetRefund(int refund) {
	m_refund = refund;
}
