#include <Ticker.h>

#include <B31DGMonitor.h>
Ticker ticker;

B31DGCyclicExecutiveMonitor monitor;


#define FRAME_DURATION_MS 4     // 4ms


int task_1_led = 25; //output port for LED of task 1 
int task_2_frq = 12;//input port to measure task-2 frequency
int task_3_frq = 14;//input port to measure task-3 frequency
int task4_ptn = 26;//input port to show analog frequency
int task_4_led_err = 27;//output port to blink the led for error using potentiometer


unsigned long frameTime = 0;//Initializing frameTimer
unsigned long frameCounter = 0;//Initializing frameCounter
unsigned int counter = 0;

//----------------------------------------------------------------------------------------------------------------------------------------------


void frame() {
   unsigned int slot = frameCounter % 10;
   
   unsigned long frameTime = millis();
  while ((millis() - frameTime) < FRAME_DURATION_MS) 
  {
    // Do nothing !!!
  }

  // Increase frame counter and reset it after 10 frames

   switch (slot) {
     case 0: JobTask1();           JobTask3();           break;
     case 1: JobTask1();JobTask2();                      break;
     case 2: JobTask1();           JobTask3();           break;
     case 3: JobTask1();                      JobTask4();break;
     case 4: JobTask1();           JobTask3();           break;
     case 5: JobTask1();JobTask2();                      break;
     case 6: JobTask1();           JobTask3();           break;
     case 7: JobTask1();                      JobTask4();break;
     case 8: JobTask1();           JobTask3();           break;
     case 9: JobTask1();                                       
  }
  
  frameCounter++; 

    
}


//----------------------------------------------------------------------------------------------------------------------------------------------


void setup(void)
{
  monitor.startMonitoring();
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Ready");
  pinMode(task_1_led, OUTPUT); // output task1 led 
  pinMode(task_2_frq, INPUT); // input task2 pin measure frequency
  pinMode(task_3_frq, INPUT); // input task3 pin measure frequency
  pinMode(task4_ptn, INPUT); // input task4 pin for potentiometer
  pinMode(task_4_led_err, OUTPUT); // output task4 led to display error
}


//----------------------------------------------------------------------------------------------------------------------------------------------


void loop(void) // Single time slot function of the Cyclic Executive (repeating)
{
  
frame();// TO-DO: wait the next frame  
ticker.attach_ms(4,frame);
}


//----------------------------------------------------------------------------------------------------------------------------------------------
// Cyclic executive tasks
//----------------------------------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------------------------

// Task 1, takes 1ms once  every 4ms

void JobTask1(void) 
{
  monitor.jobStarted(1);
  
  digitalWrite(task_1_led, HIGH); // set pin 2 high for 200us
  delayMicroseconds(200);
  digitalWrite(task_1_led, LOW); // set pin 2 low for 50us
  delayMicroseconds(50);
  digitalWrite(task_1_led, HIGH); // set pin 2 high for 30us
  delayMicroseconds(30);
  digitalWrite(task_1_led, LOW); // set pin 2 low for remaining period
  
  monitor.jobEnded(1);
} 


//----------------------------------------------------------------------------------------------------------------------------------------------

// Task 2, takes 3ms once every 20ms

void JobTask2(void) 
{
   monitor.jobStarted(2);
   
  int count = 0;
    count += pulseIn(task_2_frq, HIGH); // measure the pulse width of the input signal

  count = count*2;
  float frequency_1 = 1000000.0 / (count); // calculate frequency in Hz
  frequency_1 = constrain(frequency, 333, 1000); // frequency between 333 and 1000 Hz
  //int scaled_frequency = map(frequency, 333, 1000, 0, 99);
   Serial.println("Frequency_1:");
   Serial.println(frequency_1); // output frequency 
  
  monitor.jobEnded(2); 
} 


//----------------------------------------------------------------------------------------------------------------------------------------------


// Task 3, takes 3ms once every 8ms

void JobTask3(void) 
{
  monitor.jobStarted(3);

  int count2 = 0;

    count2 += pulseIn(task_3_frq, HIGH); // measure the pulse width of the input signal

 count2 = count2*2;
  float frequency_2 = 1000000.0 / (count2 ); // calculate frequency in Hz
  frequency_2 = constrain(frequency2, 500, 1000); // frequency between 500 and 1000 Hz
  //int scaled_frequency2 = map(frequency2, 500, 1000, 0, 99);
  Serial.println("Frequency_2:"); 
  Serial.println(frequency_2); // output frequency 

  monitor.jobEnded(3);
  Serial.println("Task 3 Done");
} 


//----------------------------------------------------------------------------------------------------------------------------------------------


// Task 4, takes 1ms once every 20ms

void JobTask4(void) 
{
  monitor.jobStarted(4);
  Serial.println("Task 4 Started");
  const int max_analog_input = 1023;
  const int num_readings = 4;
  int readings[num_readings];
  int indx = 0;
  int total = 0;
  int fltr_val = 0;
  for (int i = 0; i < num_readings; i++) 
  {
  readings[i] = 0;
  }
  
  int analogValue = analogRead(task4_ptn);   // Read analog input
  total = total - readings[indx]; // Subtract the oldest reading from the total
  total = total + analogValue;   // Add to the total
  readings[indx] = analogValue;  // Store reading in the readings array
  indx++;  // Increment the indx
  
  // Wrap the indx if it exceeds the number of readings
  if (indx >= num_readings)
   {
    indx = 0;
   }
   
  fltr_val = total / num_readings; // Compute the filtered value of 4 readings
  
  // If the filtered value is greater than half of the maximum range, turn the led on esle led off
  
  if (fltr_val > max_analog_input / 2) {
    digitalWrite(task_4_led_err, HIGH);
  } 
  else {
    digitalWrite(task_4_led_err, LOW);
  }

  Serial.println(fltr_val);
  
  monitor.jobEnded(4);
}


//----------------------------------------------------------------------------------------------------------------------------------------------


// Task 5, takes 1ms once every 100ms

void JobTask5(void) 
{

monitor.jobStarted(5);

  Serial.println("Task 5 started");//Start of Task 5
  int task_2_frq = 0;
  int task_3_frq = 0;
  
    // Measure the frequency of Task 2 signal
  task_2_frq = pulseIn(task_2_frq, HIGH, 20000) == 0 ? 0 : 1000000 / pulseIn(task_2_frq, HIGH, 20000);
  task_2_frq = map(task_2_frq, 333, 1000, 0, 99); // map frequency between to 0-99
 
  // Measure the frequency of Task 3 signal
  task_3_frq = pulseIn(task_3_frq, HIGH, 8000) == 0 ? 0 : 1000000 / pulseIn(task_2_frq, HIGH, 8000);
  task_3_frq = map(task_3_frq, 500, 1000, 0, 99); // map frequency value between 0-99
  
  // Send the frequency values to the serial port
  Serial.println(task_2_frq); //To print frequency of Task2
  Serial.println(task_3_frq); //To print frequency of Task3

  monitor.jobStarted(5);
}
