# ESP32_IOT_MQTT
Prototyp urządzenia do pomiaru warunków środowiskowych
z dodatkowymi funkcjami zarządzania pomieszczeniem.

1. Wykorzystane zasoby i technologie:
  * Mikrokontroler ESP32
  * System operacyjny czasu rzeczywistego FreeRTOS
  * Komunikacja ESP32 – laptop/telefon z wykorzystaniem protokołu MQTT
  * Czujnik temperatury i wilgotności DHT11
  * Czujnik wody HW-038
  * Czujnik odległości HC-SR04
  * Przetwornik ADC + potencjometr + dioda RGB (zmiana koloru oświetlenia)
  * Wyświetlacz LCD 2x16, wizualizacja pomiarów
  * Przekaźnik do sterowania oświetleniem
  * Protokół synchronizacji czasu sNTP
  
Opis projektu:

 ESP32 pobiera dane z czujników i reprezentuje je na wyświetlaczu LCD 2x16.
Zarządzanie oświetleniem pomieszczenia realizowane jest poprzez przetwornik
ADC z podpiętym potencjometrem który pozwala na płynną zmianę koloru diody RGB.
(Symulacja zmiany koloru oświetlenia) Dodatkowo istnieje komunikacja po MQTT, która
pozwala na użycie przekaźnika i włączenie/wyłączenie światła głównego z poziomu
laptopa/telefonu.
Całość działa z wykorzystaniem systemu FreeRTOS.


2. Realizacja projektu

Projekt został zrealizowany przy pomocy płytki prototypowej, przewodów
połączeniowych, potencjometrów, przycisków, elementów pasywnych oraz zasobów
wymienionych w założeniach projektowych.
Niestety – projekt nie został osadzony w żadnej trwałej obudowie z powodu braku
stałych połączeń pomiędzy elementami oraz planowanej przyszłej rozbiórce projektu

Wygląd fizycznej płytki prototypowej:

<img src="https://user-images.githubusercontent.com/72871744/226610364-0c0c4a7c-efc4-4e88-98e1-d7d630f6fca2.JPG"  width="650" height="500">

Wygląd aplikacji mobilnej:

<img src="https://user-images.githubusercontent.com/72871744/226610481-ea95c67c-3b41-4338-b231-20c3517d9811.PNG" width="300" height="500">


