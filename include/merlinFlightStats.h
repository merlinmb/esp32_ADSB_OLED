#ifndef FLIGHT_TRACKER_H
#define FLIGHT_TRACKER_H

#include <ArduinoJson.h>
//#include <WiFiClient.h>
#include <HTTPClient.h>
#include <limits>
#include <math.h>


const char* host = "192.168.1.99"; // Flight data source
const char* path = "/data/aircraft.json";
const int port = 8080;

const float myLat = 51.39513478804202;
const float myLon = -1.338836382480781;

struct AircraftDetailsStruct {
    String callsign;
    String type;
    int squawk;
    String route;
    int altitude;
    float distance;
    float speed;
    String status;
    String identifier;
    String flight;
    String description;
    float latitude;
    float longitude;
};

struct FlightStats {
    int totalAircraft=0;
    int emergencyCount=0;
    float avgAltitude=0;
    float avgSpeed;
    AircraftDetailsStruct highestAircraft;
    AircraftDetailsStruct lowestAircraft;
    AircraftDetailsStruct fastestAircraft;
    AircraftDetailsStruct slowestAircraft;
    AircraftDetailsStruct closestAircraft;
    AircraftDetailsStruct farthestAircraft;
    AircraftDetailsStruct emergencyAircraft[10]; // Store up to 10 emergencies
};

FlightStats _flightStats;

#define EARTH_RADIUS_KM 6371.0
#define DEG_TO_RAD 0.017453292519943295

float haversine(float lat1, float lon1, float lat2, float lon2) {
    
    DEBUG_PRINTLN("Calculating haversine");
    /*
    DEBUG_PRINTLN(lat1);
    DEBUG_PRINTLN(lon1);
    DEBUG_PRINTLN(lat2);
    DEBUG_PRINTLN(lon2);
    */
    float dLat = radians(lat2 - lat1);
    float dLon = radians(lon2 - lon1);
    Serial.print("dLat: "); Serial.println(dLat, 6);
    Serial.print("dLon: "); Serial.println(dLon, 6);
    
    float a = sin(dLat / 2) * sin(dLat / 2) +
              cos(radians(lat1)) * cos(radians(lat2)) *
              sin(dLon / 2) * sin(dLon / 2);
    Serial.print("a: "); Serial.println(a, 6);
    
    float c = 2 * atan2(sqrtf(a), sqrtf(1 - a));
    Serial.print("c: "); Serial.println(c, 6);

    return EARTH_RADIUS_KM * c;
}

String getFlightStatus(float verticalRate) {
    if (verticalRate > 500) return "Ascending";
    if (verticalRate < -500) return "Descending";
    return "Cruising";
}

String fetchFlightData(const char* host, const char* path, const int port) {
    WiFiClient client;
    //client.setInsecure(); // Disable certificate validation for testing

    DEBUG_PRINTLN("Connecting to " + String(host) + ":" + String(port));
    if (!client.connect(host, port)) {
        DEBUG_PRINTLN("Connection failed!");
        return "";
    }
    client.setTimeout(1000); // Set a timeout for reading the response
    
    DEBUG_PRINTLN("Sending GET request to " + String(host) + ":" + String(port) + String(path));
    client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    
    DEBUG_PRINTLN("Fetching response");
    String __response = "";

    // Skip headers
    DEBUG_PRINTLN("Skipping Headers");
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {  // Headers end with an empty line
            break;
        }
    }
    int __retryCount = 0;
    DEBUG_PRINTLN("Reading Response in chunks");
    while ((client.connected() || client.available()) && __retryCount < 10) {
        if (client.available()) {
            char c = client.read(); // Read one byte at a time
            __response += c; // Append the byte to the response
            DEBUG_PRINT(c);
        } else {
            delay(1); // Allow time for more data to arrive
            __retryCount++;
        }
    }

    //DEBUG_PRINTLN(__response);

    DEBUG_PRINTLN("Parsing JSON of length: " + String(__response.length()));
    int jsonStart = __response.indexOf('{');
    if (jsonStart != -1) {
        return __response.substring(jsonStart);
    }
    return "";
}

void printAircraft(AircraftDetailsStruct AircraftToPrint)
{
    DEBUG_PRINTLN("-----------------------------------------------------------------");
    DEBUG_PRINTLN("Aircraft:     "+AircraftToPrint.identifier);
    DEBUG_PRINTLN("  Altitude:   "+String(AircraftToPrint.altitude));
    DEBUG_PRINTLN("  callsign:   "+AircraftToPrint.callsign);
    DEBUG_PRINTLN("  description:"+AircraftToPrint.description);
    DEBUG_PRINTLN("  distance:   "+String(AircraftToPrint.distance));
    DEBUG_PRINTLN("  flight:     "+AircraftToPrint.flight);
    DEBUG_PRINTLN("  latitude:   "+String(AircraftToPrint.latitude));
    DEBUG_PRINTLN("  longitude:  "+String(AircraftToPrint.longitude));
    DEBUG_PRINTLN("  route:      "+AircraftToPrint.route);
    DEBUG_PRINTLN("  speed:      "+String(AircraftToPrint.speed));
    DEBUG_PRINTLN("  squawk:     "+String(AircraftToPrint.squawk));
    DEBUG_PRINTLN("  status:     "+AircraftToPrint.status);
    DEBUG_PRINTLN("-----------------------------------------------------------------");
}


bool isSquawkEmergency(int squawkCode) {
    /*
    Squawk codes are assigned by air traffic control and can be changed as needed to manage air traffic. Some common squawk codes and their meanings include:
    Squawk 7000: This is the ‘conspicuity code’ for VFR aircraft that are not assigned a specific code by ATC.     
    Squawk 2000: This is the ‘conspicuity code’ for IFR aircraft that are not assigned a specific code by ATC.     
    Squawk 7700: This is the emergency squawk code, and indicates that the aircraft is in distress and needs priority handling from air traffic control.    
    Squawk 7500: This code indicates that the aircraft is subject to unlawful interference (hijack).    
    Squawk 7600: This code indicates that the aircraft has experienced a radio failure and is unable to transmit or receive messages.    
    Squawk 0030:: This code indicates that the aircraft is lost (UK specific). 
    */
    return  (squawkCode == 0030 ||squawkCode == 7600 || squawkCode == 7500 || _flightStats.emergencyCount == 7700 || _flightStats.emergencyCount == 2000) ;
}

void processFlightData(DynamicJsonDocument &doc)
{

    DEBUG_PRINTLN("processFlightData()\n");
    
    JsonArray aircraft = doc["aircraft"].as<JsonArray>();
    _flightStats.totalAircraft = aircraft.size();
    
    _flightStats.highestAircraft = {};
    _flightStats.highestAircraft.altitude = 0; // Initialize to 0
    _flightStats.lowestAircraft = {};
    _flightStats.lowestAircraft.altitude = std::numeric_limits<int>::max(); // Initialize to max value
    _flightStats.fastestAircraft = {};
    _flightStats.fastestAircraft.speed = 0; // Initialize to 0
    _flightStats.slowestAircraft = {};
    _flightStats.slowestAircraft.speed = std::numeric_limits<float>::max(); // Initialize to max value
    _flightStats.closestAircraft = {};
    _flightStats.closestAircraft.distance = std::numeric_limits<float>::max(); // Initialize to max value
    _flightStats.farthestAircraft = {};
    _flightStats.farthestAircraft.distance = 0; // Initialize to 0
    _flightStats.emergencyCount = 0;

    float totalAltitude = 0;
    float totalSpeed = 0;
    int __currentAircraftIndex = 0;
    for (JsonObject plane : aircraft) {
      
        DEBUG_PRINTLN("processFlightData:populating AircraftDetailsStructs");
        AircraftDetailsStruct __currentAircraft = {
            plane["callsign"] | "Unknown",
            plane["type"] | "Unknown",
            plane["squawk"].as<int>() | 0,
            plane["route"] | "Unknown",
            plane["alt_baro"] | 0,
            plane["distance"],
            plane["tas"], // true air speed (ias=indicated air speed)
            getFlightStatus(plane["baro_rate"] | 0), //status
            plane["flight"] | "Unknown", //identifier
            plane["r"] | "Unknown", //flight
            plane["desc"] | "Unknown", //aircraft type
            plane["lat"].as<float>(),
            plane["lon"].as<float>()
        };

      
        float distance = haversine(myLat, myLon, __currentAircraft.latitude, __currentAircraft.longitude);
        __currentAircraft.distance = distance;
        __currentAircraft.description.trim();
        
        totalAltitude += __currentAircraft.altitude;
        totalSpeed += __currentAircraft.speed;

        printAircraft(__currentAircraft);

        if (__currentAircraft.altitude==0 || __currentAircraft.speed==0 || __currentAircraft.distance==0)
        {
            DEBUG_PRINTLN("Skipping aircraft as is likely on the ground");
            continue;
        }

        DEBUG_PRINTLN("Aircraft details captured " + String(__currentAircraftIndex));

        DEBUG_PRINTLN("Calculating highest, lowest, fastest, slowest, closest, farthest, emergency");
        //highest, lowest, fastest, slowest, closest, farthest, emergency:
        if (__currentAircraft.altitude > _flightStats.highestAircraft.altitude) {
            _flightStats.highestAircraft = __currentAircraft;
        }
        if (__currentAircraft.altitude < _flightStats.lowestAircraft.altitude && __currentAircraft.altitude > 0 && __currentAircraft.speed > 0) {
            _flightStats.lowestAircraft = __currentAircraft;
        }
        if (__currentAircraft.speed > _flightStats.fastestAircraft.speed) {
            _flightStats.fastestAircraft = __currentAircraft;
        }
        if (__currentAircraft.speed < _flightStats.slowestAircraft.speed && __currentAircraft.speed > 0) {
            _flightStats.slowestAircraft = __currentAircraft;
        }
        if (distance > 0 && distance < _flightStats.closestAircraft.distance ) {
            _flightStats.closestAircraft = __currentAircraft;
        }
        if (distance > _flightStats.farthestAircraft.distance) {
            _flightStats.farthestAircraft = __currentAircraft;
        }
        if (isSquawkEmergency(__currentAircraft.squawk))
            _flightStats.emergencyAircraft[_flightStats.emergencyCount++] = __currentAircraft;

        _flightStats.totalAircraft = __currentAircraftIndex;
        __currentAircraftIndex++;
    }
    
    
    _flightStats.avgAltitude = (_flightStats.totalAircraft > 0) ? totalAltitude / _flightStats.totalAircraft : 0;
    _flightStats.avgSpeed = (_flightStats.totalAircraft > 0) ? totalSpeed / _flightStats.totalAircraft : 0;
}


void printFlightStats() {

        
    DEBUG_PRINTLN("Closest Aircraft Details:\n"); 
    printAircraft(_flightStats.closestAircraft);

    
    DEBUG_PRINTLN("Emergency Aircraft Details:  *****\n");
    for (int i = 0; i < _flightStats.emergencyCount; i++) {
        printAircraft(_flightStats.emergencyAircraft[i]);
    }
}

#endif // FLIGHT_TRACKER_H
