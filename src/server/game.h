
#include <algorithm>
#include <vector>

class Game {
    public:
        Game();
        bool MakeMove(int x, int y, int player);
        std::pair<int, int> AIMove();
        void Reset();

    private:
        bool CheckWin(int x, int y, int player);

        std::vector<std::pair<int, int>> GetEmptyCells();
        // three diffculty AI
        std::pair<int, int> EasyAI();
        std::pair<int, int> MediumAI();
        std::pair<int, int> HardAI();

		static const int directions_[4][2];

        int board_[15][15];
};
