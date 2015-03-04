#include <StandardCplusplus.h>
#include <vector>
#include <Char2Array.h>
#include <Arduino.h>
#include "EEPROM.h"
#include "DB.h"


Char2Array converter;


const int clr = 2;
const int data = 4;
const int clock = 5;
const int latch = 6;
const int blank = 7;

const int button_pin = 3;
const int led_pin = 12;

const int rows = 6;
int shift_count = rows - 1;

//Shifts the register by one, also loops when needed
void shift(){
  if(shift_count == rows){shift_count = 0;}
  if(shift_count == rows - 1){
    digitalWrite(data, HIGH);
    digitalWrite(clock, HIGH);
    digitalWrite(data, LOW);
    digitalWrite(clock, LOW);
    
    shift_count = 0;
  } 
  else{
    digitalWrite(clock, HIGH);
    digitalWrite(clock, LOW);
    
    shift_count += 1;
  }
  
  digitalWrite(latch, HIGH);
  digitalWrite(latch, LOW);
  
}

//Shifts the register to a given position
void shift_to(int pos){
  if(pos < rows && pos >= 0){
  digitalWrite(blank, HIGH);
  while(pos != shift_count){shift();} 
  digitalWrite(blank, LOW);
  }
}

// Resets the shift register to position one, guaranteed to work
void shift_reset(){
  digitalWrite(clr, LOW);
  delay(5);
  digitalWrite(clr, HIGH);
  shift_count = rows - 1;
  shift();
}

//Gets the next complete input from serial
//All user input must end with >, known as "end of message" character
//If multiple use inputs are made, it will collect both
String wait_for_next(){
    char temp_byte;
    String temp = "";
    //Waits until user inputs
    while(Serial.available() == 0){}
    //Loops until the end of message character is seen (>)
    while(Serial.peek() != '>'){
        temp_byte = Serial.read();
        temp += String(temp_byte);
        //Waits until next character is available in stream
        while(Serial.available() == 0){}
    }
    //Don't know why this works, sorta clears the bufferish??? Just don't touch it.
    //Seriously don't remove it
    //Not joking
    while(Serial.available() > 0){Serial.read();}
    //Whew made it to the other side
    
    return temp;
}



void admin_mode(){
  Serial.begin(9600);
  
   while(Serial.available() == 0){
       Serial.println("Welcome to admin mode. Enter any input to begin. Remember to end all input with '>'.");
       digitalWrite(led_pin, HIGH);
       delay(150);
       digitalWrite(led_pin, LOW);
       delay(150);
       digitalWrite(led_pin, HIGH);
       delay(750);
       digitalWrite(led_pin, LOW);
   } 
   digitalWrite(led_pin, HIGH); 
   while(wait_for_next() != "exit"){
     Serial.println("Enter 'exit' to exit");}
     digitalWrite(led_pin, LOW);
     
  Serial.end();
}

void check_for_button(){
    boolean pressed = false;
    unsigned long start = millis();
    digitalWrite(led_pin, HIGH);
    while(millis() - start < 10000){
        if(digitalRead(button_pin)){pressed = true; break;} 
        digitalWrite(led_pin, HIGH);
        delay(500);
        if(digitalRead(button_pin)){pressed = true; break;} 
        digitalWrite(led_pin, LOW);
        delay(500);
    }
    digitalWrite(led_pin, LOW);
    if(pressed){admin_mode();}
}

void setup(){
   pinMode(led_pin, OUTPUT);
   pinMode(button_pin, INPUT);
   
   pinMode(latch, OUTPUT);
   pinMode(data, OUTPUT);
   pinMode(clock, OUTPUT);
   pinMode(blank, OUTPUT);
   pinMode(clr, OUTPUT);
   
   digitalWrite(clr, HIGH);
   shift_reset();
   
   
   Serial.begin(9600);
   //check_for_button();
 }


void loop(){
   shift_to(wait_for_next().toInt());
}
