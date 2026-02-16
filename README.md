## Kartenausgeber – STM32/Display‑Firmware

Dieses Repository enthält die STM32CubeIDE‑Firmware für einen automatischen **Kartenausgeber** mit mehreren Motoren, einem **ILI9341‑TFT‑Display** und einem **Drehencoder mit Taster** zur Bedienung.

### Überblick

- **Mikrocontroller**: STM32F3 (Firmwareprojekt `Display_Kartenausgeber`)
- **Anzeige**: ILI9341‑basiertes SPI‑TFT (z.B. 2,4"/2,8")
- **Bedienung**: Drehencoder mit integriertem Taster (`ENC_SW`)
- **Sensorik**: Mehrere Lichtschranken zur kartengesteuerten Phasenlogik
- **Antrieb**: Mehrere DC‑Motoren (L298N‑Treiber) + 1 Schrittmotor (NEMA 17 mit DRV8825)

Die Karten werden in **drei Phasen** verarbeitet:

1. **Phase 1 – Sortieren**: Karten werden am oberen Einzug vereinzelt und ausgerichtet.  
2. **Phase 2 – Transport zur Mitte**: Karten werden von oben in die mittlere Sammelposition transportiert.  
3. **Phase 3 – Auswurf & Verteilung („Snake Mode“)**: Karten werden unten an die Spielerpositionen verteilt.

Die Phasen werden sensorgestützt (Lichtschranken) und über den Systemzustand `SYSTEM_RUN` / `SYSTEM_EMERGENCY_STOP` gesteuert.

---

### Projektstruktur (kurz)

- `Core/Src/main.c`  
  Systemstart, Initialisierung von GPIO, Timern, SPI, Display und Encoder, Hauptzustandsmaschine inkl. Not‑Stopp‑Logik (`Check_Encoder_Button`).

- `Core/Src/menu_logic.c`  
  Display‑Initialisierung (`Menu_Init`), Anzeige‑Hilfsfunktionen und Menü zur Auswahl der Spieleranzahl mit dem Drehencoder (`Menu_Select_Player_Count`, `Menu_Wait_For_Start`, `Menu_Show_Message`).

- `Core/Src/motor_logic.c`  
  Ansteuerung der DC‑Motoren und des Schrittmotors (`M8_Rotate`), zentrale Stopp‑Funktion (`All_Motors_Stop_Immediate`), Phasenlogik `Phase1_Sortieren_1Minute`, `Phase2_Transport_1Minute`, `Phase3_Auswurf`.

- `Core/Src/lichtschranke_logic.c` (falls vorhanden)  
  Auswertung der Lichtschranken, z.B. `Lichtschranke_Is_Card_Detected(n)`.

---

### Voraussetzungen

- **Software**
  - STM32CubeIDE (Version passend zum Projekt, z.B. ≥ 1.15)

- **Hardware**
  - **Mikrocontroller‑Board**: ST **NUCLEO‑F303RE** (U1)  
    - ST‑Microcontroller STM32F303, Arduino‑kompatible Header.
  - **Schrittmotor‑Treiber**: **Pololu Breakout DRV8825** (A1)  
    - Steckmodul `Pololu_Breakout_DRV8825`, Footprint `Module:Pololu_Breakout-16_15.2x20.3mm`, Datasheet: `https://www.pololu.com/product/2982`  
    - Versorgt den bipolaren 4‑Draht‑Schrittmotor M8 über Schraubklemmen J1/J2.
  - **Schrittmotor**: 4‑adriger bipolarer Schrittmotor (M8)  
    - Typisch NEMA‑17, im Schaltplan als `Stepper_Motor_bipolar` geführt.
  - **DC‑Motor‑Anschlüsse**:  
    - Steckleisten `M1/2_Connect1`, `M3/4_Connect2`, `M5/6_Connect1`, `M7/8_Connect1` (jeweils `Conn_01x04`) für die H‑Brücken‑Kanäle der DC‑Motoren M1–M7.  
    - Zugehörige Versorgungs‑/GND‑Stecker `M1/2_Power_GND1`, `M3/4_Power_GND1`, `M5/6_Power_GND1`, `M7/8_Power_GND1` (jeweils `Conn_01x02`) für **+12 V** und Masse.
  - **Versorgung**:  
    - Eingang **+12 V** an den Power‑Klemmen (`M*_Power_GND1`), entkoppelt über einen **polarisierten Elektrolytkondensator C1** (Footprint `CP_Radial_D5.0mm_P2.50mm`).  
    - Das Nucleo‑Board erhält die Versorgung über `VIN_S1` / `+5V` / `+3V3`‑Pins; Logik‑Ebene ist **3,3 V**.
  - **Drehencoder‑Modul**: **KY‑040** oder kompatibel  
    - Im Schaltplan als `KY_040` (Conn_01x05, PinHeader 1x05, 2,54 mm).  
    - Pins: `+3V3`, `GND`, `CLK`, `DT`, `SW` sind mit den NUCLEO‑Pins `PC6`, `PC7`, `PC2` und Pull‑Ups verbunden (siehe Netze `Net-(KY_040-*)`).
  - **Display‑Modul**: **TFT 320x240 (ILI9341‑kompatibel)**  
    - Symbol `TFT_320x240` (U10), Footprint `z_Devices:TFT-320x240`.  
    - SPI‑Signale: `CS` (PA4), `RST` (PA0), `DC` (PA1), `MOSI` (PA7), `SCK` (PA5), `VCC`/`LED` an **+3V3**, `GND` an Masse.
  - **Lichtschranken / Sensorik**: **4× CNY70** Reflex‑Lichttaster (U2, U7, U8, U9)  
    - Footprint `OptoDevice:Vishay_CNY70`, Datasheet: `https://www.vishay.com/docs/83751/cny70.pdf`.  
    - Jeweils mit **10 kΩ Pull‑Up‑Widerständen** (`pullup_R1`–`pullup_R4`) an **+3V3**, die Transistorausgänge sind mit den NUCLEO‑Pins `PA10`, `PC4`, `PA6`, `PC0` verbunden (Lichtschranken 1–4).
    - LED‑Vorwiderstände: `R1`–`R4` mit **1 kΩ** von **+12 V** auf die LED‑Anschlüsse `A` der CNY70‑Sensoren.
  - **Sonstiges**:  
    - Mehrere Stiftleisten (`Connector_PinHeader_2.54mm`) und Schraubklemmen (`TerminalBlock_Phoenix_MKDS-1,5-2`) zur Verbindung von Motoren, Versorgung und Schrittmotor.

---

### Build & Flash

1. Projektordner `Display_Kartenausgeber` in STM32CubeIDE importieren:  
   **File → Open Projects from File System…** und den Ordner wählen.
2. Zielboard in der IDE überprüfen (stimmt mit der Hardware überein?).  
3. Projekt **builden** (Hammer‑Symbol).
4. Board via ST‑LINK verbinden und mit **Debug** oder **Run** aufspielen.

---

### Bedienung mit Drehencoder & Display

#### 1. Einschalten und Initialisierung

Nach dem Einschalten bzw. Reset:

- Alle Motoren werden durch `All_Motors_Stop_Immediate()` mehrmals in einen sicheren Stoppzustand gebracht.  
- Das Display wird initialisiert (`Menu_Init`), Encoder‑Timer `TIM8` gestartet.  
- Es erscheinen nacheinander Statusmeldungen wie:
  - **„STEP 1 – Display Init 1“**
  - **„STEP 2 – Warte auf Start“**

Ab hier erfolgt die Bedienung über den **Drehencoder‑Taster**.

#### 2. Startbildschirm

Die Funktion `Menu_Wait_For_Start()` zeigt z.B.:

- Zeile 1: **„BEREIT?“**  
- Zeile 2: **„Druecke Start“**

**Aktion:**  
- **Kurz den Taster drücken** → Das System verlässt den Wartezustand und geht zur Spielerwahl über.

#### 3. Spieleranzahl mit Drehencoder einstellen

Die Funktion `Menu_Select_Player_Count()` nutzt den Encoder (Drehen + Drücken):

- Display‑Hintergrund: schwarz, Titel **„SPIELERANZAHL:“**.  
- Unten ein Hinweis: **„Drehen & Druecken“**.  
- In der Mitte wird die momentan gewählte Spieleranzahl groß angezeigt (Standard: **4**).

**Bedienung:**

- **Drehen am Encoder:**
  - Die Spieleranzahl wird im Bereich **2…8 Spieler** verändert.
  - Der Wert wird bei jeder Änderung neu angezeigt (große Zahl in der Mitte).
- **Kurz drücken (Taster):**
  - Die aktuell angezeigte Anzahl wird übernommen.
  - Es erscheint kurz eine Bestätigung, z.B. **„OK!“** und anschließend eine Meldung wie **„LOS GEHT’S! – X Spieler“**.

Hinweis: Intern wird der Encoderzähler um `32768` herum zentriert und mit 4 Inkrementen pro Rastung ausgewertet, damit eine stabile Drehbedienung entsteht.

#### 4. Phasenablauf (Sortieren → Transport → Verteilen)

In der **laufenden Hauptlogik** (`while(1)` in `main.c`) wird der Ablauf sensorgesteuert kontrolliert:

1. **Startbedingung**  
   - Das System wartet im Zustand `SYSTEM_EMERGENCY_STOP`, bis per Drehencoder‑Taster ein Start ausgelöst wird (`Check_Encoder_Button`).
   - Beim Start wird `g_RestartFromTop = 1` gesetzt und `g_SystemState = SYSTEM_RUN`.

2. **Phase 1 – Sortieren (oben)**  
   - Anzeige (bei aktivem Display): z.B. **„Phase 1 – Sortieren…“**.  
   - `Phase1_Sortieren_1Minute()` läuft, solange oben Karten an Lichtschranke 1 erkannt werden.  
   - M2/M3 (Wellen) laufen, M1 schiebt die Karten vor und zurück, bis keine Karte oben mehr erkannt wird.

3. **Phase 2 – Transport (Mitte)**  
   - Anzeige: z.B. **„Phase 2 – Transport…“**.  
   - `Phase2_Transport_1Minute()` transportiert Karten aus der oberen Zone in die mittlere Sammelposition (Lichtschranke 3).  
   - M4/M5 bewegen die Karten zur Mitte, M2/M3 drehen in Gegenrichtung zur Sortierphase.

4. **Phase 3 – Auswurf & Verteilung (unten, Snake‑Mode)**  
   - Anzeige: z.B. **„Phase 3 – Verteilen…“**.  
   - `Phase3_Auswurf(spieler_anzahl)` verteilt die Karten an `spieler_anzahl` Positionen:
     - Lichtschranke 4 überwacht, ob noch Karten vorhanden sind.
     - M6 (Feeder) schiebt Karten in Richtung Auswurf.
     - M7 wirft jeweils eine Karte aus.
     - Schrittmotor M8 dreht zwischen den Spielerpositionen (600 Schritte pro Umdrehung, auf Spieleranzahl aufgeteilt).  
     - **Snake‑Logik**: Die Drehrichtung wechselt nach jeder Runde (1→N, dann N→1, usw.).
   - Sind unten keine Karten mehr vorhanden, stoppt `Phase3_Auswurf` alle Motoren und setzt `g_SystemState = SYSTEM_EMERGENCY_STOP`.

5. **Abschlussanzeige**  
   - Bei aktiviertem Display zeigt `Menu_Show_Message` am Ende z.B.:  
     - **„FERTIG“**  
     - **„Druecke Start neu“**
   - Danach kann der nächste Durchlauf wieder per kurzem Tastendruck gestartet werden.

---

### Tasterfunktionen des Drehencoders

Die Funktion `Check_Encoder_Button()` implementiert folgende Logik:

- **Kurz‑Druck (ca. < 1 s)**
  - **Wenn System läuft (`SYSTEM_RUN`)**:
    - Der Druck wirkt wie ein **Not‑Stopp**:
      - `g_SystemState` wird auf `SYSTEM_EMERGENCY_STOP` gesetzt.
      - `All_Motors_Stop_Immediate()` stoppt alle Motoren sofort.
  - **Wenn System steht (`SYSTEM_EMERGENCY_STOP`)**:
    - Der Druck startet einen neuen Durchlauf:
      - `g_RestartFromTop = 1`
      - `g_SystemState = SYSTEM_RUN`

- **Lang‑Druck (> 1 s) im Stopp‑Zustand**
  - Das Display wird neu initialisiert:
    - `Display_Reinit()` führt einen ILI9341‑Reset und Re‑Init aus.
    - `g_RestartFromTop` wird zurückgesetzt.
  - Das System bleibt im Zustand `SYSTEM_EMERGENCY_STOP`; der Benutzer kann danach wie gewohnt über kurzen Druck neu starten.

Diese Logik sorgt dafür, dass **Start/Stop** und ein **Display‑Reset** vollständig über den einen Drehencoder‑Taster bedient werden können.

---

### Hinweise zur Anpassung

- Die Konstante `DISPLAY_ENCODER_CONNECTED` in `main.c` steuert, ob Display und Encoder aktiv genutzt werden (`1`) oder ob ein Betrieb ohne Frontend vorgesehen ist (`0` mit vereinfachtem Testablauf).  
- Drehgeschwindigkeit, Schrittanzahl und Pausen der Motoren können in `motor_logic.c` über Konstanten wie `FULL_TURN_TIME_MS`, `HALF_TURN_TIME_MS` und `PAUSE_BETWEEN_MS` angepasst werden.  
- Die maximal/minimal zulässige Spieleranzahl kann in `Menu_Select_Player_Count()` geändert werden (derzeit 2–8).

---

### Lizenz

Siehe ggf. die von ST bereitgestellte Lizenzinformation im Kopf von `main.c` bzw. in einer vorhandenen `LICENSE`‑Datei. Ohne separate Lizenz gilt die dort angegebene **AS‑IS**‑Bereitstellung.

