#include "PerlinNoise.hpp"
#include"mine/core.h"
#include <fstream>
#pragma once

#define W 16
#define L 16
#define H 64
#define R 6 //���ذ뾶ΪR�ĸ���������

enum CUBE {
	AIR,
	GRASS,
	STONE,
	DIRT,
    SNOW,
    SAND,
    WOOD,
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
		//��ͼ��ʼ��
		const siv::PerlinNoise::seed_type seed = 123456u;

		const siv::PerlinNoise perlin{ seed };

		for (int i = 0; i < W; ++i)
		{
			for (int j = 0; j < L; ++j)
			{
				const double noise = perlin.octave2D_01((W * x + i) * 0.1, (L * y + j) * 0.1, 4);
				// std::cerr << "noise=" << noise << std::endl;
				// const double noise = 0.4;

				for (int k = 0; H * z + k < int(noise*5); k++) {
					cubearray[i][j][k] = STONE;
				}
			}
		}
	}
	//����ʹ��һ��128*128*10�����������ͼ


	CUBE cubearray[W][L][H] = {};
};

class WorldMap {
public:
	std::map<Pos, Block*> wMap;
	Pos centerBlockPos = {-123, 123, -123};
	WorldMap(){ }
	void eraseBlock(const Pos& blockPos);
	void addBlock(const Pos& blockPos);
	void update(const Pos& nowblockpos);
	Pos calculateBlockPos(const Pos& a); //���㵱ǰλ�����ĸ�����

	CUBE& getCubeAt(Pos p);
	std::pair<Pos, Pos> getPointingCube(glm::vec3 position, glm::vec3 front, double radius);
};