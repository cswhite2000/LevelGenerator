#ifndef PROJECT1_MAIN_H
#define PROJECT1_MAIN_H

#include <stdlib.h>
#include <string.h>
#include <curses.h>

#define MAX_X 180
#define MAX_Y 50

struct Room {
    int x;
    int y;
    int width;
    int height;
};

struct Pair {
    int first;
    int second;
};

struct PQueueNode {
    struct Pair data;
    float weight;
    struct PQueueNode * left;
    struct PQueueNode * right;
};

void freeTree(struct PQueueNode * root);
struct PQueueNode * removeMin(struct PQueueNode * root);
void addPair(struct PQueueNode * parent, struct Pair pair, float weight);
struct Pair getRandomCorridorStart(struct Room room);
void addAdjacentPoints(struct PQueueNode *root, struct PQueueNode *min, float **weights);
struct Pair getNext(struct Pair current, float ** distance);
int isValidPair(struct Pair pair);
struct Pair * makeRoomPairs(int numRooms);
struct Pair * makeRoomPairs(int numRooms);
void drawCorridor(int **map, struct Room *rooms, struct Pair pair, float **hardness);
void drawCorridors(int **map, struct Room * rooms, struct Pair * pairs);
char chooseChar(int **map, int x, int y);
void printMap(int **map);
int doesOverlap(struct Room room1, struct Room room2);
struct Room makeRoom(int xMin, int yMin, int xMax, int yMax);
int **makeArray(int rows, int columns, size_t typeSize);
int **makeLevel();

#endif //PROJECT1_MAIN_H
