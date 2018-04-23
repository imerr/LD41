//
// Created by iMer on 21.04.2018.
//

#include "BeatDisplay.hpp"
#include <Engine/Factory.hpp>

BeatDisplay::BeatDisplay(engine::Scene* scene) : Node(scene) {
	for (int i = 0; i < static_cast<int>(BeatType::Max); i++) {
		auto t = static_cast<BeatType>(i);
		m_bars.insert(std::make_pair(t, std::vector<engine::Node*>()));
		m_colorTweens.insert(std::make_pair(t, new engine::Tween<sf::Color>(sf::Color(100, 100, 100), sf::Color::White,
																			0.4f, [this, t](const sf::Color& v) {
					for (auto& node : m_bars[t]) {
						auto s = static_cast<engine::SpriteNode*>(node);
						sf::Color col = v;
						col.a = s->GetColor().a;
						s->SetColor(col);
					}
				})));
	}
}

BeatDisplay::~BeatDisplay() {
	for (auto& kv : m_colorTweens) {
		delete kv.second;
	}
	m_colorTweens.clear();
}

void BeatDisplay::OnUpdate(sf::Time interval) {
	Node::OnUpdate(interval);
	auto level = static_cast<Level*>(m_scene);
	float bpm = 60.0f / level->GetBPM();
	std::map<BeatType, int> barC;
	for (int i = 0; i < static_cast<int>(BeatType::Max); i++) {
		auto t = static_cast<BeatType>(i);
		barC.insert(std::make_pair(t, 0));
	}
	float barHeightPerSecond = 800 / DisplayTime;
	for (auto& sound : level->GetSounds()) {
		if (m_beats.find(sound.Type) == m_beats.end()) {
			continue;
		}
		if (sound.Played) {
			m_colorTweens[sound.Type]->Reset();
		}
		float duration = (std::abs(sound.LastPlayTime()) + sound.NextPlayTime()) / 2.0f;
		if (sound.LastPlayTime() < 0 && std::abs(sound.LastPlayTime()) < duration) {
			if (barC[sound.Type] >= m_bars[sound.Type].size()) {
				AddBar(sound.Type);
			}
			auto bar = static_cast<engine::SpriteNode*>(m_bars[sound.Type][barC[sound.Type]]);
			auto c = bar->GetColor();
			float pct =  ((duration + sound.LastPlayTime()) / duration);
			c.a = static_cast<uint8_t>((sound.IsPrevHit() ? 128 : 255) * std::min(1.0f, std::max(0.0f, pct)));
			bar->SetColor(c);
			bar->SetPosition(-45 + static_cast<int>(sound.Type) * (20), barHeightPerSecond * sound.LastPlayTime() * bpm);
			barC[sound.Type]++;
		}
		for (size_t o = 0; o < 20 && sound.NextPlayTime(o) * bpm <= DisplayTime; o++) {
			if (barC[sound.Type] >= m_bars[sound.Type].size()) {
				AddBar(sound.Type);
			}
			auto bar = static_cast<engine::SpriteNode*>(m_bars[sound.Type][barC[sound.Type]]);
			bar->SetPosition(-45 + static_cast<int>(sound.Type) * (20), barHeightPerSecond * sound.NextPlayTime(o) * bpm);
			barC[sound.Type]++;
			auto c = bar->GetColor();
			if (o == 0 && sound.IsCurrentHit()) {
				c.a = 128;
			} else {
				c.a = 255;
			}
			bar->SetColor(c);
		}
	}
	for (auto& kv : barC) {
		for (int i = kv.second; i < m_bars[kv.first].size(); i++) {
			m_bars[kv.first][i]->SetActive(false);
		}
	}
}

void BeatDisplay::OnDraw(sf::RenderTarget& target, sf::RenderStates states, float delta) {
	Node::OnDraw(target, states, delta);
	for (auto& kv : m_colorTweens) {
		kv.second->Update(delta);
	}
}

void BeatDisplay::AddBar(BeatType type) {
	auto bar = engine::Factory::CreateChild(m_beats[type], this);
	m_bars[type].push_back(bar);
}

bool BeatDisplay::initialize(Json::Value& root) {
	if (!Node::initialize(root)) {
		return false;
	}
	auto& beats = root["beats"];
	for (const auto& key : beats.getMemberNames()) {
		auto type = static_cast<BeatType>(std::stoi(key));
		m_beats.insert(std::make_pair(type, beats[key]));
	}
	return true;
}


