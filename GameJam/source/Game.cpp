#include "../header/Game.hpp"
#include "../header/TextureManager.hpp"
#include "../header/Map.hpp"
#include "../header/ECS/Components.hpp"
#include "../header/Vector2D.hpp"
#include"../header/Collision.hpp"

Map* map1;
Manager manager;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;

std::vector<ColliderComponent*> Game::colliders;

auto& player(manager.addEntity());
auto& tile(manager.addEntity());

enum groupLabels : std::size_t {
	groupMap,
	groupPlayers,
	groupEnemies,
	groupColliders
};


Game::Game() {

}

Game::~Game() {

}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
	int flags = 0;
	if (fullscreen) {
		flags = SDL_WINDOW_FULLSCREEN;
	}
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Subsystems Initialized. " << std::endl;
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (window)
			std::cout << "Window Created." << std::endl;
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			std::cout << "Renderer Created." << std::endl;
		}
		isRunning = true;
	}
	map1 = new Map(32, 18);

	int** map = map1->LoadMap();

	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 18; j++) {
			if (map[i][j] == 0) {
				tile.addComponent<TileComponent>(i*40, j*40, 40, 40, 0);
				tile.addComponent<ColliderComponent>("water");
			}
		}
	}
	//std::cout << map1->getStart().first << ", " << map1->getStart().first << std::endl;
	//player.addComponent<TransformComponent>(40*(map1->getStart().first), 40*(map1->getStart().first));
	player.addComponent<TransformComponent>(40 * (15), 40 * (9));
	player.addComponent<SpriteComponent>("assets/sprite1_anim.png", true);
	player.addComponent<KeyboardController>();
	player.addComponent<ColliderComponent>("player");
	player.addGroup(groupPlayers);

	/*wall.addComponent<TransformComponent>(300.0f, 300.0f, 300, 20, 1);
	wall.addComponent<SpriteComponent>("assets/dirt.png");
	wall.addComponent<ColliderComponent>("wall");
	wall.addGroup(groupMap);*/
}

void Game::handleEvents() {
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}
}

void Game::update() {
	Vector2D playerPos = player.getComponent<TransformComponent>().position;
	manager.refresh();
	manager.update();

	for (auto cc : colliders) {

		if (Collision::AABB(player.getComponent<ColliderComponent>(), *cc)) {
			player.getComponent<TransformComponent>().velocity * -1;
		}
	}
}

auto& tiles(manager.getGroup(groupMap));
auto& players(manager.getGroup(groupPlayers));
auto& enemies(manager.getGroup(groupEnemies));

void Game::render() {
	SDL_RenderClear(renderer);
	//add textures to be rendered in order
	//manager.draw();
	for (auto& t : tiles)
		t->draw();
	for (auto& p : players)
		p->draw();
	for (auto& e : enemies)
		e->draw();
	//
	SDL_RenderPresent(renderer);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game Cleared." << std::endl;
}

void Game::AddTile(int id, int x, int y) {
	auto& tile(manager.addEntity());
	tile.addComponent<TileComponent>(x, y, 40, 40, id);
	tile.addGroup(groupMap);
}