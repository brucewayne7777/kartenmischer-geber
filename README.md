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
  - STM32F3‑Nucleo oder eigenes STM32F3‑Board mit den im Projekt verwendeten Pins (siehe `main.h` / `.ioc`).
  - ILI9341‑SPI‑Display (Pins: `LCD_CS`, `LCD_DC`, `LCD_RST`, SPI1‑Signale).
  - Drehencoder mit Taster:
    - A/B‑Signale am Timer `TIM8` (Encoder‑Mode)
    - Taster `ENC_SW` an einem GPIO‑Eingang mit Pull‑Up.
  - Lichtschranken an den definierten Eingängen (`Lichtschranke_1/2/3/4_*`).
  - Motor‑Treiber (z.B. L298N + DRV8825) wie im Projekt verdrahtet.

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

