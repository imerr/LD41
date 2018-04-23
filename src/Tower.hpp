#ifndef LD41_TOWER_HPP
#define LD41_TOWER_HPP


#include <Engine/SpriteNode.hpp>

class Tower: public engine::SpriteNode {
protected:
	int m_attackSpeed;
	bool m_placeMode;
	bool m_placementBlocked;
	float m_range;
	int m_refund;
	std::unique_ptr<engine::BaseEventHandler> m_clickHandler;
	std::unique_ptr<engine::BaseEventHandler> m_beatHandler;
	int m_beats;
	Json::Value m_attackProjectile;
	bool m_faceMonster;
	int m_towerType;
public:
	Tower(engine::Scene* scene);
	virtual ~Tower();

	virtual bool initialize(Json::Value& root);

	void SetBuildMode();
	void SetRefund(int refund);

protected:
	virtual void OnUpdate(sf::Time interval);

	virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states, float delta);

	void Attack();

};


#endif //LD41_TOWER_HPP
