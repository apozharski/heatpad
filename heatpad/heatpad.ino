
constexpr double LOW_TEMP = 30;

constexpr double HIGH_TEMP = 32.5;

enum state_t {
  START,
  TLOW,
  TRANGE,
  THIGH,
  TFAIL
};

state_t curr_state = START;
state_t prev_state = START; 

int t_to_s(double temp)
{
  return (int)((-9.49378*temp) + 884);
}

double s_to_t(int sense)
{
  return (double)((-.105*sense) + 92.94);
}

void transition(int sense)
{
  double temp = s_to_t(sense);
  prev_state = curr_state;
  if(temp < LOW_TEMP)
    curr_state = TLOW;
  else if(temp > HIGH_TEMP)
    curr_state = THIGH;
  else
    curr_state = TRANGE;
}

void apply_changes()
{
  switch(prev_state)
  {
   case START:
     switch(curr_state)
     {
      case TLOW:
        digitalWrite(12, HIGH);
        return;
      default:
        curr_state = TFAIL;
        prev_state = TFAIL;
        return;
     };
     break;
   case TLOW:
     break;
   case TRANGE:
     switch(curr_state)
     {
      case TLOW:
        digitalWrite(12, HIGH);
        return;
      case THIGH:
        digitalWrite(12, LOW);
        return;
      default:
        return;
     };
     break;
   case THIGH:
     break;
   case TFAIL:
     break;
  };
  return;
}

void setup()
{
  pinMode(12, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //Serial.begin(9600);
}

int STATE = HIGH;
void loop() {
  int sense = analogRead(A7);
  // Serial.print(sense);
  // Serial.print(" ");
  // Serial.print(s_to_t(sense));
  
  if(curr_state != TFAIL)
  {
    transition(sense);
    apply_changes();
    delay(1000);
  }
  else
  {
    delay(100);
  }
  // Serial.print(" ");
  // Serial.println(curr_state);
  STATE = !STATE;
  digitalWrite(LED_BUILTIN, STATE);
}
