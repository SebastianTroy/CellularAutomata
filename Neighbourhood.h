#ifndef NEIGHBOURHOOD_H
#define NEIGHBOURHOOD_H

#include <vector>
#include <assert.h>

class Neighbourhood {
public:
    static std::vector<std::pair<int, int>> CreateNeighbourhoodCoordinates(std::vector<std::vector<int>>&& bitmap)
    {
        assert(bitmap.size() % 2 == 1);
        std::vector<std::pair<int, int>> coordinates;
        for (unsigned row = 0; row < bitmap.size(); row++) {
            assert(bitmap[row].size() == bitmap[0].size());
            assert(bitmap[row].size() % 2 == 1);
            for (unsigned column = 0; column < bitmap[row].size(); column++) {
                if (bitmap[row][column] == 1) {
                    int xOffset = row - (bitmap.size() / 2);
                    int yOffset = column - (bitmap[row].size() / 2);
                    coordinates.push_back(std::make_pair(xOffset, yOffset));
                } else {
                    assert(bitmap[row][column] == 0);
                }
            }
        }
        return coordinates;
    }
};

#endif // NEIGHBOURHOOD_H
