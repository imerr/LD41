#ifndef LD41_TOWER_HPP
#define LD41_TOWER_HPP


#include <Engine/SpriteNode.hpp>

class Tower: public engine::SpriteNode {
protected:
	float m_attackSpeed;
	int m_damage;
	float m_damageDelay;
	bool m_placeMode;
	bool m_placementBlocked;
	float m_range;
	int m_refund;
	std::unique_ptr<engine::BaseEventHandler> m_clickHandler;
public:
	Tower(engine::Scene* scene);

	virtual bool initialize(Json::Value& root);

	void SetBuildMode();
	void SetRefund(int refund);

protected:
	virtual void OnUpdate(sf::Time interval);

	virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states, float delta);

};


#endif //LD41_TOWER_HPP
