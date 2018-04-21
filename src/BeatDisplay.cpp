//
// Created by iMer on 21.04.2018.
//

#include "BeatDisplay.hpp"
#include "Level.hpp"
#include <Engine/Factory.hpp>

BeatDisplay::BeatDisplay(engine::Scene* scene) : Node(scene) {
	for (int i = 0; i < static_cast<int>(SoundInfo::BeatType::Max); i++) {
		auto t = static_cast<SoundInfo::BeatType>(i);
		m_bars.insert(std::make_pair(t, std::vector<engine::Node*>()));
		m_colorTweens.insert(std::make_pair(t, new engine::Tween<sf::Color>(sf::Color(100, 100, 100), sf::Color::White, 0.4, [this, t](const sf::Color& v) {
			for (auto& c : m_bars[t]) {
				static_cast<engine::SpriteNode*>(c)->SetColor(v);
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
	std::map<SoundInfo::BeatType, int> barC;
	for (int i = 0; i < static_cast<int>(SoundInfo::BeatType::Max); i++) {
		auto t = static_cast<SoundInfo::BeatType>(i);
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
		for (size_t o = 0; o < 20 && sound.NextPlayTime(o) * bpm <= DisplayTime; o++) {
			if (barC[sound.Type] >= m_bars[sound.Type].size()) {
				AddBar(sound.Type);
			}
			auto bar = static_cast<engine::SpriteNode*>(m_bars[sound.Type][barC[sound.Type]]);
			bar->SetPosition(0, barHeightPerSecond * sound.NextPlayTime(o) * bpm);
			barC[sound.Type]++;
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

void BeatDisplay::AddBar(SoundInfo::BeatType type) {
	auto bar = engine::Factory::CreateChild(m_beats[type], this);
	m_bars[type].push_back(bar);
}

bool BeatDisplay::initialize(Json::Value& root) {
	if (!Node::initialize(root)){
		return false;
	}
	auto& beats = root["beats"];
	for (const auto& key : beats.getMemberNames()) {
		auto type = static_cast<SoundInfo::BeatType>(std::stoi(key));
		m_beats.insert(std::make_pair(type, beats[key]));
	}
	return true;
}


