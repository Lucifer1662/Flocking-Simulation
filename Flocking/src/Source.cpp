#include <SFML/Graphics.hpp>
#include <glm\glm.hpp>
#include <glm\gtx\compatibility.hpp>

const int numOfBirds = 100;
const float dt = 0.01f;
const int SCREEN_WIDTH = 600, SCREEN_HEIGHT = 600;
const float WorldWidth = 100;
const glm::vec2 maxBounds{ 50, 50 }, minBounds{-50, -50};




float rand_f(float min, float max) {
	float t = (float)std::rand()/RAND_MAX; // 0-1
	t = (max - min) * t; // 0 - (min-max)
	return t + min;
}

glm::vec2 rand2(float min, float max) {
	return glm::vec2(rand_f(min, max), rand_f(min, max));
}


template<int num>
struct Birds {
	float seperationDistance, alignmentDistance, cohesionDistance, seperationCoeffiecient, alignmentCoeffiecent, cohesionCoeffiecent;
	Birds(float seperationDistance, float alignmentDistance, float cohesionDistance, 
		float seperationCoeffiecient, float alighmentLerpSpeed, float cohesionCoeffiecent) : 
		seperationDistance(seperationDistance), alignmentDistance(alignmentDistance),
		cohesionDistance(cohesionDistance), seperationCoeffiecient(seperationCoeffiecient),
		alignmentCoeffiecent(alighmentLerpSpeed), cohesionCoeffiecent(cohesionCoeffiecent),
		verts(sf::Points, num) {
		
		for (size_t i = 0; i < num; i++)
		{
			positions[i] = rand2(-50, 50);
			velocities[i] = rand2(-4, 4);
			acceleration[i] = { 0,0 };
			verts[i].color = sf::Color::Red;
		}
	}

	glm::vec2 positions[num];
	glm::vec2 velocities[num];
	glm::vec2 acceleration[num];
	sf::VertexArray verts;

	static glm::vec2 average(glm::vec2 position, glm::vec2 positions[], glm::vec2 vecs[], float maxDistance) {
		glm::vec2 avg = { 0,0 };
		int count = 0;
		for (int i = 0; i < num; i++) {
			
			if (glm::distance(positions[i], position) < maxDistance) {
				avg += vecs[i];
				count++;
			}
		}
		avg.x /= count;
		avg.y /= count;
		return avg;
	}



	/*
	Separation - avoid crowding neighbours (short range repulsion)
	Alignment - steer towards average heading of neighbours
	Cohesion - steer towards average position of neighbours (long range attraction)
	*/
	static glm::vec2 normalise(glm::vec2 v) {
		if (v.x != 0 && v.y != 0)
			return glm::normalize(v);
		return v;
	}

	glm::vec2 seperation(glm::vec2 position) {
		glm::vec2 res = normalise(average(position, positions, positions, seperationDistance) - position);
		return res;
	}

	glm::vec2 alignment(glm::vec2 position, glm::vec2 velocity) {
		 auto avgVel = average(position, positions, velocities, alignmentDistance);
		 return normalise(avgVel);
		 return avgVel * (1 - glm::dot(avgVel, velocity));
	}

	glm::vec2 cohesion(glm::vec2 position) {
		return normalise(position - average(position, positions, positions, cohesionDistance));
	}

	void update() {
		for (size_t i = 0; i < num; i++)
		{
			glm::vec2 position = positions[i];
			velocities[i] += seperation(positions[i]) * seperationCoeffiecient;
			velocities[i] += alignment(positions[i], velocities[i]) * alignmentCoeffiecent;
			velocities[i] += cohesion(positions[i]) * cohesionCoeffiecent;
			//acceleration[i] *= 0.9f;
			//velocities[i] *= 0.99f;
			velocities[i] = glm::normalize(velocities[i]) * 30.0f;

			//velocities[i] += acceleration[i] * dt;
			//velocities[i] *= 4/glm::length(velocities[i]) ;



			position += velocities[i] * dt;
			if (position.x > maxBounds.x)
				position.x = minBounds.x;
			else if (position.x < minBounds.x)
				position.x = maxBounds.x;

			if (position.y > maxBounds.y)
				position.y = minBounds.y;
			else if (position.y < minBounds.y)
				position.y = maxBounds.y;

			positions[i] = position;
			verts[i].position.x = position.x;
			verts[i].position.y = position.y;
		}
	}
	
	void draw(sf::RenderWindow& window) {
		sf::Transform transform;
		transform.translate(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
		float xScale = SCREEN_WIDTH / WorldWidth;
		transform.scale(xScale, ((float)SCREEN_HEIGHT / SCREEN_WIDTH) * xScale);
		
		//window.draw(verts, transform);

		for (size_t i = 0; i < num; i++) {
			sf::CircleShape shape(1.f);
			shape.setFillColor(sf::Color::Green);
			shape.setPosition(verts[i].position);
			window.draw(shape, transform);

		}
		
		
	}


};




int main()
{
	sf::RenderWindow window(sf::VideoMode(600, 600), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);



	Birds<numOfBirds> birds(0.5f,5,5,1,1,1);




	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		birds.update();
		birds.draw(window);
		//window.draw(shape);
		window.display();
	}

	return 0;
}