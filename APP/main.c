/* AVR includes */
#include <avr/io.h>
#include <util/delay.h>

/* Driver includes */
#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"
#include "../MCAL/DIO/DIO_int.h"
#include "../MCAL/ADC/ADC_int.h"
#include "../MCAL/USART/USART_int.h"
#include "../HAL/LCD/LCD_int.h"

/* EEPROM access macros for ATmega32 */
#define EEPROM_write(address,value) \
    do { \
        while(EECR & (1<<EEWE)); \
        EEAR = address; \
        EEDR = value; \
        EECR |= (1<<EEMWE); \
        EECR |= (1<<EEWE); \
    } while(0)


u8 EEPROM_read_byte(u16 address)
{
    while(EECR & (1<<EEWE)); // Wait for completion
    EEAR = address;
    EECR |= (1<<EERE);
    return EEDR;
}

/* Hardware Pin Definitions - From working bluetooth test */
#define LED_LIVING_ROOM_PORT    DIO_PORTA
#define LED_LIVING_ROOM_PIN     DIO_PIN3
#define LED_BEDROOM_PORT        DIO_PORTA
#define LED_BEDROOM_PIN         DIO_PIN4
#define BUZZER_PORT             DIO_PORTD
#define BUZZER_PIN              DIO_PIN4
// Fan (DC Motor) control
#define FAN_PORT                DIO_PORTC
#define FAN_PIN                 DIO_PIN7

/* Button Controls */
#define BTN_LIVING_ROOM_PORT    DIO_PORTD
#define BTN_LIVING_ROOM_PIN     DIO_PIN7
#define BTN_BEDROOM_PORT        DIO_PORTD
#define BTN_BEDROOM_PIN         DIO_PIN6
#define BTN_SECURITY_PORT       DIO_PORTD
#define BTN_SECURITY_PIN        DIO_PIN5

/* DIP Switch Override Controls - Port A */
#define DIP_OVERRIDE1_PORT      DIO_PORTA
#define DIP_OVERRIDE1_PIN       DIO_PIN6    // Master Override - Disable ALL Bluetooth commands
#define DIP_OVERRIDE2_PORT      DIO_PORTA
#define DIP_OVERRIDE2_PIN       DIO_PIN7    // Lock Mode - Only buttons work, no Bluetooth

// LDR sensor and LED for darkness detection
#define LDR_PORT                DIO_PORTA
#define LDR_PIN                 DIO_PIN5
#define DARK_LED_PORT           DIO_PORTD
#define DARK_LED_PIN            DIO_PIN3


/* System State - From working bluetooth test */
u8 living_room_light = 0;
u8 bedroom_light = 0;
u8 security_alarm = 0;
u8 fan_on = 0; // Fan state

/* Override States */
u8 master_override = 0;     // DIP1 - Complete system override
u8 lock_mode = 0;           // DIP2 - Button-only mode

/* EEPROM Logging */
#define EEPROM_LOG_START_ADDR   0x00
#define MAX_LOG_ENTRIES         20
u8 log_index = 0;

/* Button debouncing */
u8 btn_living_old = 1, btn_bedroom_old = 1, btn_security_old = 1;

/* System counters */
u8 command_count = 0;
u16 system_uptime = 0;
// Structure for status snapshot
typedef struct {
    u8 living_room_light;
    u8 bedroom_light;
    u8 security_alarm;
    u8 fan_on;
    u8 master_override;
    u8 lock_mode;
    u8 command_count;
    u16 system_uptime;
} StatusSnapshot;

// EEPROM address for status snapshots
#define EEPROM_STATUS_START_ADDR  200
#define MAX_STATUS_SNAPSHOTS      20
u8 status_snapshot_index = 0;

void Save_Status_Snapshot(void)
{
    u16 addr = EEPROM_STATUS_START_ADDR + (status_snapshot_index * sizeof(StatusSnapshot));
    StatusSnapshot snap;
    snap.living_room_light = living_room_light;
    snap.bedroom_light = bedroom_light;
    snap.security_alarm = security_alarm;
    snap.fan_on = fan_on;
    snap.master_override = master_override;
    snap.lock_mode = lock_mode;
    snap.command_count = command_count;
    snap.system_uptime = system_uptime;

    // Write each byte to EEPROM
    u8* ptr = (u8*)&snap;
    for(u8 i = 0; i < sizeof(StatusSnapshot); i++) {
        EEPROM_write(addr + i, ptr[i]);
    }

    status_snapshot_index = (status_snapshot_index + 1) % MAX_STATUS_SNAPSHOTS;
    EEPROM_write(250, status_snapshot_index); // Store index at address 250
}

void Send_Status_Snapshots(void)
{
    SendString((u8*)
"\r\n==== STATUS SNAPSHOTS ====\r\n");
    u8 idx = EEPROM_read_byte(250);
    if(idx >= MAX_STATUS_SNAPSHOTS) idx = 0;
    u8 show_count = MAX_STATUS_SNAPSHOTS;
    u8 filled = idx;
    if(filled == 0) {
        SendString((u8*)"No status snapshots yet.\r\n");
        SendString((u8*)"====================\r\n");
        return;
    }
    if(filled < MAX_STATUS_SNAPSHOTS) show_count = filled;
    else show_count = MAX_STATUS_SNAPSHOTS;
    // Find the oldest snapshot
    u8 start_idx = (filled < MAX_STATUS_SNAPSHOTS) ? 0 : idx;
    for(u8 i = 0; i < show_count; i++) {
        u8 snap_idx = (start_idx + i) % MAX_STATUS_SNAPSHOTS;
        u16 addr = EEPROM_STATUS_START_ADDR + (snap_idx * sizeof(StatusSnapshot));
        StatusSnapshot snap;
        u8* ptr = (u8*)&snap;
        for(u8 j = 0; j < sizeof(StatusSnapshot); j++) {
            ptr[j] = EEPROM_read_byte(addr + j);
        }
        // Print snapshot
        SendString((u8*)"Snapshot ");
        MUSART_vTransmit('0' + (i + 1) / 10);
        MUSART_vTransmit('0' + (i + 1) % 10);
        SendString((u8*)
": ");
        SendString((u8*)"LivingRoom:");
        SendString(snap.living_room_light ? (u8*)"ON " : (u8*)"OFF ");
        SendString((u8*)"Bedroom:");
        SendString(snap.bedroom_light ? (u8*)"ON " : (u8*)"OFF ");
        SendString((u8*)"Security:");
        SendString(snap.security_alarm ? (u8*)"ON " : (u8*)"OFF ");
        SendString((u8*)"Fan:");
        SendString(snap.fan_on ? (u8*)"ON " : (u8*)"OFF ");
        SendString((u8*)"Mode:");
        if(snap.master_override) SendString((u8*)"MASTER_OVERRIDE ");
        else if(snap.lock_mode) SendString((u8*)"LOCK_MODE ");
        else SendString((u8*)"NORMAL ");
        SendString((u8*)"Cmds:");
        MUSART_vTransmit('0' + (snap.command_count / 10));
        MUSART_vTransmit('0' + (snap.command_count % 10));
        SendString((u8*)" Uptime:");
        MUSART_vTransmit('0' + (snap.system_uptime / 10));
        MUSART_vTransmit('0' + (snap.system_uptime % 10));
        SendString((u8*)" min\r\n");
    }
    SendString((u8*)"====================\r\n");
}

/* String transmission - From working USART test */
void SendString(u8* str)
{
    while(*str)
    {
        MUSART_vTransmit(*str);
        str++;
    }
}

/* Status function - From working bluetooth test */
void SendStatus(void)
{
    SendString((u8*)
"\r\n=== STATUS ===\r\n"
"Living Room: ");
    SendString(living_room_light ? (u8*)"ON" : (u8*)"OFF");
    SendString((u8*)"\r\nBedroom: ");
    SendString(bedroom_light ? (u8*)"ON" : (u8*)"OFF");
    SendString((u8*)"\r\nSecurity: ");
    SendString(security_alarm ? (u8*)"ON" : (u8*)"OFF");
    SendString((u8*)"\r\nFan: ");
    SendString(fan_on ? (u8*)"ON" : (u8*)"OFF");

    /* Show override status */
    SendString((u8*)"\r\nMode: ");
    if(master_override) {
        SendString((u8*)"MASTER OVERRIDE");
    } else if(lock_mode) {
        SendString((u8*)"LOCK MODE");
    } else {
        SendString((u8*)"NORMAL");
    }

    SendString((u8*)"\r\nCommands: ");
    MUSART_vTransmit('0' + (command_count / 10));
    MUSART_vTransmit('0' + (command_count % 10));
    SendString((u8*)"\r\nUptime: ");
    MUSART_vTransmit('0' + (system_uptime / 10));
    MUSART_vTransmit('0' + (system_uptime % 10));
    SendString((u8*)" min\r\n==============\r\n");
}

/* Hardware update - From working bluetooth test */
void UpdateHardware(void)
{
    /* Update LEDs */
    MDIO_vSetPinVal(LED_LIVING_ROOM_PORT, LED_LIVING_ROOM_PIN,
                    living_room_light ? DIO_HIGH : DIO_LOW);
    MDIO_vSetPinVal(LED_BEDROOM_PORT, LED_BEDROOM_PIN,
                    bedroom_light ? DIO_HIGH : DIO_LOW);

    // Update fan (DC motor)
    MDIO_vSetPinVal(FAN_PORT, FAN_PIN, fan_on ? DIO_HIGH : DIO_LOW);

    /* Update buzzer for security */
    if(security_alarm)
    {
        static u8 buzzer_toggle = 0;
        static u8 buzzer_counter = 0;

        if(++buzzer_counter >= 50)  // Toggle every 500ms
        {
            buzzer_counter = 0;
            buzzer_toggle = !buzzer_toggle;
            MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, buzzer_toggle ? DIO_HIGH : DIO_LOW);
        }
    }
    else
    {
        MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_LOW);
    }
        MADC_vInit();

        // Initialize LED for darkness indication
        MDIO_vSetPinDir(DARK_LED_PORT, DARK_LED_PIN, DIO_OUTPUT);
        /* LDR sensor logic: light up LED at PORTD3 when it gets dark */
        u16 ldr_value = MADC_u16AnalogRead(LDR_PIN);
        if(ldr_value < 400) // Threshold for darkness (adjust as needed)
        {
            MDIO_vSetPinVal(DARK_LED_PORT, DARK_LED_PIN, DIO_HIGH); // LED ON
        }
        else
        {
            MDIO_vSetPinVal(DARK_LED_PORT, DARK_LED_PIN, DIO_LOW); // LED OFF
        }
}

/* LCD update - From working bluetooth test */
void UpdateLCD(void)
{
    HLCD_vSetCursorPosition(0, 0);
    HLCD_vSendStr((u8*)"L:");
    HLCD_vSendStr(living_room_light ? (u8*)"ON " : (u8*)"OFF");
    HLCD_vSendStr((u8*)" B:");
    HLCD_vSendStr(bedroom_light ? (u8*)"ON " : (u8*)"OFF");
    HLCD_vSendStr((u8*)" F:");
    HLCD_vSendStr(fan_on ? (u8*)"ON" : (u8*)"OFF");

    HLCD_vSetCursorPosition(1, 0);
    if(master_override) {
        HLCD_vSendStr((u8*)"MASTER OVERRIDE ");
    } else if(lock_mode) {
        HLCD_vSendStr((u8*)"LOCK MODE      ");
    } else {
        HLCD_vSendStr((u8*)"Sec:");
        HLCD_vSendStr(security_alarm ? (u8*)"ON  " : (u8*)"OFF ");
        HLCD_vSendStr((u8*)"Cmd:");
        HLCD_vSendNumber(command_count);
    }
}

/* Simple EEPROM logging */
void Log_Event(u8 event_type, u8 data)
{
    u16 addr = EEPROM_LOG_START_ADDR + (log_index * 2);
    EEPROM_write(addr, event_type);
    EEPROM_write(addr + 1, data);

    log_index = (log_index + 1) % MAX_LOG_ENTRIES;
    EEPROM_write(100, log_index);  // Store index at address 100
}

/* Send logs via Bluetooth */
void Send_Logs(void)
{
    SendString((u8*)
"\r\n==== EVENT LOGS ====\r\n");


    if(log_index == 0)
    {
        SendString((u8*)"No events have been logged yet.\r\n");
    }
    else
    {
        u8 show_count = (log_index > 5) ? 5 : log_index;
        u8 start_idx = (log_index > 5) ? (log_index - 5) : 0;

        for(u8 i = 0; i < show_count; i++)
        {
            u8 eeprom_idx = (start_idx + i) % MAX_LOG_ENTRIES;
            u16 addr = EEPROM_LOG_START_ADDR + (eeprom_idx * 2);
            u8 type = EEPROM_read_byte(addr);
            u8 data = EEPROM_read_byte(addr + 1);

            // Print running event number
            SendString((u8*)"[Event ");
            MUSART_vTransmit('0' + (i + 1) / 10);
            MUSART_vTransmit('0' + (i + 1) % 10);
            SendString((u8*)"] ");

            // Print descriptive message
            switch(type) {
                case 1: // Light ON
                    if(data == 1) SendString((u8*)"Living Room Light turned ON");
                    else if(data == 2) SendString((u8*)"Bedroom Light turned ON");
                    else SendString((u8*)"All Lights turned ON");
                    break;
                case 2: // Light OFF
                    if(data == 1) SendString((u8*)"Living Room Light turned OFF");
                    else if(data == 2) SendString((u8*)"Bedroom Light turned OFF");
                    else SendString((u8*)"All Lights turned OFF");
                    break;
                case 3: // Security
                    if(data) SendString((u8*)"Security System ACTIVATED");
                    else SendString((u8*)"Security System DEACTIVATED");
                    break;
                case 4: // System/Fan
                    if(data == 0) SendString((u8*)"System Started");
                    else if(data == 1) SendString((u8*)"Fan turned ON");
                    else if(data == 2) SendString((u8*)"Fan turned OFF");
                    else SendString((u8*)"System Event");
                    break;
                case 5: // Override events
                    if(data == 1) SendString((u8*)"Master Override ENABLED");
                    else if(data == 2) SendString((u8*)"Lock Mode ENABLED");
                    else if(data == 3) SendString((u8*)"Override DISABLED");
                    else SendString((u8*)"Override Changed");
                    break;
                default:
                    SendString((u8*)"Unknown Event");
                    break;
            }

            SendString((u8*)"\r\n");
        }
    }

    SendString((u8*)"====================\r\n");
}

/* Read DIP switch override states */
void Check_Override_Switches(void)
{
    u8 old_master = master_override;
    u8 old_lock = lock_mode;

    /* Read DIP switches (active LOW with pull-ups) */
    master_override = !MDIO_u8GETPinVal(DIP_OVERRIDE1_PORT, DIP_OVERRIDE1_PIN);
    lock_mode = !MDIO_u8GETPinVal(DIP_OVERRIDE2_PORT, DIP_OVERRIDE2_PIN);

    /* Master override takes priority over lock mode */
    if(master_override) {
        lock_mode = 0;  // Master override disables lock mode
    }

    /* Log override state changes */
    if(!old_master && master_override) {
        Log_Event(5, 1);  // Master override activated
        SendString((u8*)"[OVERRIDE] MASTER OVERRIDE ACTIVATED!\r\n");
        SendString((u8*)"All Bluetooth commands DISABLED\r\n");
    }
    else if(old_master && !master_override) {
        Log_Event(5, 3);  // Override deactivated
        SendString((u8*)"[OVERRIDE] Master override OFF\r\n");
    }

    // Replace unused lock_mode with fan control
    static u8 old_fan_switch = 0;
    if(!old_lock && lock_mode && !master_override) {
        fan_on = 1;
        Log_Event(4, 1); // Fan turned ON
        SendString((u8*)"[OVERRIDE] Fan ON via DIP2\r\n");
    }
    else if(old_lock && !lock_mode && !master_override) {
        fan_on = 0;
        Log_Event(4, 2); // Fan turned OFF
        SendString((u8*)"[OVERRIDE] Fan OFF via DIP2\r\n");
    }
}

/* Process physical buttons */
void Process_Buttons(void)
{
    u8 btn_living = MDIO_u8GETPinVal(BTN_LIVING_ROOM_PORT, BTN_LIVING_ROOM_PIN);
    u8 btn_bedroom = MDIO_u8GETPinVal(BTN_BEDROOM_PORT, BTN_BEDROOM_PIN);
    u8 btn_security = MDIO_u8GETPinVal(BTN_SECURITY_PORT, BTN_SECURITY_PIN);

    /* Living room button */
    if(btn_living == 0 && btn_living_old == 1)
    {
        living_room_light = !living_room_light;
        Log_Event(living_room_light ? 1 : 2, 1);
        SendString((u8*)"[BTN] Living Room ");
        SendString(living_room_light ? (u8*)"ON\r\n" : (u8*)"OFF\r\n");
        MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_HIGH);
        _delay_ms(60);
        MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_LOW);
        command_count++;
    }
    /* Bedroom button */
    if(btn_bedroom == 0 && btn_bedroom_old == 1)
    {
        bedroom_light = !bedroom_light;
        Log_Event(bedroom_light ? 1 : 2, 2);
        SendString((u8*)"[BTN] Bedroom ");
        SendString(bedroom_light ? (u8*)"ON\r\n" : (u8*)"OFF\r\n");
        MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_HIGH);
        _delay_ms(60);
        MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_LOW);
        command_count++;
    }
    /* Security button */
    if(btn_security == 0 && btn_security_old == 1)
    {
        security_alarm = !security_alarm;
        Log_Event(3, security_alarm ? 1 : 0);
        if(security_alarm)
        {
            MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_HIGH);
            _delay_ms(60);
            MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_LOW);
            SendString((u8*)"[BTN] SECURITY ACTIVATED!\r\n");
        }
        else
        {
            SendString((u8*)"[BTN] Security OFF\r\n");
            MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_HIGH);
            _delay_ms(60);
            MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_LOW);
        }
        command_count++;
    }

    /* Store previous states */
    btn_living_old = btn_living;
    btn_bedroom_old = btn_bedroom;
    btn_security_old = btn_security;
}

int main(void)
{
    u8 received_char = 0;

    /* Initialize hardware - From working tests */
    MDIO_vInit();

    HLCD_vInit();
    _delay_ms(50); // Wait for LCD to power up
    HLCD_vClearScreen();
    _delay_ms(10);
    HLCD_vSetCursorPosition(0, 0);
    HLCD_vSendStr((u8*)"Smart Home Ready");
    HLCD_vSetCursorPosition(1, 0);
    HLCD_vSendStr((u8*)"Send commands...");
    _delay_ms(2000);

    MUSART_vInit();
    // Initialize fan pin as output
    MDIO_vSetPinDir(FAN_PORT, FAN_PIN, DIO_OUTPUT);
    /* Configure LED and buzzer pins as outputs - From working bluetooth test */
    MDIO_vSetPinDir(LED_LIVING_ROOM_PORT, LED_LIVING_ROOM_PIN, DIO_OUTPUT);
    MDIO_vSetPinDir(LED_BEDROOM_PORT, LED_BEDROOM_PIN, DIO_OUTPUT);
    MDIO_vSetPinDir(BUZZER_PORT, BUZZER_PIN, DIO_OUTPUT);
    /* Configure button inputs with pull-ups */
    MDIO_vSetPinDir(BTN_LIVING_ROOM_PORT, BTN_LIVING_ROOM_PIN, DIO_INPUT);
    MDIO_vSetPinDir(BTN_BEDROOM_PORT, BTN_BEDROOM_PIN, DIO_INPUT);
    MDIO_vSetPinDir(BTN_SECURITY_PORT, BTN_SECURITY_PIN, DIO_INPUT);
    MDIO_vSetPinVal(BTN_LIVING_ROOM_PORT, BTN_LIVING_ROOM_PIN, DIO_HIGH);
    MDIO_vSetPinVal(BTN_BEDROOM_PORT, BTN_BEDROOM_PIN, DIO_HIGH);
    MDIO_vSetPinVal(BTN_SECURITY_PORT, BTN_SECURITY_PIN, DIO_HIGH);
    /* Configure DIP switch inputs with pull-ups */
    MDIO_vSetPinDir(DIP_OVERRIDE1_PORT, DIP_OVERRIDE1_PIN, DIO_INPUT);
    MDIO_vSetPinDir(DIP_OVERRIDE2_PORT, DIP_OVERRIDE2_PIN, DIO_INPUT);
    MDIO_vSetPinVal(DIP_OVERRIDE1_PORT, DIP_OVERRIDE1_PIN, DIO_HIGH);
    MDIO_vSetPinVal(DIP_OVERRIDE2_PORT, DIP_OVERRIDE2_PIN, DIO_HIGH);
    /* Initialize EEPROM logging */
    log_index = EEPROM_read_byte(100);
    if(log_index >= MAX_LOG_ENTRIES) {
        log_index = 0;
        EEPROM_write(100, log_index);
    }
        /* Initialize status snapshot index */
        status_snapshot_index = EEPROM_read_byte(250);
        if(status_snapshot_index >= MAX_STATUS_SNAPSHOTS) {
            status_snapshot_index = 0;
            EEPROM_write(250, status_snapshot_index);
        }
    /* Initialize all outputs to OFF - From working bluetooth test */
    living_room_light = 0;
    bedroom_light = 0;
    security_alarm = 0;
    UpdateHardware();

    /* Send startup message via Bluetooth - From working bluetooth test */
    SendString((u8*)"=== SMART HOME SYSTEM READY ===\r\n");
    SendString((u8*)"Commands:\r\n");
    SendString((u8*)"1 = Living Room ON\r\n");
    SendString((u8*)"2 = Living Room OFF\r\n");
    SendString((u8*)"3 = Bedroom ON\r\n");
    SendString((u8*)"4 = Bedroom OFF\r\n");
    SendString((u8*)"5 = All Lights ON\r\n");
    SendString((u8*)"6 = All Lights OFF\r\n");
    SendString((u8*)"7 = Security ON\r\n");
    SendString((u8*)"8 = Security OFF\r\n");
    SendString((u8*)"S = Status, L = Logs, H = Help\r\n");
    SendString((u8*)"--- OVERRIDE FEATURES ---\r\n");
    SendString((u8*)"DIP Switch 1: Master Override\r\n");
    SendString((u8*)"DIP Switch 2: Lock Mode\r\n");
    SendString((u8*)"Physical buttons always work!\r\n");
    SendString((u8*)"===============================\r\n");

    /* Initial status display */
    SendStatus();
    UpdateLCD();

    /* Log system startup */
    Log_Event(4, 0);

    // Timer for status snapshot
    static u16 status_timer = 0;
    while(1)
    {
        _delay_ms(100);
        UpdateLCD();
        Check_Override_Switches();
        Process_Buttons();
        UpdateHardware();

        // Periodically save status snapshot every ~5 seconds
        status_timer += 100;
        if(status_timer >= 5000) {
            status_timer = 0;
            Save_Status_Snapshot();
        }

        // USART receive logic
        if(UCSRA & (1 << RXC))
        {
            received_char = UDR;
            if(!master_override)
            {
                u8 action_taken = 0;
                switch(received_char)
                {
                    case '1': living_room_light = 1; Log_Event(1, 1); command_count++; action_taken = 1; break;
                    case '2': living_room_light = 0; Log_Event(2, 1); command_count++; action_taken = 1; break;
                    case '3': bedroom_light = 1; Log_Event(1, 2); command_count++; action_taken = 1; break;
                    case '4': bedroom_light = 0; Log_Event(2, 2); command_count++; action_taken = 1; break;
                    case '5': living_room_light = 1; bedroom_light = 1; Log_Event(1, 3); command_count++; action_taken = 1; break;
                    case '6': living_room_light = 0; bedroom_light = 0; Log_Event(2, 3); command_count++; action_taken = 1; break;
                    case '7': security_alarm = 1; Log_Event(3, 1); command_count++; action_taken = 1; break;
                    case '8': security_alarm = 0; Log_Event(3, 0); command_count++; action_taken = 1; break;
                    case 'F': fan_on = 1; Log_Event(4, 1); command_count++; action_taken = 1; break;
                    case 'f': fan_on = 0; Log_Event(4, 2); command_count++; action_taken = 1; break;
                    case 'S': SendStatus(); break;
                    case 'L': Send_Status_Snapshots(); break;
                    case 'H':
                        SendString((u8*)"Help: 1-8=Control, F/f=Fan, S=Status, L=Logs\r\n");
                        break;
                    default: break;
                }
                if(action_taken) {
                    MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_HIGH);
                    _delay_ms(60); // brief peep
                    MDIO_vSetPinVal(BUZZER_PORT, BUZZER_PIN, DIO_LOW);
                }
            }
        }

        // Uptime counter (every ~1s)
        static u8 lcd_counter = 0;
        static u16 uptime_counter = 0;
        if(++lcd_counter >= 10)
        {
            lcd_counter = 0;
            if(++uptime_counter >= 60000)
                uptime_counter = 0;
            system_uptime = uptime_counter / 10; // ~minutes
        }
    }
    return 0;
}