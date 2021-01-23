#include <time.h>
#include <math.h>
#include "main.h"

int main(void) {

    srand(time(0));

    initscr();
    clear();
    noecho();

    while (true) {
        int **map = makeLevel();

        printMap(map);

        refresh();
        char result = getch();
        free(map);

        if (result == 'q') {
            break;
        }
    }

    endwin();

    return 0;
}

char chooseChar(int **map, int x, int y) {
    if (map[x][y] == 2) {
        return '#';
    } else if (map[x][y] == 1) {
        return '.';
    } else if (map[x][y] == 0) {
        if (x > 0) {
            if (map[x - 1][y] == 1) {
                return '|';
            }
        }
        if (x < MAX_X - 1) {
            if (map[x + 1][y] == 1) {
                return '|';
            }
        }
        if (y > 0) {
            if (map[x][y - 1] == 1) {
                return '-';
            }
            if (x > 0) {
                if (map[x - 1][y - 1] == 1) {
                    return '-';
                }
            }
            if (x < MAX_X - 1) {
                if (map[x + 1][y - 1] == 1) {
                    return '-';
                }
            }
        }
        if (y < MAX_Y - 1) {
            if (map[x][y + 1] == 1) {
                return '-';
            }
            if (x > 0) {
                if (map[x - 1][y + 1] == 1) {
                    return '-';
                }
            }
            if (x < MAX_X - 1) {
                if (map[x + 1][y + 1] == 1) {
                    return '-';
                }
            }
        }
    }
    return ' ';
}

void printMap(int **map) {
    for (int i = 0; i < MAX_Y; i++) {
        for (int j = 0; j < MAX_X; j++) {
            mvaddch(i, j, chooseChar(map, j, i));
//            if (map[j][i] == 0) {
//                mvaddch(i, j, chooseChar(map, j, i));
//            } else if (map[j][i] == 1) {
//                mvprintw(i, j, ".");
//            }
        }
    }
}

struct Room makeRoom(int xMin, int yMin, int xMax, int yMax) {
    struct Room room;

    room.width = rand() % 12 + 4;
    if (room.width >= xMax - xMin) {
        room.width = xMax - xMin - 1;
    }

    room.height = rand() % 4 + 4;
    if (room.height >= yMax - yMin - 1) {
        room.height = yMax - yMin;
    }

    xMax -= room.width;
    yMax -= room.height;

    int range = xMax - xMin - room.width;
    if (range <= 0) {
        room.x = xMin;
    } else {
        room.x = xMin + rand() % (xMax - xMin - room.width);
    }
    range = yMax - yMin - room.height;
    if (range <= 0) {
        room.y = yMin;
    } else {
        room.y = yMin + rand() % (yMax - yMin - room.height);
    }

    return room;
}

int doesOverlap(struct Room room1, struct Room room2) {
    int **tempLevel = makeArray(MAX_X, MAX_Y, sizeof(int));

    for (int x = -2; x <= room1.width + 1; x++) {
        for (int y = -2; y <= room1.height + 1; y++) {
            if (room1.x + x >= 0 && room1.x + x < MAX_X && room1.y + y >= 0 && room1.y + y < MAX_Y) {
                tempLevel[room1.x + x][room1.y + y] = 1;
            }
        }
    }

    for (int x = -2; x <= room2.width + 1; x++) {
        for (int y = -2; y <= room2.height + 1; y++) {
            if (tempLevel[room2.x + x][room2.y + y] == 1) {
                free(tempLevel);
                return 1;
            }
        }
    }

    free(tempLevel);
    return 0;
}

struct Pair * makeRoomPairs(int numRooms) {
    int * unPairedRooms = calloc(numRooms, sizeof(int));
    int * pairedRooms = calloc(numRooms, sizeof(int));
    for (int i = 0 ; i < numRooms; i++) {
        unPairedRooms[i] = i;
    }

    struct Pair * pairs = calloc(numRooms + 1, sizeof(struct Pair));

    struct Pair firstPair;
    firstPair.first = unPairedRooms[numRooms - 1];
    firstPair.second = unPairedRooms[numRooms - 2];
    pairedRooms[0] = firstPair.first;
    pairedRooms[1] = firstPair.second;

    pairs[0] = firstPair;

    int roomsPaired = 2;
    int index = 1;

    for (int roomsLeft = numRooms - 2; roomsLeft > 0; roomsLeft--) {
        struct Pair pair;
        pair.first = pairedRooms[rand() % (roomsPaired)];
        int secondIndex = rand() % roomsLeft;
        pair.second = unPairedRooms[secondIndex];

        for (int i = secondIndex; i < roomsLeft; i++) {
            unPairedRooms[i] = unPairedRooms[i + 1];
        }
        pairedRooms[roomsPaired] = pair.second;

        pairs[index] = pair;
        index++;
        roomsPaired++;
    }

    free(unPairedRooms);
    free(pairedRooms);
    return pairs;
}

struct PQueueNode * removeMin(struct PQueueNode * root) {
    if (root->left == NULL) {
        if (root->right == NULL) {
            return root;
        }
        struct PQueueNode * node = malloc(sizeof(struct PQueueNode));
        node->left = root->right->left;
        node->right = root->right->right;
        node->data = root->right->data;
        node->weight = root->right->weight;

//        *root = *root->right;
        memcpy(root, root->right, sizeof(struct PQueueNode));
        return node;
    }
    if (root->left->left == NULL && root->left->right == NULL) {
        struct PQueueNode * smallest = root->left;
        root->left = NULL;
        return smallest;
    }
    if (root->left->left == NULL) {
        struct PQueueNode * smallest = root->left;
        root->left = root->left->right;
        return smallest;
    }



    return removeMin(root->left);
}

void addPair(struct PQueueNode * parent, struct Pair pair, float weight) {
    if (weight < parent->weight) {
        if (parent->left == NULL) {
            struct PQueueNode * newNode = malloc(sizeof(struct PQueueNode));
            newNode->left = NULL;
            newNode->right = NULL;
            newNode->weight = weight;
            newNode->data = pair;
            parent->left = newNode;
            return;
        } else {
            addPair(parent->left, pair, weight);
        }
    } else {
        if (parent->right == NULL) {
            struct PQueueNode * newNode = malloc(sizeof(struct PQueueNode));
            newNode->left = NULL;
            newNode->right = NULL;
            newNode->weight = weight;
            newNode->data = pair;
            parent->right = newNode;
            return;
        } else {
            addPair(parent->right, pair, weight);
        }
    }
}

void drawCorridor(int **map, struct Room *rooms, struct Pair pair, float **hardness) {
    float ** weights = (float **)makeArray(MAX_X, MAX_Y, sizeof(float));
    float ** distance = (float **)makeArray(MAX_X, MAX_Y, sizeof(float));

    for (int x = 0; x < MAX_X; x++) {
        for (int y = 0; y < MAX_Y; y++) {
            distance[x][y] = INT32_MAX;
            int value = map[x][y];
            if (value == 0) {
                weights[x][y] = 1.5;
            } else if (value == 1) {
                weights[x][y] = 1;
            } else if (value == 2) {
                weights[x][y] = 1;
            }
        }
    }

    for (int x = 2; x < MAX_X - 2; x++) {
        for (int y = 2; y < MAX_Y - 2; y++) {
            int value = map[x][y];
            if (value == 1) {
                for (int i = -2; i <= 2; i++) {
                    for (int j = -2; j <= 2; j++) {
                        weights[x + i][y + j] = weights[x + i][y + j] + 6;
                    }
                }
            }
        }
    }

    struct PQueueNode * source = calloc(1, sizeof(struct PQueueNode));


    struct Pair sourcePoint, destinationPoint;
    sourcePoint = getRandomCorridorStart(rooms[pair.first]);
    destinationPoint = getRandomCorridorStart(rooms[pair.second]);

    for (int x = 0; x < MAX_X; x++) {
        for (int y = 0; y < MAX_Y; y++) {
            weights[x][y] = weights[x][y] + hardness[x][y];
            weights[x][y] = weights[x][y] + (float)pow(pow(x - sourcePoint.first, 2) + pow(y - sourcePoint.second, 2), 0.3);
        }
    }


    source->data = sourcePoint;
    source->weight = 0;
    distance[sourcePoint.first][sourcePoint.second] = 0;

    addAdjacentPoints(source, source, weights);

    for (int x = 0; x < MAX_X; x++) {
        for (int y = 0; y < MAX_Y; y++) {
            distance[x][y] = distance[x][y] + 0.1;
        }
    }

    while(true) {
        struct PQueueNode * min = removeMin(source);

        if (min->data.first == destinationPoint.first && min->data.second == destinationPoint.second) {
            distance[min->data.first][min->data.second] = min->weight;
            free(min);
            break;
        }

        if (min->weight < distance[min->data.first][min->data.second]) {
            distance[min->data.first][min->data.second] = min->weight;

            if (min->data.first == destinationPoint.first && min->data.second == destinationPoint.second) {
                free(min);
                break;
            }

            addAdjacentPoints(source, min, weights);
        } else {
//            printf("%f %f\n", min->weight, distance[min->data.first][min->data.second]);
        }
        free(min);
    }


    freeTree(source);

    struct Pair current = destinationPoint;
    while(current.first != sourcePoint.first || current.second != sourcePoint.second) {
        map[current.first][current.second] = 2;
        current = getNext(current, distance);
    }
    map[current.first][current.second] = 2;

    free(weights);
    free(distance);
}

void freeTree(struct PQueueNode * root) {
    if (root == NULL) return;

    freeTree((root->left));
    freeTree((root->right));

    free(root);
}

int isValidPair(struct Pair pair) {
    return pair.first >= 0 && pair.second >= 0 && pair.first < MAX_X && pair.second < MAX_Y;
}

struct Pair getNext(struct Pair current, float ** distance) {
    struct Pair left = current;
    struct Pair right = current;
    struct Pair up = current;
    struct Pair down = current;
    left.first -= 1;
    right.first += 1;
    up.second -= 1;
    down.second += 1;

    float dLeft = 10000000, dRight = 1000000, dUp = 10000000, dDown = 1000000;

    if (isValidPair(left))
        dLeft = distance[left.first][left.second];
    if (isValidPair(right))
        dRight = distance[right.first][right.second];
    if (isValidPair(up))
        dUp = distance[up.first][up.second];
    if (isValidPair(down))
        dDown = distance[down.first][down.second];

    if (dLeft < dRight && dLeft < dUp && dLeft < dDown) {
        return left;
    } else if (dRight < dUp && dRight < dDown) {
        return right;
    } else if (dUp < dDown) {
        return up;
    } else {
        return down;
    }
}

void addAdjacentPoints(struct PQueueNode *root, struct PQueueNode *min, float **weights) {
    struct Pair temp;
    temp.first = min->data.first - 1;
    temp.second = min->data.second;
    if (temp.first >= 0) {
//        distance[temp.first][temp.second] = weights[temp.first][temp.second] + min->weight;
        addPair(root, temp, weights[temp.first][temp.second] + min->weight);
    }
    temp.first = min->data.first + 1;
    temp.second = min->data.second;
    if (temp.first < MAX_X) {
//        distance[temp.first][temp.second] = weights[temp.first][temp.second] + min->weight;
        addPair(root, temp, weights[temp.first][temp.second] + min->weight);
    }
    temp.first = min->data.first;
    temp.second = min->data.second - 1;
    if (temp.second >= 0) {
//        distance[temp.first][temp.second] = weights[temp.first][temp.second] + min->weight;
        addPair(root, temp, weights[temp.first][temp.second] + min->weight);
    }
    temp.first = min->data.first;
    temp.second = min->data.second + 1;
    if (temp.second < MAX_Y) {
//        distance[temp.first][temp.second] = weights[temp.first][temp.second] + min->weight;
        addPair(root, temp, weights[temp.first][temp.second] + min->weight);
    }
}

struct Pair getRandomCorridorStart(struct Room room) {
    struct Pair point;
    int side = rand() % 4;
    if (side == 0) {
        point.first = room.x - 1;
        point.second = rand() % (room.height - 2) + room.y;
    } else if (side == 1) {
        point.first = room.x + room.width;
        point.second = rand() % (room.height - 2) + room.y;
    } else if (side == 2) {
        point.first = rand() % (room.width - 2) + room.x;
        point.second = room.y - 1;
    } else {
        point.first = rand() % (room.width - 2) + room.x;
        point.second = room.y + room.height;
    }
    return point;
}

void drawCorridors(int **map, struct Room * rooms, struct Pair * pairs) {
    float ** hardness = (float **)makeArray(MAX_X, MAX_Y, sizeof(float));
    for (int x = 0; x < MAX_X; x++) {
        for (int y = 0; y < MAX_Y; y++) {
            hardness[x][y] = rand() % 100 / 15.0;
        }
    }


    for (int i = 0; pairs[i].first != pairs[i].second; i++) {
        drawCorridor(map, rooms, pairs[i], hardness);
    }
    free(hardness);
}

int **makeLevel() {
    int **level = makeArray(MAX_X, MAX_Y, sizeof(int));

    int extraRooms = rand() % 6 + 15;
    int numRooms = (4 + extraRooms);

    struct Room *rooms = malloc(sizeof(struct Room) * numRooms);
    memset(rooms, 0, sizeof(struct Room) * (numRooms));

    rooms[0] = makeRoom(2, 2, 12, MAX_Y - 3);
    rooms[1] = makeRoom(MAX_X - 14, 2, MAX_X - 3, MAX_Y - 3);
    rooms[2] = makeRoom(14, 2, MAX_X - 16, 9);
    rooms[3] = makeRoom(14, MAX_Y - 10, MAX_X - 16, MAX_Y - 3);
    int currentlyMade = 4;
    for (int i = 0; i < extraRooms; i++) {
        struct Room temp = makeRoom(2, 2, MAX_X - 3, MAX_Y - 3);
        int keepRoom = 1;
        for (int j = 0; j < currentlyMade; j++) {
            if (doesOverlap(rooms[j], temp)) {
                keepRoom = 0;
                break;
            }
        }
        if (keepRoom) {
            rooms[i + 4] = temp;
            currentlyMade++;
        } else {
            i--;
        }
    }

    for (int i = 0; i < 4 + extraRooms; i++) {
        for (int x = 0; x < rooms[i].width; x++) {
            for (int y = 0; y < rooms[i].height; y++) {
                level[rooms[i].x + x][rooms[i].y + y] = 1;
            }
        }
    }

    struct Pair * roomPairs = makeRoomPairs(numRooms);

    drawCorridors(level, rooms, roomPairs);

//    for (int i = 0; roomPairs[i].first != roomPairs[i].second; i++) {
//        fprintf(stderr, "%d, %d\n", roomPairs[i].first, roomPairs[i].second);
//    }


    free(rooms);
    free(roomPairs);
    return level;
}

int **makeArray(int rows, int columns, size_t typeSize) {
    size_t length = sizeof(int *) * rows + typeSize * columns * rows;
    char *space = malloc(length);
    memset(space, 0, sizeof(char) * length);

    int **array = (int **)space;

    for (int i = 0; i < rows; i++) {
        array[i] = (int *)&(space[rows * sizeof(int *) + i * columns * typeSize]);
    }

    return array;
}


