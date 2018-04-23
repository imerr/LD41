//
// Created by iMer on 23.04.2018.
//

#include <SFML/Window/Mouse.hpp>
#include <Engine/Game.hpp>
#include "ClickBlocker.hpp"

ClickBlocker::ClickBlocker(engine::Scene* scene) : SpriteNode(scene) {
	m_clickBlocker.reset(m_scene->GetGame()->OnMouseClick.MakeHandler(
			[this](const sf::Mouse::Button& button, const sf::Vector2f& pos, bool down) {
				return IsIn(pos.x, pos.y) && IsActive();
			}, [this](const sf::Mouse::Button& button, const sf::Vector2f&, bool down) {
				return true;
			}, this));
}

ClickBlocker::~ClickBlocker() {
	m_scene->GetGame()->OnMouseClick.RemoveHandler(m_clickBlocker.get());
}
