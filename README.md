# MetaMorserino

## Libraries

In diesem Verzeichnis findet man die Unterlagen für den Arduino-basierten CW Keyer "MetaMorserino":
bit.ly/metamorserino

Diese Bibliotheken müssen in der Arduino Entwicklungsumgebung
vorhanden sein, damit fehlerfrei kompiliert werden kann.
    * ADCTouch
    * arduino-volume3
    * ClickButton
    * EEPROMEx
    * NewliquidCrystal

## Software - Versionshistorie:

- Version 1 (nicht veröffentlicht):
  Automatischer CW Keyer mit Touch-Sensoren
- Version 2: zusätzliche Funktion: CW Übungsgenerator m. 5er-Gruppen und Callsigns
- Version 3: zusätzliche Funktion: CW Dekoder (Input entweder Tonsignal, oder Handtaste). Tonsignal-Input
     erfordert Hardware-Erweiterung (NF Verstärker)
- Version 4:
  - zusätzliche Funktion: Möglichkeit, externes mechanisches Paddle anzuschließen anstelle der
                           Touch-Sensoren
  - erweiterte Funktion: CW Dekoder nicht nur für „Straight Key“ oder Tonsignal, sondern auch mit
              Touchsensor (als Cootie / Sideswiper) verwendbar
  - erweiterte Funktion: Dekoder kann zum Tasten eines Senders verwendet werden (außer bei Tonsignal
        als Input)
  - erweiterte Funktion: CW Übungsgenerator kann auch Q-Gruppen und übliche CW-Abkürzungen ausgeben


  - ab V. 4.2 erweiterte Funktionen:
     a) Möglichkeit, auch im Betrieb als Übungsgenerator den Ausgang zu tasten
     b) Option, ACS (Automatic Character Spacing) beim CW Keyer zu verwenden

  - V. 4.3: Bugfix: im CW Trainer Mode wurden bei den 5er Gruppen gewisse Zeichen (zB "9" bei den Ziffern) nicht
    ausgegeben.

  - V. 4.4: Beim Start wird der I2C Bus gescannt, und festgestellt, auf welcher Adresse das Display liegt. Damit können
    Displays mit unterschiedlichen I2C Busadressen ohne Neukompilieren verwendet werden
    (üblich sind welche mit Adresse 0x27 und 0x3f).

  - V. 4.5: Es wurde ein Fehler behoben, der im Keyer-Modus falsches Timing in den Modi Iambic B und Iambic B+ zur
    Folge hatte. Vor allem B+ sollte jetzt deutlich besser funktionieren und jenen CW-isten, die auf
    den Iambic B Modus eingeschossen sind, weniger Probleme bereiten (der Modus B sollte eher nicht
    verwendet werden).

- Version 5.0:
  - zusätzliche Funktion: Copy Game
  - erweiterte Funktion: CW Übungsgenerator kann auch Lektionen mit 5er-Gruppen im Sinne der Koch Methode geben
- Version 5.1:
  - zusätzliche Funktion: Hintergrundbeleuchtung kann ausgeschaltet werden
- Version 6:
  - zusatzliche Funktion: Quick Echo
  - erweiterte Funktion: CW Übungsgenerator kann auch 5er-Gruppen aus interaktiv gewählten Zeichen geben
  - erweiterte Funktion: CW Übungsgenerator kann auch soetwas wie QSO Text geben
  - zusätzliche Funktion: Memo Text. Es kann ein Text im EEPROM gespeichert werden, der im Keyer Modus
    bei Doppelklick des Tasters abgespielt wird.

### Hinweis: beschränkter Programmspeicherplatz

Der Programmspeicherplatz des in den Bausätzen verwendete Arduino Nano
bzw Arduino Uno reicht nicht aus, um sowohl Memo Text als auch QSO
Text Generator gleichzeitig auf den Microcontroller zu flashen. Beide
Funktionen können durch Auskommentieren von Präprozessor #defines
gleich am Beginn des codes aus- bzw. eingebaut werden.

Viele andere Arduino (und ähnliche) Boards haben jedoch genug ROM für beide Funktionen.


## Hardware – Versionshistorie:

- Version 1: erste Auflage des Bausatzes
- Version 1a: zweite Auflage des Bausatzes. Fehlerbehebung auf Platine, anderer Lautsprecher (der alte ist
      nicht mehr lieferbar), anderer Drehgeber.

Um die Funktion des CW Dekoders zu verwenden, muss hardwareseitig ergänzt werden!

- um eine Handtaste verwenden zu können, ist die Handtaste an den Pin D12 des Arduino anzuschließen.
- um Tonsignale zu dekodieren, ist eine Verstärkerschaltung erforderlich, deren Ausgang über
  einen Kondesator an Pin A6 des Arduino anzuschließen - dieser Pin ist außerdem über jeweils
  einen 10k Widerstand an +5V und GND zu legen, so dass im Ruhezustand (kein Eingangssignal vom
  Verstärker) 2,5 V am Pin A6 liegen!
  Ein Schaltungsvorschlag für den Verstärker wird in Kürze ebenfalls im Verzeichnis Hardware
  hinterlegt werden.


### I M P O R T A N T ! !   W I C H T I G ! !  2. Serie Bausätze und Version 4!
English:
  * There are 2 (two) versions of hardware, currently, the only difference affecting the code is the
    rotary encoder
  * older versions: uses the KY-040 encoder (recognazible as it is on a separate PCB
  * newer versions: uses a more "standard" encoder
  * You NEED to #define which encoder is being used - if you fail to do so, the Arduino IDE (compiler) will
    throw an error ar you!
  * For this purpose, eliminate the '//' in front of one of the following lines in the source code! (at the
    type that is being used)

Deutsch:
  * Es gibt derzeit 2 (zwei) unterschiedliche Hardware Versionen, die sich vor allem im verwendeten
    Roray Encoder unterscheiden
  * die ältere version: benutzt einen KY-040 Encoder - man erkennt ihn leicht daran, dass er auf einer
    separaten kleinen Platine sitzt
  * die neuere Version: benutzt einen Encoder, der eher "Standard" ist
  * Du MUSST mit #define festlegen, welcher Encoder benutzt wird, sonst gibt es einen Fehler beim Kompilieren!
  * Dazu musst du die '//' bei einer der folgenden Zeilen im Source Code entfernen! (Bei dem Typ, der
    verwendet wird)

    //#define KY_040_ENCODER
    //#define STANDARD_ENCODER

