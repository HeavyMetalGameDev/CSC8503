#pragma once
#include "NavigationMap.h"
#include <string>
#include <queue>
namespace NCL {
	namespace CSC8503 {
		struct GridNode {
			GridNode* parent;

			GridNode* connected[4];
			int		  costs[4];

			Vector3		position;

			float f;
			float g;

			int type;

			bool isOpen;
			bool isClosed;

			GridNode() {
				for (int i = 0; i < 4; ++i) {
					connected[i] = nullptr;
					costs[i] = 0;
				}
				f = 0;
				g = 0;
				type = 0;
				parent = nullptr;
				isOpen = false;
				isClosed = false;
			}
			~GridNode() {	}
			void ResetNode() {
				f = 0;
				g = 0;
				isOpen = false;
				isClosed = false;
			}
		};
		struct NodePointerComparator {
			bool operator()(GridNode* const& n1, GridNode* const& n2) {
				return n1->f < n2->f;
			}
		};
		class NavigationGrid : public NavigationMap	{
		public:
			NavigationGrid();
			NavigationGrid(const std::string&filename);
			~NavigationGrid();

			void ResetGrid() {
				for (int x = 0; x < gridWidth; x++) {
					for (int y = 0; y < gridHeight; y++) {
						allNodes[x * gridWidth + y].ResetNode();
					}
				}
			}

			bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) override;
				
		protected:
			bool		NodeInList(GridNode* n, std::vector<GridNode*>& list) const;
			GridNode*	RemoveBestNode(std::vector<GridNode*>& list) const;
			float		Heuristic(GridNode* hNode, GridNode* endNode) const;
			int nodeSize;
			int gridWidth;
			int gridHeight;

			GridNode* allNodes;
		};
	}
}

