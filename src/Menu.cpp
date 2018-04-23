//
// Created by iMer on 21.04.2018.
//

#include <Engine/Button.hpp>
#include "Menu.hpp"
#include "Level.hpp"
#include <Engine/Text.hpp>
#include <Engine/Game.hpp>

Menu::Menu(engine::Scene* scene) : SpriteNode(scene), m_collapsed(false) {
	m_defaultDescription = "Buy some towers and\nonce you are happy\nclick start to\nstart the round";
	m_clickBlocker.reset(m_scene->GetGame()->OnMouseClick.MakeHandler(
			[this](const sf::Mouse::Button& button, const sf::Vector2f& pos, bool down) {
				return IsIn(pos.x, pos.y);
			}, [this](const sf::Mouse::Button& button, const sf::Vector2f&, bool down) {
				return true;
			}, this));
}

void Menu::OnUpdate(sf::Time interval) {
	engine::SpriteNode::OnUpdate(interval);
	auto level = static_cast<Level*>(m_scene);
	m_playButton->SetActive(!level->IsFighting());
}

void Menu::OnInitializeDone() {
	auto collapse = GetChildByID("collapse");
	if (collapse) {
		static_cast<engine::Button*>(collapse)->OnClick.MakeHandler([this](engine::Button* b, sf::Vector2f pos) {
			ToggleCollapse();
		});
		SetDescription(nullptr, "");
	}
	m_playButton = static_cast<engine::Button*>(GetChildByID("start"));
	m_playHandler.reset(
			m_playButton->OnClick.MakeHandler([this](engine::Button*, sf::Vector2f) {
				static_cast<Level*>(m_scene)->SetFighting(true);
				if (!IsCollapsed()) {
					ToggleCollapse();
				}
			}, this));
}

void Menu::ToggleCollapse() {
	sf::Vector2f from(824, 0);
	sf::Vector2f to(1024, 0);
	if (m_collapsed) {
		std::swap(from, to);
	}
	MakeTween<sf::Vector2f>(true, from, to, 0.3, [this](const sf::Vector2f& pos) {
		SetPosition(pos);
	}, engine::EasingLinear);
	m_collapsed = !m_collapsed;
}

void Menu::ClearDescription(engine::Node* node) {
	if (m_descOwner == node) {
		SetDescription(nullptr, "");
	}
}

void Menu::SetDescription(engine::Node* owner, std::string desc) {
	if (desc.empty()) {
		desc = m_defaultDescription;
	}
	m_descOwner = owner;
	size_t line = 0;
	size_t start = 0;
	size_t pos = 0;
	auto lines = GetChildByID("description");
	do {
		pos = desc.find('\n', start);
		if (pos == std::string::npos) {
			pos = desc.length();
		}
		if (line >= lines->GetChildren().size()) {
			std::cout << "Too many lines in desc!" << std::endl;
			return;
		}
		auto l = static_cast<engine::Text*>(lines->GetChildren()[line]);
		line++;
		l->SetText(desc.substr(start, pos - start));
		start = pos + 1;
	} while (pos != desc.length() && start != desc.length());
	for (; line < lines->GetChildren().size(); line++) {
		static_cast<engine::Text*>(lines->GetChildren()[line])->SetText("");
	}
}

bool Menu::IsCollapsed() {
	return m_collapsed;
}

void Menu::SetDefaultDescription(const std::string& desc) {
	m_defaultDescription = desc;
}

Menu::~Menu() {
	m_scene->GetGame()->OnMouseClick.RemoveHandler(m_clickBlocker.get());
}
