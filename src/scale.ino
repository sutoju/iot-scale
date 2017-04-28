void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(A0, INPUT);
}

unsigned int read_adc() {
  return (unsigned int) analogRead(A0);
}

unsigned int read_scale() {
  unsigned int v = 0;
  unsigned int n = 20;
  
  for(unsigned int i = 0; i < n; ++i) {
    delay(20);
    v += read_adc();
  }
  
  v /= n;
  return v;
}

double adc2g(unsigned int value) {
  unsigned int scale_offset = 201;
  double scale_factor = 11.9;

  unsigned delta = value - scale_offset;
  Serial.print("Delta:" );
  Serial.println(delta);

  return delta*scale_factor;
}

double get_weight() {
  unsigned int adc_value = read_scale();
  Serial.print("ADC: ");
  Serial.println(adc_value);

  double weight = adc2g(adc_value);
  return weight;
}

void loop() {
  double w = get_weight();
  Serial.print("Weight (g): ");
  Serial.println(w);
  
  delay(500);
}