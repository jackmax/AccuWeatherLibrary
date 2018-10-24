#include <JsonListener.h>
#include "AccuWeatherLibrary.h"

AccuweatherDailyData dataD[2];
AccuweatherHourlyData dataH;
AccuweatherCurrentData dataC;

Accuweather aw("api key goes here", 0, "en-us", true);

void setup() {
  Serial.begin(115200);
  WiFi.begin("WIFI_SSID", "WIFI_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(100);
  }
  Serial.println(String(ESP.getFreeHeap()));
  int timeStart = millis();
  //int ret = aw.getDaily(dataD,2);
  //int ret = aw.getHourly(&dataH, 1);
  int ret = aw.getCurrent(&dataC);
  
  if (ret != 0){
    Serial.println("ERROR");
    Serial.println(ret);
    return;
  }
  
  while (aw.continueDownload() > 0){
  }

  //print_dataD();
  //print_dataH();
  print_dataC();

  timeStart = millis() - timeStart;

  Serial.printf("Downloaded and parsed in %d ms\n", timeStart);
  Serial.println(String(ESP.getFreeHeap()));
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

void print_dataC(){
  Serial.println(dataC.LocalObservationDateTime);
  Serial.println(dataC.EpochTime);
  Serial.println(dataC.WeatherText);
  Serial.println(dataC.WeatherIcon);
  Serial.println(dataC.IsDayTime);
  Serial.println(dataC.Temperature);
  Serial.println(dataC.RealFeelTemperature);
  Serial.println(dataC.RealFeelTemperatureShade);
  Serial.println(dataC.RelativeHumidity);
  Serial.println(dataC.WindDirection);
  Serial.println(dataC.WindSpeed);
  Serial.println(dataC.WindGustSpeed);
  Serial.println(dataC.UVIndex);
  Serial.println(dataC.UVIndexText);
  Serial.println(dataC.Visibility);
  Serial.println(dataC.CloudCover);
  Serial.println(dataC.Pressure);
}

void print_dataH(){
  Serial.println(dataH.DateTime);
  Serial.println(dataH.EpochDateTime);
  Serial.println(dataH.WeatherIcon);
  Serial.println(dataH.IconPhrase);
  Serial.println(dataH.IsDaylight);
  Serial.println(dataH.Temperature);
  Serial.println(dataH.RealFeelTemperature);
  Serial.println(dataH.WindSpeed);
  Serial.println(dataH.WindDirection);
  Serial.println(dataH.WindGustSpeed);
  Serial.println(dataH.RelativeHumidity);
  Serial.println(dataH.Visibility);
  Serial.println(dataH.UVIndex);
  Serial.println(dataH.UVIndexText);
  Serial.println(dataH.PrecipitationProbability);
  Serial.println(dataH.RainProbability);
  Serial.println(dataH.SnowProbability);
  Serial.println(dataH.IceProbability);
  Serial.println(dataH.TotalLiquid);
  Serial.println(dataH.Rain);
  Serial.println(dataH.Snow);
  Serial.println(dataH.Ice);
  Serial.println(dataH.CloudCover);
}

void print_dataD(){
    Serial.println("====DAY 1====");
  Serial.println(dataD[0].Date);
  Serial.println(dataD[0].TempMin);
  Serial.println(dataD[0].TempMax);
  Serial.println(dataD[0].RealFeelTempMin);
  Serial.println(dataD[0].RealFeelTempMax);
  Serial.println(dataD[0].HoursOfSun);
  Serial.println(dataD[0].EpochDate);
  Serial.println(dataD[0].SunRise);
  Serial.println(dataD[0].SunSet);
  Serial.println("====DAY:====");
  Serial.println(dataD[0].Day.IconPhrase);
  Serial.println(dataD[0].Day.LongPhrase);
  Serial.println(dataD[0].Day.Ice);
  Serial.println(dataD[0].Day.Rain);
  Serial.println(dataD[0].Day.RelativeHumidity);
  Serial.println(dataD[0].Day.Snow);
  Serial.println(dataD[0].Day.TotalLiquid);
  Serial.println(dataD[0].Day.Visibility);
  Serial.println(dataD[0].Day.WindGustSpeed);
  Serial.println(dataD[0].Day.WindSpeed);
  Serial.println(dataD[0].Day.WindDirection);
  Serial.println(dataD[0].Day.CloudCover);
  Serial.println(dataD[0].Day.IceProbability);
  Serial.println(dataD[0].Day.PrecipitationProbability);
  Serial.println(dataD[0].Day.RainProbability);
  Serial.println(dataD[0].Day.SnowProbability);
  Serial.println(dataD[0].Day.ThunderstormProbability);
  Serial.println(dataD[0].Day.WeatherIcon);
  Serial.println("====NIGHT:====");
  Serial.println(dataD[0].Night.IconPhrase);
  Serial.println(dataD[0].Night.LongPhrase);
  Serial.println(dataD[0].Night.Ice);
  Serial.println(dataD[0].Night.Rain);
  Serial.println(dataD[0].Night.RelativeHumidity);
  Serial.println(dataD[0].Night.Snow);
  Serial.println(dataD[0].Night.TotalLiquid);
  Serial.println(dataD[0].Night.Visibility);
  Serial.println(dataD[0].Night.WindGustSpeed);
  Serial.println(dataD[0].Night.WindSpeed);
  Serial.println(dataD[0].Night.WindDirection);
  Serial.println(dataD[0].Night.CloudCover);
  Serial.println(dataD[0].Night.IceProbability);
  Serial.println(dataD[0].Night.PrecipitationProbability);
  Serial.println(dataD[0].Night.RainProbability);
  Serial.println(dataD[0].Night.SnowProbability);
  Serial.println(dataD[0].Night.ThunderstormProbability);
  Serial.println(dataD[0].Night.WeatherIcon);
  Serial.println("====DAY 2====");
  Serial.println(dataD[1].Date);
  Serial.println(dataD[1].TempMin);
  Serial.println(dataD[1].TempMax);
  Serial.println(dataD[1].RealFeelTempMin);
  Serial.println(dataD[1].RealFeelTempMax);
  Serial.println(dataD[1].HoursOfSun);
  Serial.println(dataD[1].EpochDate);
  Serial.println(dataD[1].SunRise);
  Serial.println(dataD[1].SunSet);
  Serial.println("====DAY:====");
  Serial.println(dataD[1].Day.IconPhrase);
  Serial.println(dataD[1].Day.LongPhrase);
  Serial.println(dataD[1].Day.Ice);
  Serial.println(dataD[1].Day.Rain);
  Serial.println(dataD[1].Day.RelativeHumidity);
  Serial.println(dataD[1].Day.Snow);
  Serial.println(dataD[1].Day.TotalLiquid);
  Serial.println(dataD[1].Day.Visibility);
  Serial.println(dataD[1].Day.WindGustSpeed);
  Serial.println(dataD[1].Day.WindSpeed);
  Serial.println(dataD[1].Day.WindDirection);
  Serial.println(dataD[1].Day.CloudCover);
  Serial.println(dataD[1].Day.IceProbability);
  Serial.println(dataD[1].Day.PrecipitationProbability);
  Serial.println(dataD[1].Day.RainProbability);
  Serial.println(dataD[1].Day.SnowProbability);
  Serial.println(dataD[1].Day.ThunderstormProbability);
  Serial.println(dataD[1].Day.WeatherIcon);
  Serial.println("====NIGHT:====");
  Serial.println(dataD[1].Night.IconPhrase);
  Serial.println(dataD[1].Night.LongPhrase);
  Serial.println(dataD[1].Night.Ice);
  Serial.println(dataD[1].Night.Rain);
  Serial.println(dataD[1].Night.RelativeHumidity);
  Serial.println(dataD[1].Night.Snow);
  Serial.println(dataD[1].Night.TotalLiquid);
  Serial.println(dataD[1].Night.Visibility);
  Serial.println(dataD[1].Night.WindGustSpeed);
  Serial.println(dataD[1].Night.WindSpeed);
  Serial.println(dataD[1].Night.WindDirection);
  Serial.println(dataD[1].Night.CloudCover);
  Serial.println(dataD[1].Night.IceProbability);
  Serial.println(dataD[1].Night.PrecipitationProbability);
  Serial.println(dataD[1].Night.RainProbability);
  Serial.println(dataD[1].Night.SnowProbability);
  Serial.println(dataD[1].Night.ThunderstormProbability);
  Serial.println(dataD[1].Night.WeatherIcon);
}