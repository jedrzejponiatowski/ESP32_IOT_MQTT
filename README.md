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


*************************************************
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
*************************************************

Główne funkcjonalności projektu: 
* Pomiar temperatury i wilgotności
* Pomiar odległości pomiędzy czujnikiem a przeszkodą
* Pomiar poziomu wody
* Wizualizacja danych na wyświetlaczu LCD
* Możliwość zmiany wyświetlanych pomiarów na aktualną datę i czas
* Synchronizacja czasu rzeczywistego (plan rozszerzony)
* Możliwość włączenia/wyłączenia diody RGB oraz płynna zmiana jej koloru
* Możliwość włączenia/wyłączenia przekaźnika do sterowania oświetleniem
* Wysyłanie i odbieranie danych poprzez protokół MQTT
* Zdalne zarządzanie i obserwacja danych z wykorzystaniem aplikacji
mobilnej.


Środowisko:

Program na mikrokontroler ESP32 Wroom, został napisany w środowisku Visual
Studio Code z wykorzystaniem dostępnego rozszerzenia ESP-IDF firmy Espressif.
Środowisko Espressif ESP-IDF jest oficjalną platformą projektową dla mikrokontrolerów
ESP32. Zawiera kompilator GCC i pozwala na wgrywanie programu na mikrokontroler
przy pomocy przewodu USB jak również na monitorowanie logów.

Wielozadaniowość:

Środowisko Espressif ESP-IDF wspiera również wielowątkowość / wielozadanio-
wość przy użyciu systemu operacyjnego FreeRTOS.
System operacyjny czasu rzeczywistego pozwala na quasi-równoległe wykonywanie
zadań. Bardzo szybkie i częste przełączanie kontekstu prowadzi do uzyskania wrażenia
jednoczesnej realizacji kodu różnych wątków.
Wprowadzenie do projektu RTOS’u narzuca dodanie mechanizmów pozwalających na
synchronizację wątków, która jest niezbędna do prawidłowego działania całego projektu.
Implementacja danych rozwiązań opisana zostanie w dalszej części sprawozdania.

*************************************************
3. Opis wybranych funkcjonalności

3.1 Pomiar temperatury i wilgotności:

Czujnik DHT11 wykorzystuje trzy piny: VDD, GND oraz DATA. Komunikuje się
z mikrokontrolerem przy pomocy interfejsu jednoprzewodowego 1 wire. Inicjatorem
pomiaru jest ESP32, który na krótki okres wystawia na szynie danych stan niski. Po jej
zwolnieniu czujnik DHT11 zaczyna wysyłać określoną sekwencją początkową a następnie
nadaje ramkę danych składającą się wartości temperatury, wilgotności i sumy kontrolnej
o łącznej długości czterdziestu bitów.

3.2 Pomiar odległości:

Ultradźwiękowy czujnik odległości HC-SR04 działa w zakresie od 2cm do 400cm.
Jest zasilany napięciem 5V. Wyjściem jest sygnał, którego czas trwania jest
proporcjonalny do mierzonej odległości. Aby rozpocząć pomiar należy podać na pin TRIG
impuls napięciowy (stan wysoki 5V) przez 10 mikrosekund. Moduł dokonuje pomiaru
odległości przy pomocy fali dźwiękowej o częstotliwości 40 kHz. Do mikrokontrolera
wysyłany jest sygnał na linii ECHO, w którym odległość zależna jest od czasu trwania
stanu wysokiego i można ją obliczyć ze wzoru:
distance = (high level time × velocity of sound (340m/s) ) / 2

3.3 Pomiar poziomu wody:

Czujnik poziomu wody HW-038 jest wyposażony w trzy piny (VDD, GND, SIGNAL).
Jego zasada działania opiera się na odizolowanych liniach przewodnika, które
w warunkach normalnych nie są połączone i nie przewodzą prądu. Jeśli zanurzymy
czujnik w wodzie to zrealizujemy zwarcie tych linii. Sygnał wyjściowy jest proporcjonalny
do przewodzonego prądu. Mikrokontroler wykorzystuje przetwornik analogowo-
cyfrowy do odczytania danych a następnie program przelicza tę wartość na bardziej
przyjazną dla człowieka.

3.4 Zapis i przechowywanie pomiarów:

Każdy z trzech wyżej wymienionych czujników realizuje konkretnie sprecyzowane
czynności. Przed przeprowadzeniem pomiaru należy czujnik odpowiednio zainicjować
i skonfigurować. Potem praca następuje okresowo – trzeba przeprowadzić pomiar
i zapisać dane. Stwierdziłem, że dobrym pomysłem będzie stworzenie interfejsu, którym
mógłbym posługiwać się do obsługi różnych czujników korzystając z tych samych funkcji.
```ruby
typedef struct general_sensor_interface_t
{
     void (*sensor_init)(void);
     void (*sensor_delete)(void);
     void (*sensor_read)(void* out_buff);
} general_sensor_interface_t;
```

Powyższy kod wprowadza poziom wyższej abstrakcji mimo to, że jest pisany
w języku C – uznawanym za stosunkowo „niski”.
Stworzyłem nowy typ danej – general_sensor_interface_t, która przechowuje trzy
wskaźniki na funkcje, pozwalające odpowiednio na – inicjację, usunięcie czujnika oraz na
przeprowadzenie pomiaru.
Podczas działania programu wiele wątków korzysta z danych zwracanych przez
czujniki. Rozwiązanie problemu dostępu do danych rozwiązałem przy pomocy pamięci
współdzielonej:
```ruby
typedef struct m_shd_mem_t
{
     temp_humid_t shd_temp_humid;
     distance_t shd_distance;
     water_level_t shd_water_level;
     led_val_t shd_led_value;
} m_shd_mem_t;

m_shd_mem_t* m_shd_mem_create(void);

void m_shd_mem_write(m_shd_mem_t* shd_mem, measurement_type_t m_type, void* in_buff);

void m_shd_mem_read(const m_shd_mem_t* shd_mem, measurement_type_t m_type, void* out_buff);
```

Struktura ta zawiera dane pomiarowe z czujników oraz wartości odpowiednich
składowych diody RGB. Dodatkowo udostępniony jest interfejs do obsługi pamięci
współdzielonej. Bezpieczny dostęp do danych możliwy jest jedynie przez funkcje
write/read, ponieważ zawierają one mechanizmy synchronizacji wątków – m in. mutex.

Ostatnim bardzo ważnym elementem dotyczącym przeprowadzania pomiarów
jest sposób tworzenia wątków i przekazywania im odpowiednich argumentów. Służy do
tego struktura:
```ruby
typedef struct measurement_args_t
{
     general_sensor_interface_t m_interface;
     m_shd_mem_t* m_shd_memory;
     measurement_type_t m_type;
     void* m_buff;
} measurement_args_t;
```

Pozwala ona na przekazanie zadaniu pomiarowemu odpowiednich narzędzi –
interfejsu do zarządzania czujnikiem, wskaźnika do pamięci współdzielonej, rodzaju
pomiaru jaki dany task będzie wykonywać oraz wskaźnika na bufor do przechowania
danej lokalnej.
Ostatecznie stworzenie zadania do wykonywania pomiaru jest bardzo przyjemne
i nie wymaga zagłębiania się w kod:
```ruby
void sensor_task_distance(void* args)
{
     static distance_t buff;
     measurement_args_t m_args =
     {
          .m_interface =
          {
               .sensor_init = hcsr04_init,
               .sensor_delete = hcsr04_delete,
               .sensor_read = hcsr04_read_by_interface,
          },
          .m_type = DISTANCE,
          .m_shd_memory = (m_shd_mem_t*) args,
          .m_buff = (void*)&buff,
     };
     task_measurement(&m_args);
}
```
Jest to funkcja do realizacji pomiaru odległości. Tworzony jest lokalny bufor do
którego będzie zapisywana zmierzona wartość. Następnie wypełniany jest interfejs oraz
wartości potrzebne do prawidłowej pracy zadania pomiarowego. Na sam koniec
wywoływana jest funkcja działająca nieskończenie długo – dokonuje pomiaru i zapisuje
je korzystając z pamięci współdzielonej.

3.5 Wyświetlanie wyników pomiarów – wyświetlacz LCD:

Reprezentację uzyskanych pomiarów realizuje wyświetlacz LCD 2x16. Wątek
obsługujący wyświetlacz co kilkaset milisekund pobiera dane z pamięci współdzielonej
i aktualizuje wypis. Wartości wyświetlane są w następujących jednostkach
* DIST : cm
* WAT: EMpty / LOw / MEdium / High
* TEMP: stopnie Celsjusza
* HUM: procent wilgotności względnej


<img src="https://user-images.githubusercontent.com/72871744/226616427-060fc1ef-d052-4838-a1cc-b7b4487748fe.jpg" width="500" height="200">

Wyświetlacz LCD realizuje również inne zadanie – wyposażony jest on w przycisk
pozwalający na zmianę aktualnie wyświetlanej karty. Drugim wskazaniem jest data
i godzina, która przeważnie jest zsynchronizowana z czasem lokalnym:

<img src="https://user-images.githubusercontent.com/72871744/226616828-e5dfb776-bccc-4aa4-8edc-34a45958d79a.jpg" width="500" height="200">

3.6 Dioda RGB:

Dioda RGB symuluje możliwość dołączenia do projektu ledówki większej mocy z
technologią regulowania emitowanego koloru. Do uzyskania pełnej funkcjonalności
udostępniony został przycisk pozwalający na włączenie i wyłączenie diody. Natomiast
jeśli przytrzymamy ten przycisk powyżej 1,5 sekundy, dioda wejdzie w tryb strojenia –
jednocześnie trzymając przycisk i regulując nastawy potencjometru jesteśmy w stanie
przejść przez wszystkie fazy główne i mieszane kolorów Red, Green, Blue. Sterowanie
diodą jest również możliwe poprzez aplikacje – zarówno sterowanie zasilaniem jak i
kolorem.
Działanie diody RGB opiera się na dostarczeniu odpowiedniego sygnału PWM. Wartość
odczytana z potencjometru poprzez ADC jest mapowana na odpowiednie wartości trzech
składowych kolorów a następnie zapisywana w pamięci współdzielonej. Na sam koniec
następuje uaktualnienie wypełnienia sygnału PWM.

3.7 Komunikacja z wykorzystaniem protokołu MQTT:

Mikrokontroler ESP32 udostępnia możliwość połączenia z Internetem i wykorzy-
stanie transmisji bezprzewodowej. Do przesyłania danych użyłem lekkiego protokołu
stworzonego dla rozwiązań Internetu Rzeczy.
Cały protokół oparty jest o wzorzec publikacja/subskrypcja. Broker pełni rolę serwera z
którym następnie łączą się klienci. Podłączony klient za pośrednictwem brokera może
publikować informacje. Każda z nich może natomiast składać się z tematu oraz
wiadomości.
W projekcie użyłem otwartego i dostępnego brokera: Mosquitto. Połączenie
odbywa się po URI: mqtt://test.mosquitto.org. Mikrokontroler odpowiednio subskrybuje
lub publikuje dane na wskazanych topicach. Podczas pracy drukowane są logi z odpowie-
dnimi komunikatami na temat otrzymanych danych.

3.8 Aplikacja mobilna:

W projekcie użyta została dostępna aplikacja mobilna IoT OnOff: https://www.iot-onoff.com/.
Pozwala ona na stworzenie widgetów, przycisków i grafów i odpowiednie skonfiguro-
wanie ich do prawidłowej pracy. Należy wprowadzić właściwe URI brokera i tematy
subskrypcji / publikacji. Na sam koniec zostało odpowiednio dobrać wartości do tematu.

<img src="https://user-images.githubusercontent.com/72871744/226617310-edc50494-4979-4bf5-989c-d3f48baff7f7.PNG" width="300" height="500">
