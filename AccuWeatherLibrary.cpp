#include "AccuWeatherLibrary.h"
#include "JsonListener.h"

#define SIZE_OF_CONST_TABLE(x) sizeof(x)/sizeof(x[0])
#define STACK_HAS_SUFFIX(x) stackSuffix(x, SIZE_OF_CONST_TABLE(x))
/*
 *  Parser constants
 */
//A map to transform strings into tokens
static const std::map<const String, const AccuParserTokens> stringToTokenMap = {
    { "CloudCover", ACCUPARSERCloudCover },
    { "DailyForecasts", ACCUPARSERDailyForecasts },
    { "Date", ACCUPARSERDate },
    { "DateTime", ACCUPARSERDateTime },
    { "Day", ACCUPARSERDay },
    { "Degrees", ACCUPARSERDegrees },
    { "Direction", ACCUPARSERDirection },
    { "English", ACCUPARSEREnglish },
    { "EpochDate", ACCUPARSEREpochDate },
    { "EpochDateTime", ACCUPARSEREpochDateTime },
    { "EpochRise", ACCUPARSEREpochRise },
    { "EpochSet", ACCUPARSEREpochSet },
    { "EpochTime", ACCUPARSEREpochTime },
    { "HoursOfSun", ACCUPARSERHoursOfSun },
    { "Ice", ACCUPARSERIce },
    { "IceProbability", ACCUPARSERIceProbability },
    { "Icon", ACCUPARSERIcon },
    { "IconPhrase", ACCUPARSERIconPhrase },
    { "Imperial", ACCUPARSERImperial },
    { "IsDaylight", ACCUPARSERIsDaylight },
    { "IsDayTime", ACCUPARSERIsDayTime },
    { "LocalObservationDateTime", ACCUPARSERLocalObservationDateTime },
    { "LongPhrase", ACCUPARSERLongPhrase },
    { "Maximum", ACCUPARSERMaximum },
    { "Metric", ACCUPARSERMetric },
    { "Minimum", ACCUPARSERMinimum },
    { "Moon", ACCUPARSERMoon },
    { "Night", ACCUPARSERNight },
    { "Precip1hr", ACCUPARSERPrecip1hr },
    { "PrecipitationProbability", ACCUPARSERPrecipitationProbability },
    { "Pressure", ACCUPARSERPressure },
    { "Rain", ACCUPARSERRain },
    { "RainProbability", ACCUPARSERRainProbability },
    { "RealFeelTemperature", ACCUPARSERRealFeelTemperature },
    { "RealFeelTemperatureShade", ACCUPARSERRealFeelTemperatureShade },
    { "RelativeHumidity", ACCUPARSERRelativeHumidity },
    { "ShortPhrase", ACCUPARSERShortPhrase },
    { "Snow", ACCUPARSERSnow },
    { "SnowProbability", ACCUPARSERSnowProbability },
    { "Speed", ACCUPARSERSpeed },
    { "Sun", ACCUPARSERSun },
    { "Temperature", ACCUPARSERTemperature },
    { "ThunderstormProbability", ACCUPARSERThunderstormProbability },
    { "TotalLiquid", ACCUPARSERTotalLiquid },
    { "Unit", ACCUPARSERUnit },
    { "UVIndex", ACCUPARSERUVIndex },
    { "UVIndexText", ACCUPARSERUVIndexText },
    { "Value", ACCUPARSERValue },
    { "Visibility", ACCUPARSERVisibility },
    { "WeatherIcon", ACCUPARSERWeatherIcon },
    { "WeatherText", ACCUPARSERWeatherText },
    { "Wind", ACCUPARSERWind },
    { "WindGust", ACCUPARSERWindGust },
};

//Arrays of stack suffixes - if the stack has this suffix, this means that this piece of data can be obtained
static const AccuParserTokens LocalObservationDateTime_suffix[] = {ACCUPARSERLocalObservationDateTime};
static const AccuParserTokens DateTime_suffix[] = {ACCUPARSERDateTime};
static const AccuParserTokens EpochTime_suffix[] = {ACCUPARSEREpochTime};
static const AccuParserTokens EpochDateTime_suffix[] = {ACCUPARSEREpochDateTime};
static const AccuParserTokens Date_suffix[] = {ACCUPARSERDate};
static const AccuParserTokens EpochDate_suffix[] = {ACCUPARSEREpochDate};
static const AccuParserTokens HoursOfSun_suffix[] = {ACCUPARSERHoursOfSun};
static const AccuParserTokens WeatherText_suffix[] = {ACCUPARSERWeatherText};
static const AccuParserTokens WeatherIcon_suffix[] = {ACCUPARSERWeatherIcon};
static const AccuParserTokens Icon_suffix[] = {ACCUPARSERIcon};
static const AccuParserTokens IconPhrase_suffix[] = {ACCUPARSERIconPhrase};
static const AccuParserTokens LongPhrase_suffix[] = {ACCUPARSERLongPhrase};
static const AccuParserTokens IsDayTime_suffix[] = {ACCUPARSEREpochTime};
static const AccuParserTokens IsDaylight_suffix[] = {ACCUPARSERIsDaylight};
static const AccuParserTokens RelativeHumidity_suffix[] = {ACCUPARSERRelativeHumidity};
static const AccuParserTokens WindDirection_suffix[] = {ACCUPARSERWind, ACCUPARSERObject, ACCUPARSERDirection, ACCUPARSERObject, ACCUPARSERDegrees};
static const AccuParserTokens UVIndex_suffix[] = {ACCUPARSERUVIndex};
static const AccuParserTokens UVIndexText_suffix[] = {ACCUPARSERUVIndexText};
static const AccuParserTokens CloudCover_suffix[] = {ACCUPARSERCloudCover};

static const AccuParserTokens UnitTemperature_suffix[] = {ACCUPARSERTemperature, ACCUPARSERObject, ACCUPARSERUnitPlaceholder, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens RealFeelUnitTemperature_suffix[] = {ACCUPARSERRealFeelTemperature, ACCUPARSERObject, ACCUPARSERUnitPlaceholder, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens RealFeelUnitTemperatureShade_suffix[] = {ACCUPARSERRealFeelTemperatureShade, ACCUPARSERObject, ACCUPARSERUnitPlaceholder, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens WindUnitSpeed_suffix[] = {ACCUPARSERWind, ACCUPARSERObject, ACCUPARSERSpeed, ACCUPARSERObject, ACCUPARSERUnitPlaceholder, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens WindGustUnitSpeed_suffix[] = {ACCUPARSERWindGust, ACCUPARSERObject, ACCUPARSERSpeed, ACCUPARSERObject, ACCUPARSERUnitPlaceholder, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens UnitPressure_suffix[] = {ACCUPARSERPressure, ACCUPARSERObject, ACCUPARSERUnitPlaceholder, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens UnitVisibility_suffix[] = {ACCUPARSERVisibility, ACCUPARSERObject, ACCUPARSERUnitPlaceholder, ACCUPARSERObject, ACCUPARSERValue};

static const AccuParserTokens Temperature_suffix[] = {ACCUPARSERTemperature, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens RealFeelTemperature_suffix[] = {ACCUPARSERRealFeelTemperature, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens RealFeelTemperatureShade_suffix[] = {ACCUPARSERRealFeelTemperatureShade, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens WindSpeed_suffix[] = {ACCUPARSERWind, ACCUPARSERObject, ACCUPARSERSpeed, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens WindGustSpeed_suffix[] = {ACCUPARSERWindGust, ACCUPARSERObject, ACCUPARSERSpeed, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens Pressure_suffix[] = {ACCUPARSERPressure, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens Visibility_suffix[] = {ACCUPARSERVisibility, ACCUPARSERObject, ACCUPARSERValue};

static const AccuParserTokens TempMin_suffix[] = {ACCUPARSERTemperature, ACCUPARSERObject, ACCUPARSERMinimum, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens RealFeelTempMin_suffix[] = {ACCUPARSERRealFeelTemperature, ACCUPARSERObject, ACCUPARSERMinimum, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens TempMax_suffix[] = {ACCUPARSERTemperature, ACCUPARSERObject, ACCUPARSERMaximum, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens RealFeelTempMax_suffix[] = {ACCUPARSERRealFeelTemperature, ACCUPARSERObject, ACCUPARSERMaximum, ACCUPARSERObject, ACCUPARSERValue};

static const AccuParserTokens SunRise_suffix[] = {ACCUPARSERSun, ACCUPARSERObject, ACCUPARSEREpochRise};
static const AccuParserTokens SunSet_suffix[] = {ACCUPARSERSun, ACCUPARSERObject, ACCUPARSEREpochSet};

static const AccuParserTokens PrecipitationProbability_suffix[] = {ACCUPARSERPrecipitationProbability};
static const AccuParserTokens ThunderstormProbability_suffix[] = {ACCUPARSERThunderstormProbability};
static const AccuParserTokens RainProbability_suffix[] = {ACCUPARSERRainProbability};
static const AccuParserTokens SnowProbability_suffix[] = {ACCUPARSERSnowProbability};
static const AccuParserTokens IceProbability_suffix[] = {ACCUPARSERIceProbability};
static const AccuParserTokens TotalLiquid_suffix[] = {ACCUPARSERTotalLiquid, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens Rain_suffix[] = {ACCUPARSERRain, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens Snow_suffix[] = {ACCUPARSERSnow, ACCUPARSERObject, ACCUPARSERValue};
static const AccuParserTokens Ice_suffix[] = {ACCUPARSERIce, ACCUPARSERObject, ACCUPARSERValue};

static const AccuParserTokens baseList_suffix[] = {ACCUPARSERBase, ACCUPARSERList, ACCUPARSERObject};
static const AccuParserTokens dailyForecastsList_suffix[] = {ACCUPARSERBase, ACCUPARSERObject, ACCUPARSERDailyForecasts, ACCUPARSERList, ACCUPARSERObject};

/*
 *  Parser base class
 */
AccuweatherParser::AccuweatherParser(int maxListLength_, bool isMetric_){
    maxListLength = maxListLength_;
    isMetric = isMetric_;
}

void AccuweatherParser::startDocument() {
    tokenStack.push_back(ACCUPARSERBase);
}

void AccuweatherParser::endDocument() {
  if (tokenStack.back() != ACCUPARSERBase){
        Serial.println(F("ERROR: Document token not found"));
    }
    popAllKeys();
}

void AccuweatherParser::key(String key) {
    if (baseListIdx >= maxListLength){
        return;
    }
    auto it = stringToTokenMap.find(key);
    if (it != stringToTokenMap.end()){
        tokenStack.push_back(it->second);
    }
    else {
        tokenStack.push_back(ACCUPARSERUnknown);
    }
}

void AccuweatherParser::value(String value) {
    if (baseListIdx >= maxListLength){
        return;
    }
    Serial.println("value: " + value);
    popAllKeys();
}

void AccuweatherParser::startArray() {
    tokenStack.push_back(ACCUPARSERList);
}

void AccuweatherParser::endArray() {
    if (tokenStack.back() != ACCUPARSERList){
        Serial.println(F("ERROR: List token not found"));
    }
    tokenStack.pop_back();

    popAllKeys();
}

void AccuweatherParser::startObject() {
    tokenStack.push_back(ACCUPARSERObject);
    if (STACK_HAS_SUFFIX(baseList_suffix)){
        baseListIdx++;
    }
}

void AccuweatherParser::endObject() {
    if (tokenStack.back() != ACCUPARSERObject){
        Serial.println(F("ERROR: Object token not found"));
    }
    tokenStack.pop_back();

    popAllKeys();
}

void AccuweatherParser::whitespace(char c) {
    //ignore
}

//Some housekeeping functions
void AccuweatherParser::DEBUG_printStack(){
    for (auto it = tokenStack.begin() ; it != tokenStack.end(); ++it){
        Serial.print(">");
        Serial.print(*it);
    }
    Serial.println("*");
}

bool AccuweatherParser::stackSuffix(const AccuParserTokens suffix[], int suffix_len){
    int stackSize = tokenStack.size();
    if (stackSize < suffix_len){
        return false;
    }
    if (suffix_len == 0){
        return true;
    }
    do {
        stackSize--;
        suffix_len--;
        if (tokenStack[stackSize] != suffix[suffix_len]){
            if (suffix[suffix_len] == ACCUPARSERUnitPlaceholder){
                if (isMetric){
                    if (tokenStack[stackSize] != ACCUPARSERMetric){
                        return false;
                    }
                }
                else {
                    if (tokenStack[stackSize] != ACCUPARSERImperial){
                        return false;
                    }
                }
            }
            else {
                return false;
            }
        }
    }
    while (stackSize > 0 && suffix_len > 0);
    return true;
}

bool AccuweatherParser::stackContains(const AccuParserTokens token){
    for (auto it = tokenStack.rbegin(); it != tokenStack.rend(); it++)
        if (*it == token)
            return true;
    return false;
}

void AccuweatherParser::popAllKeys(){
    while (!(tokenStack.back() == ACCUPARSERObject || tokenStack.back() == ACCUPARSERList || tokenStack.back() == ACCUPARSERBase)){
        tokenStack.pop_back();
    }
}

/*
 *  Parsers for specific endpoints
 */
AccuweatherCurrentParser::AccuweatherCurrentParser(AccuweatherCurrentData* data_ptr_, bool isMetric_) : AccuweatherParser(1, isMetric_){
    data_ptr = data_ptr_;
}

void AccuweatherCurrentParser::value(String value){
    if (baseListIdx >= maxListLength){
        return;
    }

    if (STACK_HAS_SUFFIX(LocalObservationDateTime_suffix)) {
        data_ptr->LocalObservationDateTime = value;
    }
    else if (STACK_HAS_SUFFIX(EpochTime_suffix)){
        data_ptr->EpochTime = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(WeatherText_suffix)){
        data_ptr->WeatherText = value;
    }
    else if (STACK_HAS_SUFFIX(WeatherIcon_suffix)){
        data_ptr->WeatherIcon = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(IsDayTime_suffix)){
        data_ptr->IsDayTime = value == "true";
    }
    else if (STACK_HAS_SUFFIX(UnitTemperature_suffix)){
        data_ptr->Temperature = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(RealFeelUnitTemperature_suffix)){
        data_ptr->RealFeelTemperature = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(RealFeelUnitTemperatureShade_suffix)){
        data_ptr->RealFeelTemperatureShade = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(RelativeHumidity_suffix)){
        data_ptr->RelativeHumidity = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(WindDirection_suffix)){
        data_ptr->WindDirection = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(WindUnitSpeed_suffix)){
        data_ptr->WindSpeed = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(WindGustUnitSpeed_suffix)){
        data_ptr->WindGustSpeed = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(UVIndex_suffix)){
        data_ptr->UVIndex = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(UVIndexText_suffix)){
        data_ptr->UVIndexText = value;
    }
    else if (STACK_HAS_SUFFIX(UnitVisibility_suffix)){
        data_ptr->Visibility = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(CloudCover_suffix)){
        data_ptr->CloudCover = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(UnitPressure_suffix)){
        data_ptr->Pressure = value.toFloat();
    }
    
    popAllKeys();
}

AccuweatherHourlyParser::AccuweatherHourlyParser(AccuweatherHourlyData* data_ptr_, int maxListLength_) : AccuweatherParser(maxListLength_){
    data_ptr = data_ptr_;
}

void AccuweatherHourlyParser::value(String value){
    if (baseListIdx >= maxListLength){
        return;
    }

    if (STACK_HAS_SUFFIX(DateTime_suffix)) {
        data_ptr[baseListIdx].DateTime = value;
    }
    else if (STACK_HAS_SUFFIX(EpochDateTime_suffix)) {
        data_ptr[baseListIdx].EpochDateTime = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(WeatherIcon_suffix)) {
        data_ptr[baseListIdx].WeatherIcon = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(IconPhrase_suffix)) {
        data_ptr[baseListIdx].IconPhrase = value;
    }
    else if (STACK_HAS_SUFFIX(IsDaylight_suffix)) {
        data_ptr[baseListIdx].IsDaylight = value == "true";
    }
    else if (STACK_HAS_SUFFIX(Temperature_suffix)) {
        data_ptr[baseListIdx].Temperature = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(RealFeelTemperature_suffix)) {
        data_ptr[baseListIdx].RealFeelTemperature = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(WindSpeed_suffix)) {
        data_ptr[baseListIdx].WindSpeed = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(WindDirection_suffix)) {
        data_ptr[baseListIdx].WindDirection = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(WindGustSpeed_suffix)) {
        data_ptr[baseListIdx].WindGustSpeed = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(RelativeHumidity_suffix)) {
        data_ptr[baseListIdx].RelativeHumidity = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(Visibility_suffix)) {
        data_ptr[baseListIdx].Visibility = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(UVIndex_suffix)) {
        data_ptr[baseListIdx].UVIndex = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(UVIndexText_suffix)) {
        data_ptr[baseListIdx].UVIndexText = value;
    }
    else if (STACK_HAS_SUFFIX(PrecipitationProbability_suffix)) {
        data_ptr[baseListIdx].PrecipitationProbability = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(RainProbability_suffix)) {
        data_ptr[baseListIdx].RainProbability = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(SnowProbability_suffix)) {
        data_ptr[baseListIdx].SnowProbability = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(IceProbability_suffix)) {
        data_ptr[baseListIdx].IceProbability = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(TotalLiquid_suffix)) {
        data_ptr[baseListIdx].TotalLiquid = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(Rain_suffix)) {
        data_ptr[baseListIdx].Rain = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(Snow_suffix)) {
        data_ptr[baseListIdx].Snow = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(Ice_suffix)) {
        data_ptr[baseListIdx].Ice = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(CloudCover_suffix)) {
        data_ptr[baseListIdx].CloudCover = value.toInt();
    }

    popAllKeys();
}

AccuweatherDailyParser::AccuweatherDailyParser(AccuweatherDailyData* data_ptr_, int maxListLength_) : AccuweatherParser(maxListLength_){
    data_ptr = data_ptr_;
}

void AccuweatherDailyParser::startObject() {
    tokenStack.push_back(ACCUPARSERObject);
    if (STACK_HAS_SUFFIX(dailyForecastsList_suffix)){
        baseListIdx++;
    }
}

void AccuweatherDailyParser::value(String value){
    if (baseListIdx >= maxListLength){
        return;
    }

    if (STACK_HAS_SUFFIX(Date_suffix)) {
        data_ptr[baseListIdx].Date = value;
    }
    else if (STACK_HAS_SUFFIX(TempMin_suffix)) {
        data_ptr[baseListIdx].TempMin = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(TempMax_suffix)) {
        data_ptr[baseListIdx].TempMax = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(RealFeelTempMin_suffix)) {
        data_ptr[baseListIdx].RealFeelTempMin = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(RealFeelTempMax_suffix)) {
        data_ptr[baseListIdx].RealFeelTempMax = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(HoursOfSun_suffix)) {
        data_ptr[baseListIdx].HoursOfSun = value.toFloat();
    }
    else if (STACK_HAS_SUFFIX(EpochDate_suffix)) {
        data_ptr[baseListIdx].EpochDate = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(SunRise_suffix)) {
        data_ptr[baseListIdx].SunRise = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(SunSet_suffix)) {
        data_ptr[baseListIdx].SunSet = value.toInt();
    }
    else if (STACK_HAS_SUFFIX(IconPhrase_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.IconPhrase = value;
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.IconPhrase = value;

    }
    else if (STACK_HAS_SUFFIX(LongPhrase_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.LongPhrase = value;
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.LongPhrase = value;

    }
    else if (STACK_HAS_SUFFIX(Ice_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.Ice = value.toFloat();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.Ice = value.toFloat();

    }
    else if (STACK_HAS_SUFFIX(Rain_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.Rain = value.toFloat();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.Rain = value.toFloat();

    }
    else if (STACK_HAS_SUFFIX(RelativeHumidity_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.RelativeHumidity = value.toFloat();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.RelativeHumidity = value.toFloat();

    }
    else if (STACK_HAS_SUFFIX(Snow_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.Snow = value.toFloat();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.Snow = value.toFloat();

    }
    else if (STACK_HAS_SUFFIX(TotalLiquid_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.TotalLiquid = value.toFloat();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.TotalLiquid = value.toFloat();

    }
    else if (STACK_HAS_SUFFIX(Visibility_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.Visibility = value.toFloat();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.Visibility = value.toFloat();

    }
    else if (STACK_HAS_SUFFIX(WindGustSpeed_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.WindGustSpeed = value.toFloat();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.WindGustSpeed = value.toFloat();

    }
    else if (STACK_HAS_SUFFIX(WindSpeed_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.WindSpeed = value.toFloat();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.WindSpeed = value.toFloat();

    }
    else if (STACK_HAS_SUFFIX(WindDirection_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.WindDirection = value.toInt();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.WindDirection = value.toInt();

    }
    else if (STACK_HAS_SUFFIX(CloudCover_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.CloudCover = value.toInt();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.CloudCover = value.toInt();

    }
    else if (STACK_HAS_SUFFIX(IceProbability_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.IceProbability = value.toInt();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.IceProbability = value.toInt();

    }
    else if (STACK_HAS_SUFFIX(PrecipitationProbability_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.PrecipitationProbability = value.toInt();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.PrecipitationProbability = value.toInt();

    }
    else if (STACK_HAS_SUFFIX(RainProbability_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.RainProbability = value.toInt();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.RainProbability = value.toInt();

    }
    else if (STACK_HAS_SUFFIX(SnowProbability_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.SnowProbability = value.toInt();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.SnowProbability = value.toInt();

    }
    else if (STACK_HAS_SUFFIX(ThunderstormProbability_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.ThunderstormProbability = value.toInt();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.ThunderstormProbability = value.toInt();

    }
    else if (STACK_HAS_SUFFIX(Icon_suffix)) {
        if (stackContains(ACCUPARSERDay))
            data_ptr[baseListIdx].Day.WeatherIcon = value.toInt();
        else if (stackContains(ACCUPARSERNight))
            data_ptr[baseListIdx].Night.WeatherIcon = value.toInt();

    }

    popAllKeys();
}

/*
 *  Data retrieval over HTTP
 */
static const char currentURLTemplate[] PROGMEM = "http://dataservice.accuweather.com/currentconditions/v1/%d?apikey=%s&details=true&language=%s";
int Accuweather::getCurrent(AccuweatherCurrentData* data_ptr) {
    char url[256];
    snprintf_P(url, 256, currentURLTemplate, locationID, apiKey, languageID);
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
            length = http.getSize();
            parser.reset();
            listener = new AccuweatherCurrentParser(data_ptr, isMetric);
            parser.setListener(listener);
            return 0;
        }
    }
    //If we got here, we failed to get data
    http.end();
    return httpCode;
}

static const char hourlyURLTemplate[] PROGMEM = "http://dataservice.accuweather.com/forecasts/v1/hourly/%dhour/%d?apikey=%s&details=true&metric=%s&language=%s";
int Accuweather::getHourly(AccuweatherHourlyData* data_ptr, int hours){
    static const int possibleHours[] = {120, 72, 24, 12, 1};
    int hoursToGet = possibleHours[0];
    for (int i = 0; i < SIZE_OF_CONST_TABLE(possibleHours); i++){
        if (hours <= possibleHours[i]){
            hoursToGet = possibleHours[i];
        }
        else {
            break;
        }
    }

    char url[256];
    snprintf_P(url, 256, hourlyURLTemplate, hoursToGet, locationID, apiKey, (isMetric ? "true" : "false"), languageID);
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
            length = http.getSize();
            parser.reset();
            listener = new AccuweatherHourlyParser(data_ptr, hours);
            parser.setListener(listener);
            return 0;
        }
    }
    //If we got here, we failed to get data
    http.end();
    return httpCode;
}

static const char dailyURLTemplate[] PROGMEM = "http://dataservice.accuweather.com/forecasts/v1/daily/%dday/%d?apikey=%s&details=true&metric=%s&language=%s";
int Accuweather::getDaily(AccuweatherDailyData* data_ptr, int days){
    static const int possibleDays[] = {15, 10, 5, 1};
    int daysToGet = possibleDays[0];
    for (int i = 0; i < SIZE_OF_CONST_TABLE(possibleDays); i++){
        if (days <= possibleDays[i]){
            daysToGet = possibleDays[i];
        }
        else {
            break;
        }
    }

    char url[256];
    snprintf_P(url, 256, dailyURLTemplate, daysToGet, locationID, apiKey, (isMetric ? "true" : "false"), languageID);
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
            length = http.getSize();
            parser.reset();
            listener = new AccuweatherDailyParser(data_ptr, days);
            parser.setListener(listener);
            return 0;
        }
    }
    //If we got here, we failed to get data
    http.end();
    return httpCode;
}

void Accuweather::freeConnection(){
    http.end();
    delete listener;
}

/*
 *  continueDownload() function downloads data in 128 byte chunks.
 *  Returns <0 on download error, 0 on download finished, >0 if the download can be continued.
 *  This function should be called repeatedly until the returned value is 0.
 *  The function will close the connection and free resources automatically.
 */
int Accuweather::continueDownload(){
    // read all data from server
    if (http.connected() && (length > 0 || length == -1)){
        // create buffer for read
        uint8_t buff[128] = { 0 };

        // get tcp stream
        WiFiClient * stream = http.getStreamPtr();

        // get available data size
        size_t size = stream->available();
        int c = size;
        if (size) {
            // read up to 128 byte
            c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            
            for (int i = 0; i < c; i++){
                parser.parse(buff[i]);
            }
            if (length > 0) {
                length -= c;
            }
        }
        return c+1;
    }
    else if (length == 0){
        freeConnection();
        return 0;
    }
    else {
        return -1;
    }
}
