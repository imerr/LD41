#ifndef LD41_MONSTER_HPP
#define LD41_MONSTER_HPP

#include <Engine/SpriteNode.hpp>

class Monster : public engine::SpriteNode {
protected:
	size_t m_pathSegment;
	float m_speed;
	std::string m_deathSpawn;
	size_t m_deathSpawnCount;
	int m_hp;
	bool m_dead;
	engine::EventHandlerWrapper<void, engine::Tween<sf::Vector2f>*> m_onDone;
	sf::Vector2<float> m_randomOffset;
	Json::Value m_deathDecal;
	float m_immune;
	int m_money;
public:
	Monster(engine::Scene* scene);

	virtual uint8_t GetType() const;

	virtual bool initialize(Json::Value& root);

	size_t GetPathSegment();

	float GetSegmentProgress();

	bool Damage(int damage);
	void StartPathTween();
	bool IsDead() {
		return m_hp <= 0;
	}
private:
	virtual void OnUpdate(sf::Time interval);



	void Dead();

	virtual void OnInitializeDone();


	void ReachEnd();

};

#endif //LD41_MONSTER_HPP