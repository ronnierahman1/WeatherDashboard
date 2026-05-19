#include <WiFi.h>
#include "config.h"
#include "globals.h"
#include "driver.h"
#include "weather_data.h"
#include "weather_api.h"
#include "weather_binding.h"
#include "ui_weather.h"

EPaper epaper;

static bool wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  const uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) delay(200);
  return WiFi.status() == WL_CONNECTED;
}

static void wifi_turnoff(){
  if(WiFi.status() == WL_CONNECTED){
    WiFi.disconnect();
  }
  WiFi.mode(WIFI_OFF);
}


WeatherState S{};
void setup() {
  epaper.begin();
  epaper.setRotation(4);
  epaper.fillScreen(TFT_WHITE);
  epaper.update();

  // Optional splash
  epaper.setTextSize(2);
  epaper.drawString("Connecting WiFi...", 20, 40);
  epaper.update();

  if (!wifiConnect()) {
    // fallback: draw last known / placeholder
    S.location = "Offline";
    S.temp=0; S.feelsLike=0; S.humidity=0; S.condition="—";
    renderWeatherDashboard(epaper, S);
    epaper.update();
    return;
  }

  
  // Pull current/hourly and daily
  epaper.setTextSize(1);
  if (fetchCurrentAndHourly(S) && fetchDaily(S)) {           // functions you already have
    Serial.printf("UV[0]=%d\n", S.dailyUVMax[0]);

    bindWeatherForDashboard(S);                              // map raw → UI fields
    deepClean(epaper);
    renderWeatherDashboard(epaper, S);                       // your UI (unchanged)
    // renderWeatherDataTable(epaper, S, 0, 0, 800);
    epaper.update();                                         // refresh panel
    wifi_turnoff();
  } else {
    // simple error UI
    epaper.fillScreen(TFT_WHITE);
    epaper.setTextSize(2);
    epaper.drawString("Weather fetch failed", 20, 40);
    epaper.update();
  }
}

void loop() {
  sleep(15 * 60  ); // e.g., every 15 minutes
  deepClean(epaper);
  // epaper.setTextSize(2);
  // epaper.drawString("Turning off Wifi...", 20, 40);
  // epaper.update();
  // wifi_turnoff();

  // epaper.drawString("Wifi turned off...", 20, 80);
  // epaper.update();

  // // You can schedule a refresh window to reduce ghosting / power.
  // sleep(1 * 30); // e.g., every 30 minutes
  // epaper.drawString("Wifi turning on...", 20, 120);
  // epaper.update();
  if (!wifiConnect()) {
    // fallback: draw last known / placeholder
    S.location = "Offline";
    S.temp=0; S.feelsLike=0; S.humidity=0; S.condition="—";
    renderWeatherDashboard(epaper, S);
    epaper.update();
  }
  else {
  // epaper.drawString("Wifi turned on...", 20, 160);
  // epaper.update();
  // sleep(1 * 5); // e.g., every 30 minutes
  // deepClean(epaper);
    if (fetchCurrentAndHourly(S) && fetchDaily(S)) {           // functions you already have
      bindWeatherForDashboard(S);                              // map raw → UI fields
      renderWeatherDashboard(epaper, S);                       // your UI (unchanged)
      epaper.update();                                         // refresh panel
    } else {
      // simple error UI
      epaper.fillScreen(TFT_WHITE);
      epaper.setTextSize(2);
      epaper.drawString("Weather fetch failed", 20, 40);
      epaper.update();
    }
  }
  wifi_turnoff();  
}
void deepClean    (EPaper& epaper) { epaper.fillScreen(TFT_BLACK); epaper.update();
                                     epaper.fillScreen(TFT_WHITE); epaper.update(); }
