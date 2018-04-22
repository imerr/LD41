//
// Created by iMer on 21.04.2018.
//

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
public:
	Monster(engine::Scene* scene);

	virtual bool initialize(Json::Value& root);
private:
	virtual void OnUpdate(sf::Time interval);


	void StartPathTween();

	void Damage(int damage);

	void Dead();

	virtual void OnInitializeDone();


	void ReachEnd();
};


#endif //LD41_MONSTER_HPP
