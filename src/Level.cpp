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
#include "Monster.hpp"
#include "LD41.hpp"

Level::Level(engine::Game* game) : Scene(game), m_bpm(0), m_inputIndicator(nullptr),
								   m_pressedInput{false, false, false, false}, m_currentRound(0), m_currentSpawn(0),
								   m_fighting(false),
								   m_spawnTimer(0), m_money(0), m_gameOver(false), TowerCount{0, 0, 0, 0} {
	m_keyHandler = m_game->OnKeyPress.MakeHandler([](const sf::Event::KeyEvent&, bool down) { return down; },
												  [this](const sf::Event::KeyEvent& e, bool down) {
													  if (!down) {
														  return false;
													  }
													  if (e.code == sf::Keyboard::Left || e.code == sf::Keyboard::A) {
														  m_pressedInput[0] = true;
													  } else if (e.code == sf::Keyboard::Down ||
																 e.code == sf::Keyboard::S) {
														  m_pressedInput[1] = true;
													  } else if (e.code == sf::Keyboard::Up ||
																 e.code == sf::Keyboard::K ||
																 e.code == sf::Keyboard::W) {
														  m_pressedInput[2] = true;
													  } else if (e.code == sf::Keyboard::Right ||
																 e.code == sf::Keyboard::L ||
																 e.code == sf::Keyboard::D) {
														  m_pressedInput[3] = true;
													  } else {
														  return false;
													  }
													  return true;
												  }, this);
}

Level::~Level() {
	m_game->OnKeyPress.RemoveHandler(m_keyHandler);
	// Need to do this here due to reverse destruction order
	while (m_children.size()) {
		delete m_children.front();
	}
}

bool Level::initialize(Json::Value& root) {
	if (!Scene::initialize(root)) {
		return false;
	}
	m_bpm = root["music"].get("bpm", 120).asInt();
	m_sounds.clear();
	for (auto& sound : root["music"]["sounds"]) {
		SoundInfo s(this);
		s.Sound.reset(engine::ResourceManager::instance()->MakeSound(sound.get("sound", "").asString()));
		s.Interval = sound.get("interval", 1).asFloat();
		s.Volume = sound.get("volume", 1).asFloat();
		s.Offset = sound.get("offset", 0).asFloat();
		s.Type = static_cast<BeatType>(sound.get("type", 0).asInt());
		for (const auto& r : sound["req"].getMemberNames()) {
			s.TowerRequirement[std::stoi(r)][0] = sound["req"][r].get(0u, 0).asInt();
			s.TowerRequirement[std::stoi(r)][1] = sound["req"][r].get(1u, 999).asInt();
		}
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
	if (m_gameOver) {
		return;
	}
	m_inputIndicator->SetActive(m_fighting);
	if (m_fighting) {
		m_spawnTimer += interval.asSeconds();
		auto monsterContainer = GetChildByID("monsters");
		if (m_currentSpawn >= m_rounds[m_currentRound].size() && monsterContainer->GetChildren().empty()) {
			m_fighting = false;
			m_currentRound++;
			m_currentSpawn = 0;
			m_spawnTimer = 0;
			for (auto& sound : m_sounds) {
				sound.Reset();
			}
			auto cat = static_cast<engine::SpriteNode*>(m_scene->GetChildByID("cat"));
			if (cat) {
				cat->SetColor(sf::Color::White);
			}
			auto menu = static_cast<Menu*>(GetUi()->GetChildByID("menu"));
			if (m_currentRound >= m_rounds.size()) {
				m_gameOver = true;
				m_ui->GetChildByID("win")->SetActive(true);
				if (!menu->IsCollapsed()) {
					menu->ToggleCollapse();
				}
				return;
			}
			if (menu->IsCollapsed()) {
				menu->ToggleCollapse();
			}
			std::ostringstream ss;
			ss << "Completed round " << m_currentRound << "\nAwarded TODO moneys\n";
			menu->SetDefaultDescription(ss.str());
			menu->ClearDescription(nullptr);
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
			static_cast<Monster*>(engine::Factory::CreateChildFromFile(spawn.Script,
																	   monsterContainer))->StartPathTween();
		}
		// Unscaled time!
		float beatTime = interval.asSeconds() / m_scene->GetGame()->GetTimeScale() / (60.0f / m_bpm);
		bool prevHit[4] = {false, false, false, false};
		float closestDistance[4] = {
				std::numeric_limits<float>::infinity(),
				std::numeric_limits<float>::infinity(),
				std::numeric_limits<float>::infinity(),
				std::numeric_limits<float>::infinity()
		};
		SoundInfo* closestSound[4] = {
				nullptr, nullptr, nullptr, nullptr
		};
		for (auto& sound : m_sounds) {
			sound.Update(beatTime);
			int t = (int) sound.Type;
			if (m_pressedInput[t]) {
				float distance =
						std::min(std::abs(sound.NextPlayTime()), std::abs(sound.LastPlayTime())) * (60.0f / m_bpm);
				bool prev =std::abs(sound.NextPlayTime()) > std::abs(sound.LastPlayTime());
				if (distance < closestDistance[t] && (!prev && !sound.IsCurrentHit() || prev && !sound.IsPrevHit()) && distance < HitLimit) {
					prevHit[t] = prev;
					closestDistance[t] = distance;
					closestSound[t] = &sound;
				}
			}
		}
		for (int i = 0; i < (int) BeatType::Max; i++) {
			if (m_pressedInput[i]) {
				sf::Color c = sf::Color::Blue;
				BeatQuality quality = BeatQuality::Perfect;
				if (closestSound[i]) {
					closestSound[i]->Hit(prevHit[i]);
					if (closestDistance[i] > 0.3) {
						c = sf::Color::Red;
						quality = BeatQuality::Miss;
					} else if (closestDistance[i] > 0.2) {
						c = sf::Color(255, 128, 0); // Orange
						quality = BeatQuality::Bad;
					} else if (closestDistance[i] > 0.1) {
						c = sf::Color::Yellow;
						quality = BeatQuality::Ok;
					} else if (closestDistance[i] > 0.05) {
						c = sf::Color::Green;
						quality = BeatQuality::Good;
					}
				} else {
					quality = BeatQuality::Miss;
				}
				if (quality == BeatQuality::Miss || quality == BeatQuality::Bad) {
					MakeTween<float>(true, 2.0f, 1.0, 3, [this](const float& val) {
						m_scene->GetGame()->SetTimeScale(val);
					});
				}
				std::string name;
				switch (quality) {
					case BeatQuality::Perfect:
						name = "perfect";
						break;
					case BeatQuality::Good:
						name = "good";
						break;
					case BeatQuality::Ok:
						name = "ok";
						break;
					case BeatQuality::Bad:
						name = "bad";
						break;
					case BeatQuality::Miss:
						name = "miss";
						break;
				}
				engine::SpriteNode* display = static_cast<engine::SpriteNode*>(m_inputIndicator->GetChildByID(name));
				if (display) {
					display->ClearTweens();
					display->MakeTween<sf::Color>(true, sf::Color::White, sf::Color(255, 255, 255, 0), 0.5,
												  [display](const sf::Color& col) {
													  auto color = col;
													  if (color.a > 128) {
														  color.a = 255;
													  } else {
														  color.a *= 2;
													  }
													  display->SetColor(color);
												  });
				}
				OnBeat.Fire(quality);
				m_inputIndicator->MakeTween<sf::Color>(false, c, sf::Color::White, 0.2f, [this](const sf::Color& c) {
					m_inputIndicator->SetColor(c);
				});
			}
			m_pressedInput[i] = false;
		}
	}
}

void Level::OnInitializeDone() {
	Node::OnInitializeDone();
	static_cast<engine::SpriteNode*>(GetUi()->GetChildByID("beat_indicator"));
	m_inputIndicator = static_cast<engine::SpriteNode*>(GetUi()->GetChildByID("input_indicator"));
	static_cast<engine::Button*>(m_ui->GetChildByID("game_over")->GetChildByID("restart"))->OnClick.MakeHandler(
			[this](engine::Button*, sf::Vector2f) {
				static_cast<LD41*>(m_game)->Restart();
			});
	static_cast<engine::Button*>(m_ui->GetChildByID("win")->GetChildByID("restart"))->OnClick.MakeHandler(
			[this](engine::Button*, sf::Vector2f) {
				static_cast<LD41*>(m_game)->Restart();

			});
	ChangeMoney(10);
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

void Level::GameOver() {
	m_gameOver = true;
	auto gameover = m_ui->GetChildByID("game_over");
	if (gameover) {
		gameover->SetActive(true);
	}
	auto menu = static_cast<Menu*>(GetUi()->GetChildByID("menu"));
	if (!menu->IsCollapsed()) {
		menu->ToggleCollapse();
	}
}

SpawnInfo::SpawnInfo(const std::string& script, size_t count, float delay) {
	Script = script;
	Count = count;
	Delay = delay;
}
