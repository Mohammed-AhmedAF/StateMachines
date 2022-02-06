
enum event {
  ON,
  OFF 
};

typedef enum
{
  LIGHT_OFF,
  LIGHT_DIM,
  LIGHT_MEDIUM,
  LIGHT_FULL
} light_state;
void light_state_machine(uint8_t event);
void light_change_intensity(uint8_t ledPin, uint8_t intensity);

#define PIN_LED 9

#define LIGHT_BRIGHT_DIM 25
#define LIGHT_BRIGHT_MED 85
#define LIGHT_BRIGHT_FULL 255
#define LIGHT_BRIGHT_OFF 0

uint8_t current_state = LIGHT_OFF;

void light_state_machine(uint8_t event)
{
  switch(current_state)
  {
    case LIGHT_OFF:
    switch(event)
    {
     case ON:
     light_change_intensity(PIN_LED,LIGHT_BRIGHT_DIM);
     current_state = LIGHT_DIM;
     break;
     case OFF:
     break; 
    }
    break;
    case LIGHT_DIM:
    switch(event)
    {
     case ON:
     light_change_intensity(PIN_LED,LIGHT_BRIGHT_MED);
     current_state = LIGHT_MEDIUM;
     break;
     case OFF:
     light_change_intensity(PIN_LED,LIGHT_BRIGHT_OFF);
     current_state = LIGHT_OFF;
     break; 
    }
    break;
    case LIGHT_MEDIUM:
    switch(event)
    {
     case ON:
     light_change_intensity(PIN_LED,LIGHT_BRIGHT_FULL);
     current_state = LIGHT_FULL;
     break;
     case OFF:
     light_change_intensity(PIN_LED,LIGHT_BRIGHT_DIM);
     current_state = LIGHT_DIM;
     break; 
    }
    break;
    case LIGHT_FULL:
    switch(event)
    {
     case ON:
     light_change_intensity(PIN_LED,LIGHT_BRIGHT_OFF);
     current_state = LIGHT_OFF;
     break;
     case OFF:
     break; 
    }
    break;
  }  
}

void light_change_intensity(uint8_t ledPin, uint8_t intensity)
{
 analogWrite(ledPin,intensity); 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t event;
  if(Serial.available() > 0)
  {
    event = Serial.read(); 
    if (event == 'o')
    {
      light_state_machine(ON);  
    }
    else if(event == 'x')
      {
      light_state_machine(OFF);  
      }
  }
  
   
}
