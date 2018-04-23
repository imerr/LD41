#ifndef LD41_PROJECTILE_HPP
#define LD41_PROJECTILE_HPP
#include <Engine/SpriteNode.hpp>
#include <set>
#include "Monster.hpp"

class Projectile: public engine::SpriteNode {
protected:
	std::unique_ptr<engine::BaseEventHandler> m_collisionHandler;
	std::set<engine::Node*> m_damaged;
	int m_damage;
	int m_hits;
public:
	Projectile(engine::Scene* scene);
	virtual ~Projectile();

	virtual uint8_t GetType() const;

	void Damage(Monster* monster);

	virtual bool initialize(Json::Value& root);

};


#endif //LD41_PROJECTILE_HPP
