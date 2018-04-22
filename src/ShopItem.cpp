//
// Created by iMer on 21.04.2018.
//

#include <Engine/Text.hpp>
#include <sstream>
#include "ShopItem.hpp"
#include "Menu.hpp"
#include "Level.hpp"
#include "Tower.hpp"
#include <Engine/Factory.hpp>

ShopItem::ShopItem(engine::Scene* scene) : Button(scene), m_price(0) {
	OnClick.MakeHandler([this](Button*, sf::Vector2f) {
		if (static_cast<Level*>(m_scene)->ChangeMoney(-m_price)) {
			auto tower = static_cast<Tower*>(engine::Factory::CreateChildFromFile(m_script,
																				  m_scene->GetChildByID("towers")));
			tower->SetRefund(m_price);
			tower->SetBuildMode();
		} else {
			// TODO: error sound or sth
		}
	});
}

void ShopItem::OnUpdate(sf::Time interval) {
	Button::OnUpdate(interval);
	auto menu = static_cast<Menu*>(m_parent->GetParent());
	if (m_state == BUTTON_NONE) {
		menu->ClearDescription(this);
	} else {
		menu->SetDescription(this, m_description);
	}
}

bool ShopItem::initialize(Json::Value& root) {
	if (!SpriteNode::initialize(root)) {
		return false;
	}
	m_price = root.get("price", 0).asInt();
	m_description = root.get("description", "").asString();
	m_icon = root.get("icon", "").asString();
	m_script = root.get("script", "").asString();
	return true;
}

void ShopItem::OnInitializeDone() {
	Node::OnInitializeDone();
	if (!m_icon.empty()) {
		auto ch = GetChildByID("icon");
		if (ch) {
			static_cast<engine::SpriteNode*>(ch)->SetTexture(m_icon);
		}
	}
	auto price = GetChildByID("price");
	if (price) {
		std::ostringstream ss;
		ss << m_price;
		static_cast<engine::Text*>(price)->SetText(ss.str());
	}
}
