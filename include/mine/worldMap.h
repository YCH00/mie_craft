#include "PerlinNoise.hpp"
#include"mine/core.h"
#include <fstream>
#pragma once

#define W 16
#define L 16
#define H 32
#define R 6 //加载半径为R的个数的区块

enum CUBE {
	AIR,
	GRASS,
	STONE,
	DIRT,
    SNOW,
    SAND,
    WOOD,
	GLOWSTONE,
	NULLCUBE
};

struct Pos
{
	int x, y, z;
	bool operator<(const Pos& a)const{
		if(x != a.x)
			return x < a.x;
		if(y != a.y)
			return y < a.y;
		return z < a.z;
	}
	bool operator==(const Pos& a)const {
		return x == a.x && y == a.y && z == a.z;
	}
	bool operator!=(const Pos& a)const {
		return !(*this == a);
	}
};

// std::ostream 


class Block {
private:
	
public:
	Block(){}
	Block(const Pos& pos) {
		auto [x, y, z] = pos;
		//地图初始化
		const siv::PerlinNoise::seed_type seed = 123456u;

		const siv::PerlinNoise perlin{ seed };

		for (int i = 0; i < W; ++i)
		{
			for (int j = 0; j < L; ++j)
			{
				const double noise = perlin.octave2D_01((W * x + i) * 0.1, (L * y + j) * 0.1, 4);
				// std::cerr << "noise=" << noise << std::endl;
				// const double noise = 0.4;

				for (int k = 0; H * z + k <= int(noise*5); k++) {
					cubearray[i][j][k] = STONE;
				}
				for (int k = 0; k < H; k++)
				{
					model[i][j][k] = glm::translate(glm::mat4(1.0f), glm::vec3(i + (pos.x * W), k + (pos.z * H), j + (pos.y * L)));
				}
			}
		}
	}
	//我们使用一个128*128*10的数组管理地图


	CUBE cubearray[W][L][H] = {};
	glm::mat4 model[W][L][H];
};

class WorldMap {
public:
	std::map<Pos, Block*> wMap;
	Pos centerBlockPos = {-123, 123, -123};
	WorldMap(){ }
	~WorldMap();
	void eraseBlock(const Pos& blockPos);
	void addBlock(const Pos& blockPos);
	void update(const Pos& nowblockpos);
	Pos calculateBlockPos(const Pos& a); //计算当前位置是哪个区块

	CUBE& getCubeAt(Pos p);
	std::pair<Pos, Pos> getPointingCube(glm::vec3 position, glm::vec3 front, double radius);
};

Pos transWorldposToMappos(glm::vec3);
