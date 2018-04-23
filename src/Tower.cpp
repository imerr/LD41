//
// Created by iMer on 22.04.2018.
//

#include "Tower.hpp"
#include "Level.hpp"
#include "misc.hpp"
#include "Monster.hpp"
#include <Engine/Game.hpp>
#include <Engine/util/misc.hpp>
#include <Engine/Factory.hpp>

Tower::Tower(engine::Scene* scene) : SpriteNode(scene), m_placeMode(false), m_placementBlocked(false), m_beats(0) {
	m_clickHandler.reset(m_scene->GetGame()->OnMouseClick.MakeHandler(
			[this](const sf::Mouse::Button& button, const sf::Vector2f&, bool down) {
				return m_placeMode && !down &&
					   (!m_placementBlocked && button == sf::Mouse::Left || button == sf::Mouse::Right);
			}, [this](const sf::Mouse::Button& button, const sf::Vector2f&, bool down) {
				if (button == sf::Mouse::Right) {
					Delete();
					static_cast<Level*>(m_scene)->ChangeMoney(m_refund);
				}
				m_placeMode = false;
				return true;
			}, this));
	m_beatHandler.reset(
			static_cast<Level*>(m_scene)->OnBeat.MakeHandler([this](BeatQuality q) {
				if (q != BeatQuality::Miss) {
					m_beats++;
					if (q == BeatQuality::Perfect) {
						m_beats++;
					}
				}
			}));
}

void Tower::OnUpdate(sf::Time interval) {
	if (m_placeMode) {
		m_placementBlocked = false;
		auto pos = m_scene->GetGame()->GetMousePosition();
		SetPosition(pos);
		if (pos.x < 0 || pos.x > m_scene->GetSize().x || pos.y < 0 || pos.y > m_scene->GetSize().y) {
			m_placementBlocked = true;
			return;
		}
		b2AABB aabb;
		aabb.lowerBound.x = m_scene->PixelToMeter(pos.x - GetSize().x / 4);
		aabb.lowerBound.y = m_scene->PixelToMeter(pos.y - GetSize().y / 4);
		aabb.upperBound.x = m_scene->PixelToMeter(pos.x + GetSize().x / 4);
		aabb.upperBound.y = m_scene->PixelToMeter(pos.y + GetSize().y / 4);
		bool collided = false;
		auto callback = engine::MakeAABBQueryCallback([this, &collided](b2Fixture* fixture) {
			auto data = fixture->GetBody()->GetUserData();
			if (data && data != this) {
				collided = true;
				return false;
			}
			return true;
		});
		m_scene->GetWorld()->QueryAABB(&callback, aabb);
		if (collided) {
			m_placementBlocked = true;
			return;
		}
		// TODO: check if the tower is colliding with anything else
	} else {
		// Find target
		b2AABB aabb;
		auto pos = GetPosition();
		aabb.lowerBound.x = m_scene->PixelToMeter(pos.x - m_range);
		aabb.lowerBound.y = m_scene->PixelToMeter(pos.y - m_range);
		aabb.upperBound.x = m_scene->PixelToMeter(pos.x + m_range);
		aabb.upperBound.y = m_scene->PixelToMeter(pos.y + m_range);
		Monster * target = nullptr;
		auto callback = engine::MakeAABBQueryCallback([this, &target, pos](b2Fixture* fixture) {
			auto data = static_cast<Node*>(fixture->GetBody()->GetUserData());
			if (!data) {
				return true;
			}
			if (data->GetType() == static_cast<uint8_t>(LD41Type::Monster)) {
				auto* m = static_cast<Monster*>(data);
				if (engine::distance(pos, m->GetPosition()) > m_range) {
					return true;
				}
				// Always target the furthest monster
				if (!target || m->GetPathSegment() > target->GetPathSegment() ||
					m->GetPathSegment() == target->GetPathSegment() &&
					m->GetSegmentProgress() > target->GetSegmentProgress()) {
					target = m;
				}
			}
			return true;
		});
		m_scene->GetWorld()->QueryAABB(&callback, aabb);
		if (target) {
			if (m_faceMonster) {
				SetRotation(engine::b2Angle(GetPosition(), target->GetPosition()) * 180 / engine::fPI + 90);
			}
			while (m_beats >= m_attackSpeed) {
				Attack();
				m_beats -= m_attackSpeed;
			}
		} else {
			if (m_beats >= m_attackSpeed) {
				m_beats = m_attackSpeed - 1;
			}
		}
	}
}

void Tower::OnDraw(sf::RenderTarget& target, sf::RenderStates states, float delta) {
	if (m_placeMode) {
		sf::CircleShape c;
		if (m_placementBlocked) {
			c.setFillColor(sf::Color(100, 0, 0, 50));
		} else {
			c.setFillColor(sf::Color(0, 0, 0, 50));
		}
		c.setPosition(-m_range + getOrigin().x, -m_range + getOrigin().y);
		c.setRadius(m_range);
		target.draw(c, states);
	}
	SpriteNode::OnDraw(target, states, delta);
}

bool Tower::initialize(Json::Value& root) {
	if (!SpriteNode::initialize(root)) {
		return false;
	}
	m_attackSpeed = root.get("speed", 10).asInt();
	m_range = root.get("range", 50).asFloat();
	m_attackProjectile = root["projectile"];
	m_faceMonster = root.get("face_monster", true).asBool();
	m_towerType = root.get("tower_type", 0).asInt();
	static_cast<Level*>(m_scene)->TowerCount[m_towerType]++;
	return true;
}

void Tower::SetBuildMode() {
	m_placeMode = true;
}

void Tower::SetRefund(int refund) {
	m_refund = refund;
}

void Tower::Attack() {
	if (m_animations.find("attack") != m_animations.end()) {
		PlayAnimation("attack", "default");
	}
	if (!m_attackProjectile.isNull()) {
		int count = m_attackProjectile.get("count", 1).asInt();
		float spread = m_attackProjectile.get("spread", 0).asFloat();
		float velocity = m_attackProjectile.get("velocity", 10).asFloat();
		float spreadPer = spread / count;
		float angle = GetRotation() - 90 - (spread / 2);
		for (int i = 0; i < count; i++) {
			auto projectile = engine::Factory::CreateChild(m_attackProjectile, m_scene);
			projectile->SetPosition(GetPosition());
			projectile->SetRotation(angle);
			projectile->GetBody()->SetLinearVelocity(
					m_scene->PixelToMeter(sf::Vector2f(cosf(angle / 180 * engine::fPI) * velocity,
													   sinf(angle / 180 * engine::fPI) * velocity)));
			angle += spreadPer;
		}
	}
}

Tower::~Tower() {
	m_scene->GetGame()->OnMouseClick.RemoveHandler(m_clickHandler.get());
	static_cast<Level*>(m_scene)->OnBeat.RemoveHandler(m_beatHandler.get());
	static_cast<Level*>(m_scene)->TowerCount[m_towerType]--;
}
