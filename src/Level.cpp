//
// Created by iMer on 21.04.2018.
//

#include <Engine/SpriteNode.hpp>
#include <Engine/Factory.hpp>
#include <Engine/ResourceManager.hpp>
#include <Engine/util/Random.hpp>
#include <Engine/Game.hpp>
#include <Engine/util/json.hpp>
#include <sstream>
#include <Engine/Text.hpp>
#include "Level.hpp"
#include "Menu.hpp"

Level::Level(engine::Game* game) : Scene(game), m_bpm(0), m_inputIndicator(nullptr),
								   m_pressedInput(false), m_currentRound(0), m_currentSpawn(0), m_fighting(false),
								   m_spawnTimer(0), m_money(0) {
	m_keyHandler = m_game->OnKeyPress.MakeHandler([](const sf::Event::KeyEvent&, bool) { return true; },
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
		s.Type = static_cast<BeatType>(sound.get("type", 0).asInt());
		s.Reset();
		m_sounds.emplace_back(std::move(s));
	}

	for (auto& point : root["path"]) {
		m_path.push_back(engine::vector2FromJson<float>(point));
	}
	m_rounds.clear();
	for (auto& round : root["rounds"]) {
		std::vector<SpawnInfo> r;
		for (auto& spawn : round) {
			r.push_back(SpawnInfo(spawn[0u].asString(), spawn.get(1u, 1).asInt(), spawn.get(2u, 1).asFloat()));
		}
		m_rounds.emplace_back(std::move(r));
	}

	return true;
}

void Level::OnUpdate(sf::Time interval) {
	Scene::OnUpdate(interval);
	m_inputIndicator->SetActive(m_fighting);
	if (m_fighting) {
		m_spawnTimer += interval.asSeconds();
		auto monsterContainer = GetChildByID("monsters");
		if (m_currentSpawn >= m_rounds[m_currentRound].size() && monsterContainer->GetChildren().size() == 0) {
			m_fighting = false;
			m_currentRound++;
			auto menu = static_cast<Menu*>(GetUi()->GetChildByID("menu"));
			if (menu->IsCollapsed()) {
				menu->ToggleCollapse();
				std::ostringstream ss;
				ss << "Completed round " << m_currentRound << "\nAwarded TODO moneys\n";
				menu->SetDefaultDescription(ss.str());
			}
			return;
		}
		while (m_currentSpawn < m_rounds[m_currentRound].size() &&
			   m_spawnTimer > m_rounds[m_currentRound][m_currentSpawn].Delay) {
			auto& spawn = m_rounds[m_currentRound][m_currentSpawn];
			if (spawn.Count == 0) {
				m_currentSpawn++;
				continue;
			}
			spawn.Count--;
			m_spawnTimer -= spawn.Delay;
			engine::Factory::CreateChildFromFile(spawn.Script, monsterContainer);
		}

		float beatTime = interval.asSeconds() / (60.0f / m_bpm);
		bool prevHit = false;
		float closestDistance = std::numeric_limits<float>::infinity();
		SoundInfo* closestSound = nullptr;
		for (auto& sound : m_sounds) {
			sound.Update(beatTime);
			if (sound.Type == BeatType::Beat) {
				if (m_pressedInput) {
					float distance =
							std::min(std::abs(sound.NextPlayTime()), std::abs(sound.LastPlayTime())) * (60.0f / m_bpm);

					if (distance < closestDistance && !sound.IsHit && distance < HitLimit) {
						prevHit = std::abs(sound.NextPlayTime()) > std::abs(sound.LastPlayTime());
						closestDistance = distance;
						closestSound = &sound;
					}
				}
			}
		}
		if (m_pressedInput && closestSound) {
			std::cout << closestDistance << std::endl;
			sf::Color c = sf::Color::Blue;
			closestSound->Hit(prevHit);
			if (closestDistance > 0.3) {
				c = sf::Color::Red;
			} else if (closestDistance > 0.2) {
				c = sf::Color(255, 128, 0); // Orange
			} else if (closestDistance > 0.1) {
				c = sf::Color::Yellow;
			} else if (closestDistance > 0.05) {
				c = sf::Color::Green;
			}
			m_inputIndicator->MakeTween<sf::Color>(false, c, sf::Color::White, 0.2f, [this](const sf::Color& c) {
				m_inputIndicator->SetColor(c);
			});
		}
		m_pressedInput = false;
	}
}

void Level::OnInitializeDone() {
	Node::OnInitializeDone();
	static_cast<engine::SpriteNode*>(GetUi()->GetChildByID("beat_indicator"));
	m_inputIndicator = static_cast<engine::SpriteNode*>(GetUi()->GetChildByID("input_indicator"));
	ChangeMoney(100);
}

std::vector<SoundInfo>& Level::GetSounds() {
	return m_sounds;
}

bool Level::ChangeMoney(int money) {
	if (money + m_money < 0) {
		return false;
	}
	m_money += money;
	std::ostringstream ss;
	ss << m_money;
	static_cast<engine::Text*>(m_ui->GetChildByID("menu")->GetChildByID("money"))->SetText(ss.str());
	return true;
}

SpawnInfo::SpawnInfo(const std::string& script, size_t count, float delay) {
	Script = script;
	Count = count;
	Delay = delay;
}
