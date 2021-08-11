#pragma once
#include <vector>
#include <chrono>

#include "glm/glm.hpp"

namespace Pathfinding
{
	using namespace glm;
	using namespace std;

	struct Node
	{
		vec2 position;
		vec2 parent = vec2(-1);

		bool operator == (const Node& lhs)
		{
			return lhs.position == position;
		}
	};

	inline vector<vec2> FindPath(vector<vector<short>> map, vec2 start, vec2 goal, int w, int h, int open = 0)
	{
		vector<Node> oldPoints;
		vector<Node> newPoints;

		newPoints.push_back({ start, vec2(-1) });

		while (newPoints.size() > 0)
		{
			bool foundTarget = false;
			Node finalNode;

			for (int i = 0; i < newPoints.size(); )
			{
				Node current = newPoints[i];

				oldPoints.push_back(newPoints[i]);
				newPoints.erase(newPoints.begin() + i);

				vec2 l = current.position + vec2(-1, 0);
				vec2 r = current.position + vec2(1, 0);
				vec2 u = current.position + vec2(0, 1);
				vec2 d = current.position + vec2(0, -1);

				////////////////////////////////////////////////////////////////////////////////
				//Adding New Points
				////////////////////////////////////////////////////////////////////////////////

				if (l.x >= 0 && map[l.x][l.y] == open)
				{
					Node n = { l, current.position };
					if (std::find(oldPoints.begin(), oldPoints.end(), n) == oldPoints.end())
						if (std::find(newPoints.begin(), newPoints.end(), n) == newPoints.end())
							newPoints.push_back(n);
				}

				if (r.x < w && map[r.x][r.y] == open)
				{
					Node n = { r, current.position };
					if (std::find(oldPoints.begin(), oldPoints.end(), n) == oldPoints.end())
						if (std::find(newPoints.begin(), newPoints.end(), n) == newPoints.end())
							newPoints.push_back(n);
				}

				if (u.y < h && map[u.x][u.y] == open)
				{
					Node n = { u, current.position };
					if (std::find(oldPoints.begin(), oldPoints.end(), n) == oldPoints.end())
						if (std::find(newPoints.begin(), newPoints.end(), n) == newPoints.end())
							newPoints.push_back(n);
				}

				if (d.y >= 0 && map[d.x][d.y] == open)
				{
					Node n = { d, current.position };
					if (std::find(oldPoints.begin(), oldPoints.end(), n) == oldPoints.end())
						if (std::find(newPoints.begin(), newPoints.end(), n) == newPoints.end())
							newPoints.push_back(n);
				}

				////////////////////////////////////////////////////////////////////////////////
				//Check if goal is found
				////////////////////////////////////////////////////////////////////////////////

				if (l == goal)
				{
					finalNode = { l, current.position };
					foundTarget = true;
				}

				if (r == goal)
				{
					finalNode = { r, current.position };
					foundTarget = true;
				}

				if (u == goal)
				{
					finalNode = { u, current.position };
					foundTarget = true;
				}

				if (d == goal)
				{
					finalNode = { d, current.position };
					foundTarget = true;
				}
			}

			//Find Inverted path
			if (foundTarget)
			{
				Node current = finalNode;
				vector<vec2> invertedPath;

				while (current.parent != vec2(-1))
				{
					//Adding to path
					invertedPath.push_back(current.position);

					//Find Parent
					for (int i = 0; i < oldPoints.size(); i++)
						if (oldPoints[i].position == current.parent) current = oldPoints[i];
				}

				//Invert path
				std::reverse(invertedPath.begin(), invertedPath.end());

				//Return path
				return invertedPath;
			}
		}

		return vector<vec2>();
	}

	inline void FloodFillTest()
	{
		using std::chrono::high_resolution_clock;
		using std::chrono::duration_cast;
		using std::chrono::duration;
		using std::chrono::milliseconds;

		vector<vector<short>> map =
		{
			{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 },
			{ 1, 1, 1, 0, 1, 1, 1, 0, 1, 1 },
			{ 1, 1, 1, 0, 1, 1, 0, 1, 0, 1 },
			{ 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
			{ 1, 1, 1, 0, 1, 1, 1, 0, 1, 0 },
			{ 1, 0, 1, 1, 1, 1, 0, 1, 0, 0 },
			{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
			{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
			{ 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 }
		};

		auto t1 = high_resolution_clock::now();
		vector<vec2> path = FindPath(map, { 8,9 }, { 0,0 }, 9, 10, 1);
		auto t2 = high_resolution_clock::now();

		/* Getting number of milliseconds as a double. */
		duration<double, std::milli> ms_double = t2 - t1;

		printf("Path Time: %f \n", ms_double.count());

		if (path.size() > 0) {
			printf("Path Found %d \n", path.size());
			for (int i = 0; i < path.size(); i++)
				printf("%d { %f, %f } \n", path.size(), path[i].x, path[i].y);
		}

		else printf("Path Error \n");
	}
}