#ifndef LD41_LEVEL_HPP
#define LD41_LEVEL_HPP

#include <Engine/Scene.hpp>
#include <SFML/Audio/Sound.hpp>
#include <Engine/SpriteNode.hpp>
#include "SoundInfo.hpp"
#include "BeatQuality.hpp"

class SpawnInfo {
public:
	SpawnInfo(const std::string& script, size_t count, float delay);

	std::string Script;
	size_t Count;
	float Delay;
};

class Level : public engine::Scene {
protected:
	std::vector<SoundInfo> m_sounds;
	int m_bpm;
	engine::SpriteNode* m_inputIndicator;
	engine::EventHandler<bool, const sf::Event::KeyEvent&, bool>* m_keyHandler;
	bool m_pressedInput[4];
	std::vector<sf::Vector2f> m_path;
	std::vector<std::vector<SpawnInfo>> m_rounds;
	size_t m_currentRound;
	size_t m_currentSpawn;
	bool m_fighting;
	float m_spawnTimer;
	int m_money;
	bool m_gameOver;
public:
	explicit Level(engine::Game* game);
	virtual ~Level();

	virtual void OnInitializeDone();
	int GetBPM() {
		return m_bpm;
	}

	const std::vector<sf::Vector2f>& GetPath() {
		return m_path;
	}

	bool IsFighting() {
		return m_fighting;
	}
	void SetFighting(bool fighting) {
		m_fighting = fighting;
	}

protected:
	virtual void OnUpdate(sf::Time interval);

public:
	virtual bool initialize(Json::Value& root);


	std::vector<SoundInfo>& GetSounds();

	static constexpr float HitLimit = 0.5f;

	bool ChangeMoney(int money);

	engine::Event<BeatQuality> OnBeat;

	void GameOver();

	int TowerCount[4];
};


#endif //LD41_LEVEL_HPP
