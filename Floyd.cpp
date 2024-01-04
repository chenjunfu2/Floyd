#include <new>
class Floyd
{
private:
	size_t *pszWpTable;//路径点表,wp代表waypoint
	size_t szWpCount;//路径点个数,表大小是它的平方
public:
	constexpr const static size_t Infinite = (size_t)-1;
public:
	Floyd(size_t _szWpCount) :pszWpTable(nullptr), szWpCount(_szWpCount)
	{
		pszWpTable = new size_t[szWpCount * szWpCount];//一维线性数组模拟二维数组

		//对角线0初始化，其余位置为无限
		for (size_t y = 0; y < szWpCount; ++y)
		{
			for (size_t x = 0; x < szWpCount; ++x)
			{
				pszWpTable[y * szWpCount + x] = (x == y) ? 0 : Infinite;//对角线为0(点位到自身长度为0)
			}
		}
	}

	~Floyd(void)
	{
		delete[] pszWpTable;
		pszWpTable = nullptr;
		szWpCount = 0;
	}

private:
	//全部禁止
	Floyd &operator=(const Floyd &) = delete;
	Floyd &operator=(Floyd &&) = delete;

	Floyd(const Floyd &) = delete;
	Floyd(Floyd &&) = delete;

public:
	size_t GetWpCount(void)
	{
		return szWpCount;
	}

	size_t &GetLong(size_t szSrc, size_t szDst)//src(y)->dst(x)
	{
		return pszWpTable[szSrc * szWpCount + szDst];
	}

	void CalculateShortestLong(void)//计算最短长度
	{
		//开始迭代
		for (size_t i = 0; i < szWpCount; ++i)
		{
			IterationFloydTable(i);
		}
	}
private:
	void IterationFloydTable(const size_t szCurrentElem)//中转点
	{
		for (size_t y = 0; y < szWpCount; ++y)
		{
			if (y == szCurrentElem)//szCurrentElem代表当前迭代的行列，也就是当前的中转点，中转点在本次迭代中不改变
			{
				continue;
			}

			for (size_t x = 0; x < szWpCount; ++x)
			{
				if (x == y || x == szCurrentElem)//因为斜向的位置（点到自身距离）永不改变，所以也无需计算，中转点也是如此
				{
					continue;
				}
				//计算剩余符合条件的点
				//即当前点的xy比中转点相交的两个值的和大则使用其更新当前点，否则略过
				size_t szIntersectionX = pszWpTable[szCurrentElem * szWpCount + x];//x方向上的交点(x不变)
				size_t szIntersectionY = pszWpTable[y * szWpCount + szCurrentElem];//y方向上的交点(y不变)

				if (szIntersectionX == Infinite || szIntersectionY == Infinite)//只要其中一个是无限，那就肯定不会比当前点小，直接continue
				{
					continue;
				}

				size_t szNewDistance = szIntersectionX + szIntersectionY;//计算从中转点到目标点的距离
				//如果更小，则更新否则略过
				if (szNewDistance < pszWpTable[y * szWpCount + x])
				{
					pszWpTable[y * szWpCount + x] = szNewDistance;
				}
			}
		}
	}
};

#include <unordered_map>
#include <string>
#include <stdio.h>
#include <iostream>

int main(void)
{
	//创建映射表，映射地名到表下标
	std::unordered_map<std::string, size_t> umPlaceMap;//sPlaceName->szTableIndex


	std::string sIpt;


	size_t szWpCount;
	printf("输入路径点个数:\n");
	while (true)
	{
		std::getline(std::cin, sIpt, '\n');
		if (sscanf(sIpt.c_str(), "%zu", &szWpCount) != 1)
		{
			printf("输入错误,请重新输入:");
			continue;
		}

		break;
	}

	//建表
	Floyd flod(szWpCount);

	printf("输入%zu个地名，一行一个\n", flod.GetWpCount());

	for (size_t i = 0; i < flod.GetWpCount(); ++i)
	{
		printf("下标[%d]:", i);
		std::getline(std::cin, sIpt, '\n');
		umPlaceMap.insert({ sIpt, i });
	}


	printf("依次输入地名中的相对距离,不输入的距离默认为无限\n用空行结束输入,输入格式[起始点下标 目标点下标 距离值]\n");
	size_t szIptSrc, szIptDst, szIptLong;
	while (true)
	{
		printf(":");
		std::getline(std::cin, sIpt, '\n');
		int ret = sscanf(sIpt.c_str(), "%zu%zu%zu", &szIptSrc, &szIptDst, &szIptLong);
		if (ret == -1)
		{
			break;
		}
		else if (ret != 3)
		{
			printf("输入错误，请重新输入:\n");
			continue;
		}

		if (szIptSrc < 0 || szIptSrc >= flod.GetWpCount() ||
			szIptDst < 0 || szIptDst >= flod.GetWpCount())
		{
			printf("下标超过范围，请重新输入:\n");
			continue;
		}
		if (szIptSrc == szIptDst)
		{
			printf("两个位置相同，请重新输入:\n");
			continue;
		}

		flod.GetLong(szIptSrc, szIptDst) = flod.GetLong(szIptDst, szIptSrc) = szIptLong;//因为是无向图所以双向通路
	}

	//迭代计算最短路径
	flod.CalculateShortestLong();

	printf("输入起始点和目的地，一行一个\n");
	std::string sSrc, sDst;
	while (true)
	{
		printf("起始点:");
		std::getline(std::cin, sSrc, '\n');

		auto findSrc = umPlaceMap.find(sSrc);
		if (findSrc == umPlaceMap.end())
		{
			printf("未知地点:%s\n", sSrc.c_str());
			continue;
		}
		size_t szSrcIndex = findSrc->second;

		printf("目的地:");
		std::getline(std::cin, sDst, '\n');

		auto findDst = umPlaceMap.find(sDst);
		if (findDst == umPlaceMap.end())
		{
			printf("未知地点:%s\n", sDst.c_str());
			continue;
		}
		size_t szDstIndex = findDst->second;

		size_t szLong = flod.GetLong(szSrcIndex, szDstIndex);
		if (szLong == flod.Infinite)
		{
			printf("长度为:无限\n");
		}
		else
		{
			printf("长度为:%zu\n", szLong);
		}
	}

	return 0;
}

/*
输入数据:
5
0
1
2
3
4
0 1 20
1 2 30
2 3 50
3 0 40
2 4 60
3 4 70


*/