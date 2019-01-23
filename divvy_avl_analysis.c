/*main.cpp*/

//
// Divvy Bike Ride Route Analysis, using AVL trees.
//
// Alex Viznytsya
// Spring 2017
//

// ignore stdlib warnings if working in Visual Studio:
#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "avl.h"


// DistBetween2Points:
// Returns the distance in miles between 2 points (lat1, long1) and (lat2, long2).
// Reference: http://www8.nau.edu/cvm/latlon_formula.html
//
double DistBetween2Points(double lat1, double long1, double lat2, double long2) {
    
    double PI = 3.14159265;
    double earth_rad = 3963.1;

    double lat1_rad = lat1 * PI / 180.0;
    double long1_rad = long1 * PI / 180.0;
    double lat2_rad = lat2 * PI / 180.0;
    double long2_rad = long2 * PI / 180.0;

    double dist = earth_rad * acos(
           (cos(lat1_rad)*cos(long1_rad)*cos(lat2_rad)*cos(long2_rad)) +
           (cos(lat1_rad)*sin(long1_rad)*cos(lat2_rad)*sin(long2_rad)) +
           (sin(lat1_rad)*sin(lat2_rad))
           );

  return dist;
}

// FreeAVLNodeData:
// Works with AVLFree() to free the data inside (key, value) pairs.
//
void FreeAVLNodeData(AVLKey key, AVLValue value) {
    
    // What we free depends on what type of value we have:
    if (value.Type == STATIONTYPE) {
        free(value.Station.StationName);
        free(value.Station.StationOnlineDate);
    } else if (value.Type == TRIPTYPE) {
        free(value.Trip.TripStartTime);
        free(value.Trip.TripStopTime);
        free(value.Trip.TripFromStationName);
        free(value.Trip.TripToStationName);
    } else if (value.Type == BIKETYPE) {
        return;
    } else {
        printf("**ERROR: unexpected value type in freeAVLNodeData!\n\n");
        exit(-1);
    }
    
    return;
}

// FreeStationsLL:
//
//
void FreeStationsLL(StationsLL **root) {
    
    StationsLL *cur = *root;
    StationsLL *del = NULL;
    while(cur != NULL) {
        del = cur;
        cur = cur->next;
        free(del);
    }
    
    return;
}

// GetFileName:
// Inputs a filename from the keyboard, make sure the file can be
// opened, and returns the filename if so.  If the file cannot be 
// opened, an error message is output and the program is exited.
//
char *GetFileName() {
    
    char filename[512];
    int  fnsize = sizeof(filename) / sizeof(filename[0]);

    // Input filename from the keyboard:
    fgets(filename, fnsize, stdin);
    filename[strcspn(filename, "\r\n")] = '\0';  // strip EOL char(s):

    // Make sure filename exists and can be opened:
    FILE *infile = fopen(filename, "r");
    if (infile == NULL) {
        printf("**Error: unable to open '%s'\n\n", filename);
        exit(-1);
    }

    fclose(infile);

    // Duplicate and return filename:
    char *s = (char *)malloc((strlen(filename) + 1) * sizeof(char));
    strcpy(s, filename);

    return s;
}

// SkipRestOfInput:
// Inputs and discards the remainder of the current line for the 
// given input stream, including the EOL character(s).
//
void SkipRestOfInput(FILE *stream) {
    
    char restOfLine[256];
    int rolLength = sizeof(restOfLine) / sizeof(restOfLine[0]);

    fgets(restOfLine, rolLength, stream);
    
    return;
}

// PopulateStations:
// Read each record from stationsFileName csv file and build stations
// AVL tree.
//
void PopulateStations(char *stationsFileName, AVL *stations) {
    
    char tempString[512];
    int tempStringLength = sizeof(tempString)/sizeof(tempString[0]);
    char tData[256];
    
    // Open File and read its data:
    FILE *file = fopen(stationsFileName, "r");
    
    // Read and ignore first line of the input file:
    fgets(tempString, tempStringLength, file);
    
    // Read,parse and insert into AVL tree the rest of the input file:
    fgets(tempString, tempStringLength, file);
    
    while (!feof(file)) {
        
        AVLValue stationValue;
        stationValue.Type = STATIONTYPE;
        stationValue.Station.StationID = atoi(strtok(tempString, ","));
        strcpy(tData, strtok(NULL, ","));
        stationValue.Station.StationName = (char *)malloc((strlen(tData) + 1) * sizeof(char));
        strcpy(stationValue.Station.StationName, tData);
        stationValue.Station.StationLatitude = atof(strtok(NULL, ","));
        stationValue.Station.StationLongitude = atof(strtok(NULL, ","));
        stationValue.Station.StationDPCapacity = atoi(strtok(NULL, ","));
        strcpy(tData, strtok(NULL, "\r\n"));
        stationValue.Station.StationOnlineDate = (char *)malloc((strlen(tData) + 1) * sizeof(char));
        strcpy(stationValue.Station.StationOnlineDate, tData);

        AVLInsert(stations, stationValue.Station.StationID, stationValue);
        
        fgets(tempString, tempStringLength, file);
    }
    
    fclose(file);
    free(stationsFileName);
    
    return;
}

// PopulateTripsAnsBikes:
// Read each record from tripsFileName csv file and build trips and bikes
// AVL trees.
//
void PopulateTripsAnsBikes(char *tripsFileName, AVL *trips, AVL *bikes) {
    
    char tempString[512];
    int tempStringLength = sizeof(tempString)/sizeof(tempString[0]);
    char tData[256];
    
    // Open File and read its data:
    FILE *file = fopen(tripsFileName, "r");
    
    // Read and ignore first line of the input file:
    fgets(tempString, tempStringLength, file);
    
    // Read,parse and insert into AVL tree the rest of the input file:
    fgets(tempString, tempStringLength, file);
    while (!feof(file)) {
        
        // Create and instert into AVL tree each trip data:
        AVLValue tripValue;
        tripValue.Type = TRIPTYPE;
        tripValue.Trip.TripID = atoi(strtok(tempString, ","));
        strcpy(tData, strtok(NULL, ","));
        tripValue.Trip.TripStartTime = (char *)malloc((strlen(tData) + 1) * sizeof(char));
        strcpy(tripValue.Trip.TripStartTime, tData);
        strcpy(tData, strtok(NULL, ","));
        tripValue.Trip.TripStopTime = (char *)malloc((strlen(tData) + 1) * sizeof(char));
        strcpy(tripValue.Trip.TripStopTime, tData);
        tripValue.Trip.TripBikeID = atoi(strtok(NULL, ","));
        tripValue.Trip.TripDuration = atoi(strtok(NULL, ","));
        tripValue.Trip.TripFromStationID = atoi(strtok(NULL, ","));
        strcpy(tData, strtok(NULL, ","));
        tripValue.Trip.TripFromStationName = (char *)malloc((strlen(tData) + 1) * sizeof(char));
        strcpy(tripValue.Trip.TripFromStationName, tData);
        tripValue.Trip.TripToStationID = atoi(strtok(NULL, ","));
        strcpy(tData, strtok(NULL, ","));
        tripValue.Trip.TripToStationName = (char *)malloc((strlen(tData) + 1) * sizeof(char));
        strcpy(tripValue.Trip.TripToStationName, tData);
        tripValue.Trip.TripUserType = (strcmp(strtok(NULL, ","), "Subscriber")) ? CUSTOMER : SUBSCRIBER;
        strcpy(tData, strtok(NULL, ","));
        if(tData[0] == '\r' || tData[0] == '\n') {
            tripValue.Trip.TripUserGenger = UNKNOWN;
            tripValue.Trip.TripUserBirthYear = -1;
        } else if(tData[0] == 'M' || tData[0] == 'F'){
            tripValue.Trip.TripUserGenger = (strcmp(tData, "Male")) ? FEMALE : MALE;
            strcpy(tData, strtok(NULL, "\r\n"));
        } else if(tData[0] == '1' || tData[0] == '2') {
            tripValue.Trip.TripUserBirthYear = atoi(tData);
        } else {
            tripValue.Trip.TripUserBirthYear = -1;
        }
        
        AVLInsert(trips, tripValue.Station.StationID, tripValue);
        
        // Create and instert into AVL tree each bike data:
        AVLValue bikeValue;
        bikeValue.Type = BIKETYPE;
        bikeValue.Bike.BikeID = tripValue.Trip.TripBikeID;
        bikeValue.Bike.BikeTripCount = 1;
        
        if(!AVLInsert(bikes, bikeValue.Bike.BikeID, bikeValue)) {
            AVLNode *tBike = AVLSearch(bikes, bikeValue.Bike.BikeID);
            tBike->Value.Bike.BikeTripCount += 1;
        }
        
        fgets(tempString, tempStringLength, file);
    }
    
    fclose(file);
    free(tripsFileName);
    
    return;
}

// PrintStats:
// Print statistics about stations, trips and bikes AVL trees, such as:
// cound of nodes and tree heights.
//
void PrintStats(AVL *stations, AVL *trips, AVL *bikes) {
    
    printf("** Trees:\n");
    printf("   Stations: count = %d, height = %d\n",
           AVLCount(stations), AVLHeight(stations));
    printf("   Trips:    count = %d, height = %d\n",
           AVLCount(trips), AVLHeight(trips));
    printf("   Bikes:    count = %d, height = %d\n",
           AVLCount(bikes), AVLHeight(bikes));
    
    return;
}

// TripsAtStation:
// Returns the number of trips that originated, or ended at requested station ID.
//
int TripsAtStation(AVLNode *root, int stationID) {
    
    if(root == NULL) {
        return 0;
    } else {
        int num = 0;
        if(root->Value.Trip.TripFromStationID == stationID) {
            num += 1;
        }
        if(root->Value.Trip.TripToStationID == stationID) {
            num +=1;
        }
        return num + TripsAtStation(root->Left, stationID) +
                     TripsAtStation(root->Right, stationID);
    }
}

// PrintStationInfo:
// Print requested station information: station ID, station name, station bike
// capacity and trip count that start or eneded at requested station.
//
void PrintStationInfo(AVL *stations, AVL *trips, int stationID) {
    
    AVLNode *stationNode = AVLSearch(stations, stationID);
    if(stationNode != NULL) {
        printf("**Station %d:\n", stationID);
        printf("  Name: '%s'\n", stationNode->Value.Station.StationName);
        printf("  %-11s (%f,%f)\n", "Location:", stationNode->Value.Station.StationLatitude,
                                                 stationNode->Value.Station.StationLongitude);
        printf("  %-11s %d\n", "Capacity:", stationNode->Value.Station.StationDPCapacity);
        printf("  %-11s %d\n", "Trip count:", TripsAtStation(trips->Root, stationID));
    } else {
        printf("**not found\n");
    }
    
    return;
}

// PrintBikeInfo:
// Print number of trips of requested bike ID.
//
void PrintBikeInfo(AVL *bikes, int bikeID) {
    
    AVLNode *bikeNode = AVLSearch(bikes, bikeID);
    if(bikeNode != NULL) {
        printf("**Bike %d:\n", bikeID);
        printf("  Trip count: %d\n", bikeNode->Value.Bike.BikeTripCount);
    } else {
        printf("**not found\n");
    }
    
    return;
}

// PrintTripInfo:
// Print requested trip inforamtion: bike ID, from station ID, to station ID and
// duration of the requested trip.
//
void PrintTripInfo(AVL *trips, int tripID) {
    
    AVLNode *tripNode = AVLSearch(trips, tripID);
    if(tripNode != NULL) {
        printf("**Trip %d:\n", tripID);
        printf("  %-5s %d\n", "Bike:", tripNode->Value.Trip.TripBikeID);
        printf("  %-5s %d\n", "From:", tripNode->Value.Trip.TripFromStationID);
        printf("  %-5s %d\n", "To:", tripNode->Value.Trip.TripToStationID);
        int tripDuratiuonMin = tripNode->Value.Trip.TripDuration / 60;
        int tripDurationSec = tripNode->Value.Trip.TripDuration - (tripDuratiuonMin * 60);
        printf("  Duration: %d min, %d secs\n", tripDuratiuonMin, tripDurationSec);
    } else {
        printf("**not found\n");
    }
    
    return;
}

// FindNearbyStations:
// Find station from requested latitude and lonfiture in radius of requested
// distance:
//
void FindNearbyStations(AVLNode *root, double latitude, double longitude,
                        double distance, StationsLL **nerbyStations) {
    
    // Do nothing:
    if(root == NULL) {
        return;
    }
    
    // Check if root node is in searched distance nad if so, append it in
    // ascending order to the nerbyStations list:
    else {
        double milage = DistBetween2Points(root->Value.Station.StationLatitude,
                                           root->Value.Station.StationLongitude,
                                           latitude, longitude);
        if((milage - distance) < 0.0000001) {
            
            
            // Finf the location, where to append station ID and distance:
            StationsLL *cur = *nerbyStations;
            StationsLL *prev = NULL;
            while(cur != NULL && (cur->milage - milage) < 0.0000001) {
                prev = cur;
                cur = cur->next;
            }
            
            // Creare new nerbyStations node:
            StationsLL *newNode = (StationsLL *)malloc(sizeof(StationsLL));
            newNode->stationID = root->Value.Station.StationID;
            newNode->milage = milage;
            newNode->next = cur;
            
            // Append if only one node:
            if(prev == NULL) {
                *nerbyStations = newNode;
            }
            // Append after the longest distance:
            else {
                
                prev->next =newNode;
            }
        }
        
        // Continue search recursively:
        FindNearbyStations(root->Left, latitude, longitude, distance, nerbyStations);
        FindNearbyStations(root->Right, latitude, longitude, distance, nerbyStations);
    }
    
    return;
}

// PrintNearbyStations:
// Prints the ascending list (from shortest to longest) of nearest stations
// from requested coordinates and maximum distange from these coordinates.
//
void PrintNearbyStations(AVL *stations, double latitude, double longitude, double distance) {
    
    // Find and create the list of nearest stations:
    StationsLL *nerbyStations = NULL;
    FindNearbyStations(stations->Root, latitude, longitude, distance, &nerbyStations);
    
    // Print the list of found stations:
    StationsLL *cur = nerbyStations;
    if(cur != NULL) {
        while(cur != NULL) {
            printf("Station %d: distance %f miles\n", cur->stationID, cur->milage);
            cur= cur->next;
        }
    }
    FreeStationsLL(&nerbyStations);
    return;
}

// FromToTrips:
// Match all trips that starts from requested statin ID:
//
void MatchStarionsFromID(AVLNode *root, StationsLL **routeList, int fromStationID) {
    
    if(root == NULL) {
        return;
    } else {
        if(root->Value.Trip.TripFromStationID == fromStationID) {
            StationsLL *newNode = (StationsLL *)malloc(sizeof(StationsLL));
            newNode->stationToID = root->Value.Trip.TripToStationID;
            newNode->milage = 0.0;
            if(routeList == NULL) {
                newNode->next = NULL;
                *routeList = newNode;
            } else {
                newNode->next = *routeList;
                *routeList = newNode;
            }
        }
        MatchStarionsFromID(root->Left, routeList, fromStationID);
        MatchStarionsFromID(root->Right, routeList, fromStationID);
    }
}

// PrintRouuteAnalysis:
// print an analysis to see how many trips are taken along a given route.
//
void PrintRouteAnalysis(AVL *stations, AVL *trips, int tripID, double distance) {
    
    // Find trip:
    AVLNode *tripNode = AVLSearch(trips, tripID);
    
    if(tripNode != NULL) {
        
        // Find information about trip from station and trip to stations:
        AVLNode *stationA = AVLSearch(stations,
                                      tripNode->Value.Trip.TripFromStationID);
        AVLNode *stationB = AVLSearch(stations,
                                      tripNode->Value.Trip.TripToStationID);
        
        // Find all nearby stations from trip's from station ID:
        StationsLL *nearbyStationsA = NULL;
        FindNearbyStations(stations->Root,
                           stationA->Value.Station.StationLatitude,
                           stationA->Value.Station.StationLongitude,
                           distance, &nearbyStationsA);
        
        // Find all nearby stations from trip's to station ID:
        StationsLL *nearbyStationsB = NULL;
        FindNearbyStations(stations->Root,
                           stationB->Value.Station.StationLatitude,
                           stationB->Value.Station.StationLongitude,
                           distance, &nearbyStationsB);
        
        int tripCount = 0;
        
        // Find all trips matched in "trips" AVL of all trip that start and end
        // with data from stationA and stationB:
        StationsLL *curA = nearbyStationsA;
        StationsLL *curB = nearbyStationsB;
        StationsLL *routeList = NULL;
        while(curA != NULL) {
            MatchStarionsFromID(trips->Root, &routeList, curA->stationID);
            curA = curA->next;
        }
        StationsLL *curC = routeList;
        while(curB != NULL) {
            while(curC != NULL) {
                if(curB->stationFromID == curC->stationFromID) {
                    tripCount++;
                }
                curC = curC->next;
            }
            curB = curB->next;
            curC = routeList;
        }
        
        printf("** Route: from station #%d to station #%d\n",
               stationA->Value.Station.StationID,
               stationB->Value.Station.StationID);
        printf("** Trip count: %d\n", tripCount);
        printf("** Percentage: %f%%\n",
               ((double)tripCount / (double)AVLCount(trips)) * 100);
        
        FreeStationsLL(&routeList);
        FreeStationsLL(&nearbyStationsA);
        FreeStationsLL(&nearbyStationsB);
        
    } else {
        printf("**not found\n");
    }
    
    return;
}


// UserInput:
// All commands that user can use in order to look and search infromation
// about stations, trips and bikes.
//
void UserInput(AVL *stations, AVL *trips, AVL *bikes) {
    
    char  cmd[64];
    printf("** Ready **\n");
    scanf("%s", cmd);
    
    while (strcmp(cmd, "exit") != 0) {
        
        // Output some stats about our data structures:
        if (strcmp(cmd, "stats") == 0) {
            SkipRestOfInput(stdin);
            PrintStats(stations, trips, bikes);
        }
        
        // Output station info:
        else if(strcmp(cmd, "station") == 0) {
            int stationID = -1;
            scanf("%d", &stationID);
            SkipRestOfInput(stdin);
            PrintStationInfo(stations, trips, stationID);
        }
        
        // Output trip info:
        else if(strcmp(cmd, "trip") == 0) {
            int tripID = -1;
            scanf("%d", &tripID);
            SkipRestOfInput(stdin);
            PrintTripInfo(trips, tripID);
        }
        
        // Output bike info:
        else if(strcmp(cmd, "bike") == 0){
            int bikeID = -1;
            scanf("%d", &bikeID);
            SkipRestOfInput(stdin);
            PrintBikeInfo(bikes, bikeID);
        }
        
        // Output nearby stations:
        else if(strcmp(cmd, "find") == 0){
            double latitude = 0.0;
            double longitude = 0.0;
            double distance = 0.0;
            scanf("%lf %lf %lf", &latitude, &longitude, &distance);
            SkipRestOfInput(stdin);
            PrintNearbyStations(stations, latitude, longitude, distance);
        }
        
        // Output analysis of the route:
        else if(strcmp(cmd, "route") == 0){
            int tripID = -1;
            double distance = 0.0;
            scanf("%d %lf", &tripID, &distance);
            PrintRouteAnalysis(stations, trips, tripID, distance);
        }
        
        // If command wasn't found, print error message:
        else {
            printf("**unknown cmd, try again...\n");
        }
        
        scanf("%s", cmd);
    }
    
    return;
}

// main:
//
int main(void) {
    
    printf("** Welcome to Divvy Route Analysis **\n");

    // Get filenames from the user/stdin:
    char *stationsFileName = GetFileName();
    char *tripsFileName = GetFileName();

    // Create AVL trees:
    AVL *stations = AVLCreate();
    AVL *trips = AVLCreate();
    AVL *bikes = AVLCreate();
    
    // Populate AVL trees with data from input files:
    PopulateStations(stationsFileName, stations);
    PopulateTripsAnsBikes(tripsFileName, trips, bikes);
    
    // Interact with user:
    UserInput(stations, trips, bikes);

    // Done, free memory and quit:
    printf("** Freeing memory **\n");
    AVLFree(stations, FreeAVLNodeData);
    AVLFree(trips, FreeAVLNodeData);
    AVLFree(bikes, FreeAVLNodeData);
    
    printf("** Done **\n");
    return 0;
}
