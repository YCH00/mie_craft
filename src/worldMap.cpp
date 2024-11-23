#include"mine/worldMap.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, const Pos& p)
{
    out << p.x << "," << p.y << "," << p.z;
    return out;
}

WorldMap::~WorldMap()
{
    for (auto [pos, block] : wMap)
    {
        eraseBlock(pos);
    }
}

void WorldMap::eraseBlock(const Pos& blockPos)
{
    std::ofstream fout("save/worldmap/worldmap_" + std::to_string(blockPos.x) + "_" + std::to_string(blockPos.y) + "_" + std::to_string(blockPos.z) + ".worldmap");
    Block *block = wMap[blockPos];
    for (int i = 0; i < W; i++)
    {
        for (int j = 0; j < L; j++)
        {
            for (int k = 0; k < H; k++)
            {
                fout << (int)block->cubearray[i][j][k] << " ";
            }
            fout << "\n";
        }
        fout << "\n";
    }
    fout.close();
    delete block;
    wMap.erase(blockPos);
}
void WorldMap::addBlock(const Pos& blockPos)
{
    std::ifstream fin("save/worldmap/worldmap_" + std::to_string(blockPos.x) + "_" + std::to_string(blockPos.y) + "_" + std::to_string(blockPos.z) + ".worldmap");
    if(fin.is_open())//如果成功打开了，就直接加载已经存储的地图
    {
        Block *block = new Block;
        for (int i = 0; i < W; i++)
        {
            for (int j = 0; j < L; j++)
            {
                for (int k = 0; k < H; k++)
                {
                    int x;
                    fin >> x;
                    block->cubearray[i][j][k] = (CUBE)x;
                }
            }
        }
        wMap[blockPos] = block;
    }
    else//否则根据柏林噪声新建一块
    {
        wMap[blockPos] = new Block(blockPos);
    }
}

void WorldMap::update(const Pos& nowblockpos)
{
    if(nowblockpos == centerBlockPos)
        return;
    // std::cerr << "update" << std::endl;
    //先找所有需要删除的块
    std::vector<Pos> toerase;
    for (auto [p, _] : wMap)
    {
        // std::cerr << "p:" << p.x << " " << p.y << " " << p.z << std::endl;
        if(abs(p.x - nowblockpos.x) > R || abs(p.y - nowblockpos.y) > R)
        {
            toerase.push_back(p);
        }
    }
    for (const Pos& p : toerase)
    {
        eraseBlock(p);
    }
    //添加所有需要添加的块
    for (int x = nowblockpos.x - R; x <= nowblockpos.x + R; x++)
    {
        for (int y = nowblockpos.y - R; y <= nowblockpos.y + R; y++)
        {
            // for (int z = nowblockpos.z - R; z <= nowblockpos.z + R; z++)
            // {
                if(!wMap.count({x, y, 0}))
                {
                    addBlock({x, y, 0});
                }
            // }
        }
    }
    centerBlockPos = nowblockpos;
}

Pos WorldMap::calculateBlockPos(const Pos& a) //计算当前位置是哪个区块
{
    // std::cerr << "a: " << a.x << " " << a.y << " " << a.z << std::endl;
    Pos ans;
    if(a.x >= 0)
        ans.x = a.x / W;
    else
        ans.x = (a.x - W + 1) / W;
    if(a.y >= 0)
        ans.y = a.y / L;
    else
        ans.y = (a.y - L + 1) / L;
    // if(a.z >= 0)
    // 	ans.z = a.z / H;
    // else
    // 	ans.z = (a.z - H + 1) / H;
    ans.z = 0;//高度不按照区块，所以固定
    // std::cerr << "ans: " << ans.x << " " << ans.y << " " << ans.z << std::endl;
    return ans;
}

CUBE& WorldMap::getCubeAt(Pos p)
{
    Pos block = calculateBlockPos(p);
    Pos left = {p.x - block.x * W, p.y - block.y * L, p.z - block.z * H};
    return wMap[block]->cubearray[left.x][left.y][left.z];
}
/**
 * 给定相机的位置、方向、手臂的半径，返回指向方块的引用和指向可以放置方块的位置
 * 如果有效，返回应该是{第一个碰到方块的位置，放置方块的位置}，可以通过getCubeAt函数得到对应方块的引用然后修改
 * 如果没找到方块，也会同样返回半径处最远的那一对，所以可以通过getCubeAt判断碰到的方块是否为AIR来判断是否有效
 */
std::pair<Pos, Pos> WorldMap::getPointingCube(glm::vec3 position, glm::vec3 front, double radius)
{
    //先把y向上改为z向上，与逻辑的pos一致
    std::swap(position.y, position.z);
    std::swap(front.y, front.z);
    // std::cerr << std::endl;
    // std::cerr << position.x << " " << position.y << " " << position.z << std::endl;
    // std::cerr << front.x << " " << front.y << " " << front.z << std::endl;

    //首先确定position所在的方块的pos
    Pos beginpos;
    //需要区分正负，比如在0.5实际上是在0 - 1之间的方块，逻辑上存储在0位置
    //而如果是负的，比如-0.5实际上在-1 - 0之间，逻辑上存储在-1位置
    beginpos.x = position.x >= 0 ? position.x : position.x - 1;
    beginpos.y = position.y >= 0 ? position.y : position.y - 1;
    beginpos.z = position.z >= 0 ? position.z : position.z - 1;
    std::pair<Pos, Pos> ans = {beginpos, beginpos};
    int sign_x = front.x > 0 ? 1 : front.x == 0 ? 0 : -1;
    int sign_y = front.y > 0 ? 1 : front.y == 0 ? 0 : -1;
    int sign_z = front.z > 0 ? 1 : front.z == 0 ? 0 : -1;
    //每次向三个方向相邻的方块做尝试

    Pos nowpos = beginpos;
    double x0 = position.x, y0 = position.y, z0 = position.z;
    double xf = front.x, yf = front.y, zf = front.z;

    while (getCubeAt(ans.first) == AIR && std::hypot(nowpos.x - beginpos.x, nowpos.y - beginpos.y, nowpos.z - beginpos.z) <= radius)
    {
        // std::cerr << nowpos.x << " " << nowpos.y << " " << nowpos.z << std::endl;
        auto [x1, y1, z1] = nowpos;
        //如何计算这条射线是否经过某一个方块呢？以上方向为例，例如当前正在x1,y1,z1，那么上表面就是z = z1 + 1,
        //我们根据比例计算这条射线与这个平面的交点，如果交点在x1 - x1+1, y1 - y1+1的范围，就说明与上面的方块相交
        //上方向
        bool intersectWithUpCube = 0;
        if(sign_z == 1)
        {
            double dz = z1 + 1 - z0;
            double x2 = x0 + dz * xf / zf;
            double y2 = y0 + dz * yf / zf;
            if(x2 >= x1 && x2 <= x1 + 1 && y2 >= y1 && y2 <= y1 + 1)
                intersectWithUpCube = 1;
        }
        if(intersectWithUpCube)
        {
            nowpos.z++;
            ans.second = ans.first;
            ans.first = nowpos;
            continue;
        }
        //下方向
        bool intersectWithDown = 0;
        if(sign_z == -1)
        {
            double dz = z1 - z0;
            double x2 = x0 + dz * xf / zf;
            double y2 = y0 + dz * yf / zf;
            if(x2 >= x1 && x2 <= x1 + 1 && y2 >= y1 && y2 <= y1 + 1)
                intersectWithDown = 1;
        }
        if(intersectWithDown)
        {
            nowpos.z--;
            ans.second = ans.first;
            ans.first = nowpos;
            continue;
        }
        //x正方向
        bool intersectWithRight = 0;
        if(sign_x == 1)
        {
            double dx = x1 + 1 - x0;
            double y2 = y0 + dx * yf / xf;
            double z2 = z0 + dx * zf / xf;
            if(y2 >= y1 && y2 <= y1 + 1 && z2 >= z1 && z2 <= z1 + 1)
                intersectWithRight = 1;
        }
        if(intersectWithRight)
        {
            nowpos.x++;
            ans.second = ans.first;
            ans.first = nowpos;
            continue;
        }
        //x负方向
        bool intersectWithLeft = 0;
        if(sign_x == -1)
        {
            double dx = x1 - x0;
            double y2 = y0 + dx * yf / xf;
            double z2 = z0 + dx * zf / xf;
            if(y2 >= y1 && y2 <= y1 + 1 && z2 >= z1 && z2 <= z1 + 1)
                intersectWithLeft = 1;
        }
        if(intersectWithLeft)
        {
            nowpos.x--;
            ans.second = ans.first;
            ans.first = nowpos;
            continue;
        }
        //y正方向
        bool intersectWithFront = 0;
        if(sign_y == 1)
        {
            double dy = y1 + 1 - y0;
            double x2 = x0 + dy * xf / yf;
            double z2 = z0 + dy * zf / yf;
            if(x2 >= x1 && x2 <= x1 + 1 && z2 >= z1 && z2 <= z1 + 1)
                intersectWithFront = 1;
        }
        if(intersectWithFront)
        {
            nowpos.y++;
            ans.second = ans.first;
            ans.first = nowpos;
            continue;
        }
        //y负方向
        bool intersectWithBack = 0;
        if(sign_y == -1)
        {
            double dy = y1 - y0;
            double x2 = x0 + dy * xf / yf;
            double z2 = z0 + dy * zf / yf;
            if(x2 >= x1 && x2 <= x1 + 1 && z2 >= z1 && z2 <= z1 + 1)
                intersectWithBack = 1;
        }
        if(intersectWithBack)
        {
            nowpos.y--;
            ans.second = ans.first;
            ans.first = nowpos;
            continue;
        }
        assert(1);//按道理来说上面六种总会有一个符合的，不应该走到这里
    }
    return ans;
}

Pos transWorldposToMappos(glm::vec3 p)
{
    Pos ans;
    if(p.x >= 0)
        ans.x = p.x;
    else
        ans.x = p.x - 1;
    if(p.y >= 0)
        ans.y = p.y;
    else
        ans.y = p.y - 1;
    if(p.z >= 0)
        ans.z = p.z;
    else
        ans.z = p.z - 1;
    std::swap(ans.y, ans.z);
    return ans;
}