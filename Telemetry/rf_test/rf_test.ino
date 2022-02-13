#define autonomousStop     3
#define autonomousStart    4

void setup()
{
  pinMode(autonomousStart, INPUT_PULLUP);
  pinMode(autonomousStop, INPUT_PULLUP);
  Serial1.begin(115200); // RF
}

void loop()
{
  if (!digitalRead(autonomousStart))
  {
    Serial1.write(1);
    while (!digitalRead(autonomousStart));
  }
  else if (!digitalRead(autonomousStop))
  {
    Serial1.write(2);
    while (!digitalRead(autonomousStop));
  }
}
