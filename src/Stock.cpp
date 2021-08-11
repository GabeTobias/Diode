#include "Stock.h"

#include "Renderer.h"

#include "Shield.h"
#include "Movable.h"
#include "Collidable.h"

using namespace glm;

Stock::Stock(glm::vec2 pos, glm::vec2 velocity)
{
	className = "Stock";

	collider = new Collidable(vec2(0));
	AttachComponent(collider);

	rigidbody = new Movable(collider);
	AttachComponent(rigidbody);

	shield = new Shield();
	AttachComponent(shield);

	position = pos;
	rigidbody->velocity = velocity;
	shield->size = 20;
}

void Stock::Awake(){}

void Stock::Update()
{
	rigidbody->velocity = lerp(rigidbody->velocity, vec2(0), 0.2f);
}

void Stock::Draw()
{
	Render::DrawQuad(vec3(position,11), 0, vec2(0.5), vec4(1,1,1,0.5));
	Render::DrawCircle(vec3(position,11), vec2(0.5), vec4(1,1,1,0.5), true, 0.05);
}
