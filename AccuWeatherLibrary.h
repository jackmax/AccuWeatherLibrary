#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "JsonStreamingParser.h"
#include "JsonListener.h"
#include <limits.h>
#include <vector>
#include <map>

//Structures for holding the received data
typedef struct {
    String LocalObservationDateTime;
    String UVIndexText;
    String WeatherText;
    float CloudCover;
    float Pressure;
    float RealFeelTemperature;
    float RealFeelTemperatureShade;
    float RelativeHumidity;
    float Temperature;
    float Visibility;
    float WindGustSpeed;
    float WindSpeed;
    int EpochTime;
    int16_t WindDirection;
    uint8_t IsDayTime;
    uint8_t UVIndex;
    uint8_t WeatherIcon;
}
AccuweatherCurrentData;

typedef struct {
    String DateTime;
    String IconPhrase;
    String UVIndexText;
    float Ice;
    float Rain;
    float RealFeelTemperature;
    float RelativeHumidity;
    float Snow;
    float Temperature;
    float TotalLiquid;
    float Visibility;
    float WindGustSpeed;
    float WindSpeed;
    int EpochDateTime;
    int16_t WindDirection;
    uint8_t CloudCover;
    uint8_t IceProbability;
    uint8_t IsDaylight;
    uint8_t PrecipitationProbability;
    uint8_t RainProbability;
    uint8_t SnowProbability;
    uint8_t UVIndex;
    uint8_t WeatherIcon;
}
AccuweatherHourlyData;

typedef struct {
    String IconPhrase;
    String LongPhrase;
    float Ice;
    float Rain;
    float RelativeHumidity;
    float Snow;
    float TotalLiquid;
    float Visibility;
    float WindGustSpeed;
    float WindSpeed;
    int16_t WindDirection;
    uint8_t CloudCover;
    uint8_t IceProbability;
    uint8_t PrecipitationProbability;
    uint8_t RainProbability;
    uint8_t SnowProbability;
    uint8_t ThunderstormProbability;
    uint8_t WeatherIcon;
}
AccuweatherForecastData;

typedef struct {
    String Date;
    float TempMin;
    float TempMax;
    float RealFeelTempMin;
    float RealFeelTempMax;
    float HoursOfSun;
    int EpochDate;
    int SunRise;
    int SunSet;
    AccuweatherForecastData Day;
    AccuweatherForecastData Night;
}
AccuweatherDailyData;

//Tokens for different keys that can be found in the JSON responses
//To extend the functionality, first a new token should be added to the enum below,
//then a mapping from String to that token in the map.
enum AccuParserTokens_ {
    ACCUPARSERUnknown,
    ACCUPARSERBase,
    ACCUPARSERList,
    ACCUPARSERObject,
    ACCUPARSERUnitPlaceholder,
    ACCUPARSERCloudCover,
    ACCUPARSERDateTime,
    ACCUPARSERDay,
    ACCUPARSERDegrees,
    ACCUPARSERDirection,
    ACCUPARSEREnglish,
    ACCUPARSEREpochDateTime,
    ACCUPARSEREpochRise,
    ACCUPARSEREpochSet,
    ACCUPARSEREpochTime,
    ACCUPARSERHoursOfSun,
    ACCUPARSERIce,
    ACCUPARSERIceProbability,
    ACCUPARSERIconPhrase,
    ACCUPARSERImperial,
    ACCUPARSERIsDaylight,
    ACCUPARSERIsDayTime,
    ACCUPARSERLocalObservationDateTime,
    ACCUPARSERLongPhrase,
    ACCUPARSERMaximum,
    ACCUPARSERMetric,
    ACCUPARSERMinimum,
    ACCUPARSERMoon,
    ACCUPARSERNight,
    ACCUPARSERPrecip1hr,
    ACCUPARSERPrecipitationProbability,
    ACCUPARSERPressure,
    ACCUPARSERRain,
    ACCUPARSERRainProbability,
    ACCUPARSERRealFeelTemperature,
    ACCUPARSERRealFeelTemperatureShade,
    ACCUPARSERRelativeHumidity,
    ACCUPARSERShortPhrase,
    ACCUPARSERSnow,
    ACCUPARSERSnowProbability,
    ACCUPARSERSpeed,
    ACCUPARSERSun,
    ACCUPARSERTemperature,
    ACCUPARSERThunderstormProbability,
    ACCUPARSERTotalLiquid,
    ACCUPARSERUnit,
    ACCUPARSERUVIndex,
    ACCUPARSERUVIndexText,
    ACCUPARSERValue,
    ACCUPARSERVisibility,
    ACCUPARSERWeatherIcon,
    ACCUPARSERWeatherText,
    ACCUPARSERWind,
    ACCUPARSERWindGust,
    ACCUPARSERDailyForecasts,
    ACCUPARSERDate,
    ACCUPARSEREpochDate,
    ACCUPARSERIcon,
};

//I do this to save memory - by default enums are ints, so to save a bit of memory I use chars instead.
//WARNING: change this if you will need more than 256 tokens
typedef uint8_t AccuParserTokens;

//The parser creates a stack while parsing the JSON response. This allows a lot of flexibility when creating a parser (e.g. we can infer all previous keys and objects before a given value)
//The stack contains tokens instead of strings. This greatly reduces memory usage and allows us to compare stack contents much quicker.
class AccuweatherParser: public JsonListener {
    public:
        AccuweatherParser(int maxListLength_, bool isMetric_ = true);
        virtual void whitespace(char c);
        virtual void key(String key);
        virtual void value(String value);
        virtual void startDocument();
        virtual void endDocument();
        virtual void startArray();
        virtual void endArray();
        virtual void startObject();
        virtual void endObject();
        bool isMetric = true;

    protected:
        std::vector<AccuParserTokens> tokenStack;
        void DEBUG_printStack();
        bool stackSuffix(const AccuParserTokens suffix[], int suffix_len);
        bool stackContains(const AccuParserTokens token);
        void popAllKeys();
        int baseListIdx = -1;
        int maxListLength = INT_MAX;
        bool listFull = false;
};

//Descendant classes for parsing particular types of responses
class AccuweatherCurrentParser: public AccuweatherParser {
    public:
        AccuweatherCurrentParser(AccuweatherCurrentData* data_ptr_, bool isMetric_);
        virtual void value(String value);
    protected:
        AccuweatherCurrentData* data_ptr;
};

class AccuweatherHourlyParser: public AccuweatherParser {
    public:
        AccuweatherHourlyParser(AccuweatherHourlyData* data_ptr_, int maxListLength_);
        virtual void value(String value);
    protected:
        AccuweatherHourlyData* data_ptr;
};

class AccuweatherDailyParser: public AccuweatherParser {
    public:
        AccuweatherDailyParser(AccuweatherDailyData* data_ptr_, int maxListLength_);
        virtual void startObject();
        virtual void value(String value);
    protected:
        AccuweatherDailyData* data_ptr;
};

//Main class for sending requests and parsing responses
class Accuweather {
public:
    HTTPClient http;
    JsonStreamingParser parser;
    AccuweatherParser* listener = NULL;
    const int locationID;
    const char* languageID;
    const char* apiKey;
    const bool isMetric;
    int length;

    Accuweather(const char* apiKey_, const int locationID_, const char* languageID_, const bool isMetric_):
        locationID(locationID_),
        languageID(languageID_),
        apiKey(apiKey_),
        isMetric(isMetric_){

    }

    int getCurrent(AccuweatherCurrentData* data_ptr);
    int getHourly(AccuweatherHourlyData* data_ptr, int hours);
    int getDaily(AccuweatherDailyData* data_ptr, int days);
    int continueDownload();
    void freeConnection();
};

