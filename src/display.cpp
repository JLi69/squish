#include "display.hpp"
#include "assets.hpp"
#include "app.hpp"
#include <glm/gtc/matrix_transform.hpp>

float getZFromY(float y, float topy, float boty) {
	return (y - topy) / float(topy - boty);
}

void setupShader(const std::string &shader, int w, int h, float zoom) {
	ShaderProgram &shaderProgram = SHADERS->getShader(shader);
	shaderProgram.use();
	glm::mat4 windowMat = calculateWindowMat(float(w), float(h), zoom);
	shaderProgram.uniformMat4x4("windowMat", windowMat);
}

void setupShaderCam(const std::string &shader, int w, int h, float zoom, const glm::mat4 &camMat) {
	ShaderProgram &shaderProgram = SHADERS->getShader(shader);
	shaderProgram.use();
	glm::mat4 windowMat = calculateWindowMat(float(w), float(h), zoom);
	shaderProgram.uniformMat4x4("windowMat", windowMat * camMat);
}

void displayChunk(const gfx::Vao &tileVao, ShaderProgram &shader, glm::vec2 offset, float z) {
	tileVao.bind();
	shader.uniformFloat("z", z);
	shader.uniformVec2("offset", offset);
	glDrawElements(GL_TRIANGLES, tileVao.vertcount, GL_UNSIGNED_INT, 0);
}

void displayLevel(const TileVaos &tileVaos) {
	ShaderProgram &tileShader = SHADERS->getShader("tile_shader");
	TEXTURES->bindTexture("tiles", GL_TEXTURE0);	
	tileShader.uniformVec2("textureScale", glm::vec2(16.0f, 16.0f));
	// Draw level
	for(const auto &tileVao : tileVaos) {
		const std::pair<int, int> &coords = tileVao.first;
		const gfx::Vao &vao = tileVao.second;
		glm::vec2 offset(
			float(coords.first * CHUNK_SIZE),
			float(coords.second * CHUNK_SIZE)
		);
		displayChunk(vao, tileShader, offset, 0.0f);
	}
}

void displaySprite(const Sprite &sprite, glm::vec2 pos, const Level &level) {
	VAOS->bind("quad");
	ShaderProgram &shadowShader = SHADERS->getShader("shadow_shader");
	ShaderProgram &spriteShader = SHADERS->getShader("sprite_shader");
	
	// Display shadow
	if(sprite.drawShadow) {
		glm::vec3 shadowPos = glm::vec3(sprite.shadowOffset + pos, 1.0f);
		shadowShader.use();
		glm::mat4 shadowTransform = glm::mat4(1.0f);
		shadowTransform = glm::translate(shadowTransform, shadowPos);
		shadowTransform = glm::scale(shadowTransform, glm::vec3(sprite.shadowScale, 1.0f));
		shadowShader.uniformMat4x4("transform", shadowTransform);
		VAOS->draw();
	}

	// Display the sprite
	spriteShader.use();
	spriteShader.uniformBool("flipVert", sprite.flip);
	float displayy = pos.y + sprite.offset.y;
	float z = getZFromY(pos.y, float(level.getTopY()), float(level.getBottomY()));
	glm::vec3 displayPos = glm::vec3(pos.x + sprite.offset.x, displayy, z);
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, displayPos);
	transform = glm::scale(transform, glm::vec3(sprite.scale, 1.0f));
	transform = glm::rotate(transform, glm::radians(sprite.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	spriteShader.uniformMat4x4("transform", transform);
	TEXTURES->bindTexture(sprite.spriteTexId, GL_TEXTURE0);
	VAOS->draw();
}


void displayParticle(const Particle &particle, const Level &level) {
	VAOS->bind("quad");
	ShaderProgram &spriteShader = SHADERS->getShader("particle_shader");
	ShaderProgram &shadowShader = SHADERS->getShader("shadow_shader");
	
	// Display shadow
	if(particle.sprite.drawShadow) {
		glm::vec3 shadowPos = glm::vec3(particle.position.x, particle.floory, 1.0f);
		shadowPos += glm::vec3(particle.sprite.shadowOffset, 0.0f);
		shadowShader.use();
		glm::mat4 shadowTransform = glm::mat4(1.0f);
		shadowTransform = glm::translate(shadowTransform, shadowPos);
		shadowTransform = glm::scale(shadowTransform, glm::vec3(particle.sprite.shadowScale, 1.0f));
		shadowShader.uniformMat4x4("transform", shadowTransform);
		VAOS->draw();
	}

	spriteShader.use();
	spriteShader.uniformBool("flipVert", particle.sprite.flip);
	float displayy = particle.position.y + particle.sprite.offset.y;
	float z = getZFromY(particle.floory, float(level.getTopY()), float(level.getBottomY()));
	glm::vec3 displayPos = glm::vec3(particle.position.x + particle.sprite.offset.x, displayy, z);
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, displayPos);
	transform = glm::scale(transform, glm::vec3(particle.sprite.scale, 1.0f));
	transform = glm::rotate(transform, glm::radians(particle.sprite.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	spriteShader.uniformMat4x4("transform", transform);
	spriteShader.uniformVec4("color", particle.color);
	TEXTURES->bindTexture(particle.sprite.spriteTexId, GL_TEXTURE0);
	VAOS->draw();
}
