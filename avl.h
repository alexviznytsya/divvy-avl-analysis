/*avl.h*/

//
// AVL Tree ADT header file.
//
// Alex Viznytsya
// Spring 2017
//

// make sure this header file is #include exactly once:
#pragma once

typedef enum boolean {
    false,
    true
} boolean;

//
// AVL type declarations:
//

typedef enum GENDER {
    MALE,
    FEMALE,
    UNKNOWN
} GENDER;

typedef enum USERTYPE {
    SUBSCRIBER,
    CUSTOMER
} USERTYPE;

typedef struct STATION {
    int  StationID;
    int StationDPCapacity;
    double StationLatitude;
    double StationLongitude;
    char *StationOnlineDate;
    char *StationName;
} STATION;

typedef struct TRIP {
    int  TripID;
    char *TripStartTime;
    char *TripStopTime;
    int TripBikeID;
    int TripDuration;
    int TripFromStationID;
    char *TripFromStationName;
    int TripToStationID;
    char *TripToStationName;
    USERTYPE TripUserType;
    GENDER TripUserGenger;
    int TripUserBirthYear;
} TRIP;

typedef struct BIKE {
  int  BikeID;
  int  BikeTripCount;
} BIKE;

typedef enum UNIONTYPE {
    STATIONTYPE,
    TRIPTYPE,
    BIKETYPE
} UNIONTYPE;

typedef int  AVLKey;

typedef struct AVLValue {
  UNIONTYPE Type;
  union {
    STATION  Station;
    TRIP     Trip;
    BIKE     Bike;
  };
} AVLValue;

typedef struct AVLNode {
  AVLKey    Key;
  AVLValue  Value;
  struct AVLNode  *Left;
  struct AVLNode  *Right;
  int       Height;
} AVLNode;

typedef struct AVL {
  AVLNode *Root;
  int      Count;
} AVL;

typedef struct StationsLL {
    union {
        int stationID;
        int stationFromID;
        int stationToID;
    };
    double milage;
    struct StationsLL *next;
} StationsLL;

//
// AVL API: function prototypes
//

AVL *AVLCreate();

void AVLFree(AVL *tree, void(*fp)(AVLKey key, AVLValue value));

int AVLCompareKeys(AVLKey key1, AVLKey key2);
AVLNode *AVLSearch(AVL *tree, AVLKey key);
boolean AVLInsert(AVL *tree, AVLKey key, AVLValue value);

int AVLCount(AVL *tree);
int AVLHeight(AVL *tree);
