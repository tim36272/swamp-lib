#define STATE_TRANSITION_TIMEOUT 1000
#include <libHomeMesh.h>

typedef enum {
  COMMAND_STATE_IDLE,
  COMMAND_STATE_FIRST_HIGH,
  COMMAND_STATE_FIRST_LOW,
  COMMAND_STATE_ENGAGE
}commandState_t;

//pin: The pin to check
//state: the current state
//stateTimestamp: the time the state was entered
void checkForPinSignal(int pin, commandState_t* state, unsigned long* stateTimestamp) {
  int command;
	command = digitalRead(pin);
  switch(*state) {
    case COMMAND_STATE_IDLE:
      if (command == HIGH) {
        *stateTimestamp = millis();
        *state = COMMAND_STATE_FIRST_HIGH;
      }
      break;
    case COMMAND_STATE_FIRST_HIGH:
      if (command == LOW) {
        *stateTimestamp = millis();
        *state = COMMAND_STATE_FIRST_LOW;
      } else if (millis() - *stateTimestamp > STATE_TRANSITION_TIMEOUT) {
        *state = COMMAND_STATE_IDLE;
      }
      break;
    case COMMAND_STATE_FIRST_LOW:
      if (command == HIGH) {
          *state = COMMAND_STATE_ENGAGE;
      }else if (millis() - *stateTimestamp > STATE_TRANSITION_TIMEOUT) {
        *state = COMMAND_STATE_IDLE;
      }
  }
}

//duration: in millisecons
void acknowledge(uint8_t duration) {
  //blink the light and at the same time block further commands for six seconds since the transmitter may retransmit the same message many times
  uint8_t i;
  for (i = 0; i < (duration / 200); i++) { 
    digitalWrite(13,1);
    delay(100);
    digitalWrite(13,0);
    delay(100);
  }
}

void transmitCommand(int command, unsigned int duration) {
  acknowledge(1000);
  uint8_t i;
  //each transmission takes about 99 ms, so with a 10 ms delay this transmits for five seconds
  //transmit the signal for duration seconds
  for (i=0;i<50;i++) {
    man.transmit(man.encodeMessage(1, command));
  }
  //signal that the command worked
  acknowledge(2000);
}


