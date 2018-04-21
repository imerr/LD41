//
// Created by iMer on 21.04.2018.
//

#include <Engine/SpriteNode.hpp>
#include <Engine/Factory.hpp>
#include <Engine/ResourceManager.hpp>
#include <Engine/util/Random.hpp>
#include <Engine/Game.hpp>
#include "Level.hpp"

Level::Level(engine::Game* game) : Scene(game), m_bpm(0) {
	m_beatIndicator = new engine::SpriteNode(this);
	m_beatIndicator->SetSize(sf::Vector2f(100, 100));
	m_beatIndicator->SetColor(sf::Color::Red);
	AddNode(m_beatIndicator);
	m_inputIndicator = new engine::SpriteNode(this);
	m_inputIndicator->SetPosition(100, 0);
	m_inputIndicator->SetSize(sf::Vector2f(100, 100));
	m_inputIndicator->SetColor(sf::Color::Black);
	AddNode(m_inputIndicator);
	m_keyHandler = game->OnKeyPress.MakeHandler([](const sf::Event::KeyEvent&, bool) {return true;},
												[this](const sf::Event::KeyEvent& e, bool down) {
		if (e.code == sf::Keyboard::Space && down) {
			m_pressedInput = true;
		}
		return false;
	}, this);
}

Level::~Level() {
	m_game->OnKeyPress.RemoveHandler(m_keyHandler);
}

bool Level::initialize(Json::Value& root) {
	if (!Scene::initialize(root)) {
		return false;
	}
	m_bpm = root["music"].get("bpm", 120).asInt();
	m_sounds.clear();
	for (auto& sound : root["music"]["sounds"]) {
		SoundInfo s;
		s.Sound.reset(engine::ResourceManager::instance()->MakeSound(sound.get("sound", "").asString()));
		s.Interval = sound.get("interval", 1).asFloat();
		s.Volume = sound.get("volume", 1).asFloat();
		s.Offset = sound.get("offset", 0).asFloat();
		s.Deviation = sound.get("deviation", 0).asFloat();
		s.Type = static_cast<SoundInfo::SoundType>(sound.get("type", 0).asInt());
				s.Reset();
		// TODO: pitches
		m_sounds.emplace_back(std::move(s));
	}
	return true;
}

void Level::OnUpdate(sf::Time interval) {
	Scene::OnUpdate(interval);
	float beatTime = interval.asSeconds() / (60.0f / m_bpm);
	float closestDistance = std::numeric_limits<float>::infinity();
	for (auto& sound : m_sounds) {
		sound.Update(beatTime);
		if (sound.Type == SoundInfo::SoundType::Beat) {
			if (sound.Played) {
				m_beatIndicator->MakeTween<sf::Color>(false, sf::Color::Green, sf::Color::Red, 0.2f, [this](const sf::Color& c){
					m_beatIndicator->SetColor(c);
				});
			}
			if (m_pressedInput) {
				float distance = std::min(std::abs(sound.NextPlayTime()), std::abs(sound.LastPlayTime())) * (60.0f / m_bpm);
				if (distance < closestDistance) {
					closestDistance = distance;
				}
			}
		}
	}
	if (m_pressedInput && closestDistance != std::numeric_limits<float>::infinity()) {
		std::cout << closestDistance << std::endl;
		sf::Color c = sf::Color::Green;
		if (closestDistance > 0.3) {
			c = sf::Color::Red;
		} else if (closestDistance > 0.2) {
			c = sf::Color(255, 128, 0); // Orange
		} else  if (closestDistance > 0.1) {
			c = sf::Color::Yellow;
		}
		m_inputIndicator->MakeTween<sf::Color>(false, c, sf::Color::Black, 0.2f, [this](const sf::Color& c){
			m_inputIndicator->SetColor(c);
		});
	}
	m_pressedInput = false;
}



Level::SoundInfo::SoundInfo() : Interval(1), Volume(1), Offset(0), Deviation(0), Sound(nullptr),
								m_beatTime(std::numeric_limits<float>::infinity()), Played(false) {

}

void Level::SoundInfo::Update(float interval) {
	m_beatTime += interval;
	Played = false;
	// TODO: deviation, pitch
	if (m_beatTime > m_playIn) {
		m_prevPlayTime = m_playIn;
		m_beatTime -= m_playIn;
		m_playIn = Interval;
		Sound->play();
		Played = true;
	}
}

void Level::SoundInfo::Reset() {
	m_beatTime = -Offset;
	m_playIn = Interval;
	Sound->setVolume(Volume * 100);
}
